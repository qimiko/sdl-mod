#define SDL_MAIN_HANDLED
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_timer.h>

#include "bind.hpp"
#include "base.h"
#include "platform.h"
#include "events.h"

#define ENABLE_VKMOD 0

#if ENABLE_VKMOD
#include "vkmod/Vulkan.hpp"
#endif

// for geode 5.3.0 and below, gameexit event is called during platformshutdown
// we can't call it twice or it'll crash, but we're also more reliable about calling it on those versions
// this is slightly hacky. it'll do for now
// TODO: remove when geode 5.3.1+ releases and min version is increased
bool exit_already_called = false;

SDLManager& SDLManager::get() {
	static SDLManager manager;
	return manager;
}

void resize_window(int w, int h) {
	cocos2d::CCEGLView::sharedOpenGLView()->setFrameSize(w, h);
	cocos2d::CCDirector::sharedDirector()->updateScreenScale({static_cast<float>(w), static_cast<float>(h)});
	cocos2d::CCDirector::sharedDirector()->setViewport();
	cocos2d::CCDirector::sharedDirector()->setProjection(cocos2d::kCCDirectorProjection2D);
}

void update_display_scale() {
	auto display_scale = SDL_GetWindowDisplayScale(SDLManager::get().m_window);
	geode::log::debug("Updating display scale to {}x", display_scale);

	// with display scale, we have the choice of either scaling inputs, or everything in cocos
	// we chose inputs, but the other solution works just fine (basically no changes to end-user)

	if constexpr (SDLManager::s_cocosHandlesScaling) {
		update_platform_scale(display_scale);
	} else {
		SDLManager::get().m_displayScale = display_scale;
	}
}

void fix_relative_timer() {
	// sdl_time represents the ns since application start
	// gd expects input timestamps to be in ns since system start
	auto sdl_time = SDL_GetTicksNS();
	auto current_time = get_platform_time_ns();

	auto time_diff = current_time - sdl_time;

	geode::log::debug("Calculated time difference as {}ns", time_diff);

	SDLManager::get().m_timeDiff = time_diff;
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
		"com.robtop.geometrydash-mac"
	);

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
		geode::log::warn("failed to initialize SDL: {}", SDL_GetError());
		return SDL_APP_FAILURE;
	}

#if !ENABLE_VKMOD
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	/*
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	*/

	// vanilla parity
	// we have to request stencil bits, or we won't get them (which breaks clipping nodes)
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
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

	platform_fixes();
	fix_relative_timer();

	auto fullScreen = !GameManager::sharedState()->getGameVariable(GameVar::WindowedMode);

	// only force exclusive fullscreen if we're right about to go into fullscreen
	auto exclusive_fullscreen = geode::Mod::get()->getSettingValue<bool>("exclusive-fullscreen");
	toggle_exclusive_fullscreen(exclusive_fullscreen, fullScreen);

	*appstate = reinterpret_cast<void*>(&SDLManager::get());

	SDL_SetWindowMinimumSize(window, 100, 100);
	SDL_RaiseWindow(window);

#if !ENABLE_VKMOD
	if (!SDL_GL_CreateContext(window)) {
		geode::log::warn("failed to create GL context: {}", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	geode::log::debug("GL context information:");
	{
		geode::log::pushNest();

		auto vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		auto renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		auto version = reinterpret_cast<const char*>(glGetString(GL_VERSION));

		geode::log::debug("{} ({}) - {}", renderer, vendor, version);

		GLint red, green, blue, alpha, depth, stencil;
		glGetIntegerv(GL_RED_BITS, &red);
		glGetIntegerv(GL_GREEN_BITS, &green);
		glGetIntegerv(GL_BLUE_BITS, &blue);
		glGetIntegerv(GL_ALPHA_BITS, &alpha);
		glGetIntegerv(GL_DEPTH_BITS, &depth);
		glGetIntegerv(GL_STENCIL_BITS, &stencil);

		geode::log::debug("red={}, green={}, blue={}, alpha={}, depth={}, stencil={}", red, green, blue, alpha, depth, stencil);

		geode::log::popNest();
	}
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

	auto unlockAspectRatio = geode::Mod::get()->getSettingValue<bool>("unlock-aspect-ratio");
	SDLManager::get().m_unlockAspectRatio = unlockAspectRatio;

	if (!unlockAspectRatio) {
		auto targetAspect = gameResolution.width / gameResolution.height;
		SDL_SetWindowAspectRatio(window, targetAspect, targetAspect);
	}

	if (disable_swap) {
		auto str = geode::utils::numToString(targetFramerate);
		SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, str.c_str());
	}

	if (fullScreen) {
		SDL_SetWindowFullscreen(window, true);
	}

	update_display_scale();

	int window_w, window_h;
	SDL_GetWindowSizeInPixels(window, &window_w, &window_h);

	cocos2d::CCEGLView::sharedOpenGLView()->setFrameSize(window_w, window_h);

#if ENABLE_VKMOD
	getRenderer()->init(window);
#endif

	AppDelegate::get()->applicationDidFinishLaunching();

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
	if (!exit_already_called) {
		geode::GameEvent(geode::GameEventType::Exiting).send();
	}
}

SDL_AppResult SDLCALL my_event_callback(void *appstate, SDL_Event *event) {
	if (!sdl_on_event(appstate, event)) {
		return SDL_APP_SUCCESS;
	}

	return SDL_APP_CONTINUE;
}

int SDLCALL my_runapp_callback(int argc, char *argv[]) {
	return SDL_EnterAppMainCallbacks(
		argc, argv,
		my_init_callback, my_iterate_callback, my_event_callback, my_quit_callback
	);

	std::exit(0);
}

void perform_sdl_runapp() {
	SDL_RunApp(0, nullptr, my_runapp_callback, nullptr);
}

$execute {
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
		if (enabled) {
			auto shownWarning = geode::Mod::get()->getSavedValue<bool>("shown-exclusive-warning");
			if (!shownWarning) {
				geode::createQuickPopup(
					"Exclusive Fullscreen",
					"Be <cr>careful</c>! This option is very <cy>unstable</c> and may disable the macOS system UI, locking you into the game. If you <cj>get stuck</c>, CMD + Q or CMD + Tab usually works...",
					"OK", nullptr,
					[](auto, bool btn2) {
						geode::Mod::get()->setSavedValue<bool>("shown-exclusive-warning", true);
						toggle_exclusive_fullscreen(true, false);
					}
				);
				return;
			}
		}

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

	/*
	// disabled as it didn't seem to work properly (crashes)
	geode::listenForSettingChanges<bool>("unlock-aspect-ratio", [](bool enabled) {
		auto window = SDLManager::get().m_window;
		SDLManager::get().m_unlockAspectRatio = enabled;

		if (enabled) {
			auto gameResolution = GameManager::sharedState()->resolutionForKey(GameManager::sharedState()->m_resolution);
			auto targetAspect = gameResolution.width / gameResolution.height;

			SDL_SetWindowAspectRatio(window, targetAspect, targetAspect);
		} else {
			SDL_SetWindowAspectRatio(window, 0.0f, 0.0f);
		}
	});
	*/

	geode::GameEvent(geode::GameEventType::Exiting).listen([] {
		exit_already_called = true;

		platform_shutdown_steam();
	}).leak();
}
