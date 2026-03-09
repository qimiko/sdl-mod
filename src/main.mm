#include <Geode/Geode.hpp>

#include "base.h"
#include "objc.h"
#include "bind.h"

#define ENABLE_VKMOD 0

#if ENABLE_VKMOD
#include <dlfcn.h>
#include "vkmod/Vulkan.hpp"
#endif

SDLManager& SDLManager::get() {
	static SDLManager manager;
	return manager;
}

void trigger_shutdown() {
	[[AppController2 sharedAppController] shutdownGame];
}

void fix_color_space() {
	auto props = SDL_GetWindowProperties(SDLManager::get().m_window);
	if (auto nsWindow = reinterpret_cast<NSWindow*>(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr))) {
		[nsWindow setColorSpace:[NSColorSpace displayP3ColorSpace]];
	}
}

void resize_window(int w, int h) {
	cocos2d::CCEGLView::sharedOpenGLView()->setFrameSize(w, h);
	cocos2d::CCDirector::sharedDirector()->updateScreenScale({static_cast<float>(w), static_cast<float>(h)});
	cocos2d::CCDirector::sharedDirector()->setViewport();
	cocos2d::CCDirector::sharedDirector()->setProjection(cocos2d::kCCDirectorProjection2D);
}

void update_display_scale() {
	auto display_scale = SDL_GetWindowDisplayScale(SDLManager::get().m_window);
	geode::log::debug("update_display_scale: {}", display_scale);

	// with display scale, we have the choice of either scaling inputs, or everything in cocos
	// we chose inputs, but the other solution works just fine (basically no changes to end-user)

	if constexpr (SDLManager::s_cocosHandlesScaling) {
		[[MetalGLView sharedEGLView] setBackingFactor:display_scale];
	} else {
		SDLManager::get().m_displayScale = display_scale;
	}
}

void toggle_vsync(bool disabled) {
	SDLManager::get().m_vsyncEnabled = !disabled;

	if (disabled) {
		auto str = geode::utils::numToString(SDLManager::get().m_targetFramerate);
		SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, str.c_str());

		if (!SDL_GL_SetSwapInterval(0)) {
			geode::log::warn("Failed to disable vertical sync: {}", SDL_GetError());
		}
	} else {
		SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "0");

		if (!SDL_GL_SetSwapInterval(-1)) {
			if (!SDL_GL_SetSwapInterval(1)) {
				geode::log::warn("Failed to enable vertical sync: {}", SDL_GetError());
			}
		}
	}
}

void check_exclusive_fullscreen() {
	auto exclusive_fullscreen = geode::Mod::get()->getSettingValue<bool>("exclusive-fullscreen");
	toggle_exclusive_fullscreen(exclusive_fullscreen, true);
}

void toggle_exclusive_fullscreen(bool enabled, bool force) {
	auto window = SDLManager::get().m_window;
	if (!enabled) {
		SDL_SetWindowFullscreenMode(window, nullptr);
		return;
	}

	auto window_flags = SDL_GetWindowFlags(window);
	if (!force && (window_flags & SDL_WINDOW_FULLSCREEN) != SDL_WINDOW_FULLSCREEN) {
		// wait until we're going into fullscreen to apply this
		return;
	}

	auto display = SDL_GetDisplayForWindow(window);

	int count = 0;
	auto display_modes = SDL_GetFullscreenDisplayModes(display, &count);

	if (count > 0) {
		auto best_mode = display_modes[0];
		SDL_SetWindowFullscreenMode(window, best_mode);
	}
}

void fix_relative_timer() {
	// sdl_time represents the ns since application start
	// gd expects input timestamps to be in ns since system start
	auto sdl_time = SDL_GetTicksNS();
	auto current_time = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);

	auto time_diff = current_time - sdl_time;

	geode::log::debug("Calculated time difference as {}ns", time_diff);

	SDLManager::get().m_timeDiff = time_diff;
}

void fix_menu_items() {
	// redirect quit to sdl to prevent crash
	auto mainMenu = [NSApp mainMenu];
	auto appMenu = [[mainMenu itemAtIndex:0] submenu];

	for (NSMenuItem* item in [appMenu itemArray]) {
		// Quit Geometry Dash
		id target = [item target];
		auto selector = [item action];
		if (target != nil && selector != nil && target == NSApp && selector == @selector(terminate:)) {
			auto updInst = [[NSClassFromString(@"AppControllerManager") sharedInstance] controller];
			[item setTarget:updInst];
			[item setAction:@selector(shutdownGame)];
		}
	}
}

bool setup_steam_manager() {
	// having steam api gives us some nice benefits (achievements, steam input) so enable it if we can
	if (SteamManager::sharedState()->setupSteam()) {
		return true;
	}

	if (geode::Loader::get()->isModLoaded("prevter.vapor")) {
		return true;
	}

	geode::log::error("failed to setup SteamManager!");
	return false;
}

SDL_AppResult SDLCALL my_init_callback(void **appstate, int argc, char *argv[]) {
	if (!setup_steam_manager()) {
		return SDL_APP_FAILURE;
	}

	SDL_SetAppMetadata(
		"Geometry Dash",
		geode::Loader::get()->getGameVersion().c_str(),
		"com.robtop.geometrydashmac"
	);

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
		geode::log::warn("failed to initialize SDL: {}", SDL_GetError());
		return SDL_APP_FAILURE;
	}

#if !ENABLE_VKMOD
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif

	auto gameResolution = GameManager::sharedState()->resolutionForKey(GameManager::sharedState()->m_resolution);

#if ENABLE_VKMOD
	auto lib_path = geode::utils::string::pathToString(geode::Mod::get()->getResourcesDir() / "libMoltenVK.dylib");

	auto lib = dlopen(lib_path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
	if (!lib) {
		geode::log::warn("failed to dlopen moltenvk!!!");
		return SDL_APP_FAILURE;
	}
#endif

	auto window_flags = SDL_WINDOW_RESIZABLE;

	auto disable_high_dpi = geode::Mod::get()->getSettingValue<bool>("disable-high-dpi");
	if (!disable_high_dpi) {
		window_flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
	}

#if ENABLE_VKMOD
	window_flags |= SDL_WINDOW_VULKAN;
#else
	window_flags |= SDL_WINDOW_OPENGL;
#endif

	auto window_title = geode::Mod::get()->getSettingValue<std::string>("window-title");
	auto window = SDL_CreateWindow(window_title.c_str(), gameResolution.width, gameResolution.height, window_flags);
	if (window == nullptr) {
		geode::log::warn("failed to create SDL window: {}", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	SDLManager::get().m_window = window;

	fix_menu_items();
	fix_relative_timer();

	if (geode::Mod::get()->getSettingValue<bool>("p3-color-space")) {
		fix_color_space();
	}

	*appstate = reinterpret_cast<void*>(&SDLManager::get());

	SDL_RaiseWindow(window);

#if !ENABLE_VKMOD
	auto glContext = SDL_GL_CreateContext(window);
	if (glContext == nullptr) {
		geode::log::warn("failed to create GL context: {}", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	auto vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	auto renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	auto version = reinterpret_cast<const char*>(glGetString(GL_VERSION));

	geode::log::debug("gl information: {} ({}) - {}", renderer, vendor, version);
#endif

	auto disable_swap = geode::Mod::get()->getSettingValue<bool>("disable-vsync");

#if !ENABLE_VKMOD
	toggle_vsync(disable_swap);
#endif

	auto uncapFramerate = geode::Mod::get()->getSettingValue<bool>("uncap-framerate");
	auto targetFramerate = uncapFramerate
		? 0
		: geode::Mod::get()->getSettingValue<int>("framerate-limit");

	SDLManager::get().m_targetFramerate = targetFramerate;

	SDLManager::get().m_useIME = geode::Mod::get()->getSettingValue<bool>("use-ime");

	if (disable_swap) {
		auto str = geode::utils::numToString(targetFramerate);
		SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, str.c_str());
	}

	auto exclusive_fullscreen = geode::Mod::get()->getSettingValue<bool>("exclusive-fullscreen");
	toggle_exclusive_fullscreen(exclusive_fullscreen, false);

	update_display_scale();

	int window_w, window_h;
	SDL_GetWindowSizeInPixels(window, &window_w, &window_h);

	cocos2d::CCEGLView::sharedOpenGLView()->setFrameSize(window_w, window_h);

#if ENABLE_VKMOD
	getRenderer()->init(window);
#endif

	AppDelegate::get()->applicationDidFinishLaunching();

	auto fullScreen = GameManager::sharedState()->getGameVariable(GameVar::WindowedMode);
	if (!fullScreen) {
		SDL_SetWindowFullscreen(window, true);
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDLCALL my_iterate_callback(void* appstate) {
	auto appManager = reinterpret_cast<SDLManager*>(appstate);

	reinterpret_cast<cocos2d::CCDisplayLinkDirector*>(cocos2d::CCDirector::sharedDirector())
		->mainLoop();

#if !ENABLE_VKMOD
	SDL_GL_SwapWindow(appManager->m_window);
#endif

	return SDL_APP_CONTINUE;
}

void SDLCALL my_quit_callback(void *appstate, SDL_AppResult result) {
	geode::GameEvent(geode::GameEventType::Exiting).send();
}

int SDLCALL my_runapp_callback(int argc, char *argv[]) {
	return SDL_EnterAppMainCallbacks(
		argc, argv,
		my_init_callback, my_iterate_callback, my_event_callback, my_quit_callback
	);

	std::exit(0);
}

void applicationDidFinishLaunching(void* self, SEL, NSNotification*) {
	// we can use this to swap out the AppController
	AppController2* sharedAppController = [AppController2 sharedAppController];
	(void)[[NSClassFromString(@"AppControllerManager") sharedInstance] setController:sharedAppController];

	SDL_RunApp(0, nullptr, my_runapp_callback, nullptr);

	std::exit(0);
}

MetalGLView* c_sharedEGLView() {
	return [MetalGLView sharedEGLView];
}

MetalCCDirectorCaller* c_sharedDirectorCaller() {
	return [MetalCCDirectorCaller sharedDirectorCaller];
}

$execute {
	{
		// geode replaces the implementation so we have to hook it by address !! yay
		auto h = geode::Mod::get()->hook(
			reinterpret_cast<void*>(geode::base::get() + AppController_applicationDidFinishLaunching_addr),
			&applicationDidFinishLaunching,
			"[AppController applicationDidFinishLaunching:]",
			tulip::hook::TulipConvention::Default
		);
		if (!h) {
			geode::log::warn("failed to hook applicationDidFinishLaunching: {}", h.unwrapErr());
		}
		h.unwrap()->setPriority(geode::Priority::Replace);
	}

	{
		auto objc_class = objc_getClass("CCDirectorCaller");
		auto method = class_getClassMethod(objc_class, @selector(sharedDirectorCaller));
		auto impl = method_getImplementation(method);

		auto h = geode::Mod::get()->hook(
			reinterpret_cast<void*>(impl),
			&c_sharedDirectorCaller,
			"[CCDirectorCaller sharedDirectorCaller]",
			tulip::hook::TulipConvention::Default
		);
		if (!h) {
			geode::log::warn("failed to hook sharedDirectorCaller: {}", h.unwrapErr());
		}
		h.unwrap()->setPriority(geode::Priority::Replace);
	}

	{
		auto objc_class = objc_getClass("EAGLView");
		auto method = class_getClassMethod(objc_class, @selector(sharedEGLView));
		auto impl = method_getImplementation(method);

		auto h = geode::Mod::get()->hook(
			reinterpret_cast<void*>(impl),
			&c_sharedEGLView,
			"[EAGLView sharedEGLView]",
			tulip::hook::TulipConvention::Default
		);
		if (!h) {
			geode::log::warn("failed to hook EAGLView sharedEGLView: {}", h.unwrapErr());
		}
		h.unwrap()->setPriority(geode::Priority::Replace);
	}

	geode::listenForSettingChanges<bool>("disable-vsync", [](bool disable_swap) {
		toggle_vsync(disable_swap);
	});

	geode::listenForSettingChanges<bool>("uncap-framerate", [](bool uncap_framerate) {
		auto target_framerate = uncap_framerate
			? 0
			: geode::Mod::get()->getSettingValue<int>("framerate-limit");
		SDLManager::get().m_targetFramerate = target_framerate;

		if (!SDLManager::get().m_vsyncEnabled) {
			auto str = geode::utils::numToString(target_framerate);
			SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, str.c_str());
		}
	});

	geode::listenForSettingChanges<int>("framerate-limit", [](int frame_limit) {
		SDLManager::get().m_targetFramerate = frame_limit;

		if (!SDLManager::get().m_vsyncEnabled) {
			auto str = geode::utils::numToString(frame_limit);
			SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, str.c_str());
		}
	});

	geode::listenForSettingChanges<bool>("exclusive-fullscreen", [](bool enabled) {
		toggle_exclusive_fullscreen(enabled, false);
	});

	geode::listenForSettingChanges<std::string>("window-title", [](std::string value) {
		SDL_SetWindowTitle(SDLManager::get().m_window, value.c_str());
	});

	geode::listenForSettingChanges<bool>("use-ime", [](bool enabled) {
		SDLManager::get().m_useIME = enabled;
		if (!enabled) {
			auto window = SDLManager::get().m_window;
			SDL_StopTextInput(window);
		}
	});

	geode::GameEvent(geode::GameEventType::Exiting).listen([] {
		geode::log::info("Closing Steam API");
		SteamAPI_Shutdown();
	}).leak();
}
