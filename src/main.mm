#include <Geode/Geode.hpp>

#include "bind.hpp"
#include "base.h"
#include "objc.h"
#include "platform.h"
#include "events.h"

#include <dlfcn.h>

void platform_shutdown_steam() {
	if (auto shutdownAddr = dlsym(RTLD_DEFAULT, "SteamAPI_Shutdown")) {
		geode::log::info("Closing Steam API");
		reinterpret_cast<void(*)()>(shutdownAddr)();
	} else {
		geode::log::warn("Failed to fetch address of SteamAPI_Shutdown");
	}
}

void platform_trigger_shutdown() {
	[[AppController2 sharedAppController] shutdownGame];
}

void update_platform_scale(float scale) {
	[[MetalGLView sharedEGLView] setBackingFactor:scale];
}

std::uint64_t get_platform_time_ns() {
	return clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
}

void fix_menu_items() {
	// redirect quit to sdl to prevent crash
	auto mainMenu = [NSApp mainMenu];
	auto appMenu = [[mainMenu itemAtIndex:0] submenu];

	bool fixedItem = false;

	for (NSMenuItem* item in [appMenu itemArray]) {
		// Quit Geometry Dash
		id target = [item target];
		auto selector = [item action];
		if (target != nil && selector != nil && target == NSApp && selector == @selector(terminate:)) {
			auto updInst = [[NSClassFromString(@"AppControllerManager") sharedInstance] controller];
			[item setTarget:updInst];
			[item setAction:@selector(shutdownGame)];

			if (geode::Mod::get()->getSettingValue<bool>("switch-quit")) {
				unichar c = NSF4FunctionKey;
				auto f4 = [NSString stringWithCharacters:&c length:1];

				[item setKeyEquivalent:f4];
				[item setKeyEquivalentModifierMask:NSEventModifierFlagOption];
			}

			fixedItem = true;
			break;
		}
	}

	if (!fixedItem) {
		geode::log::warn("Failed to fix terminate menu item - ⌘ + Q might not work!");
	}
}

void platform_fixes() {
	fix_menu_items();
}

void applicationDidFinishLaunching(void* self, SEL, NSNotification*) {
	// we can use this to swap out the AppController
	AppController2* sharedAppController = [AppController2 sharedAppController];
	[[NSClassFromString(@"AppControllerManager") sharedInstance] setController:sharedAppController];

	perform_sdl_runapp();

	std::exit(0);
}

NSApplicationTerminateReply applicationShouldTerminate(void* self, SEL, NSApplication*) {
	SDL_Event ev{};
	ev.type = SDL_EVENT_QUIT;
	SDL_PushEvent(&ev);

	return NSTerminateCancel;
}

MetalGLView* c_sharedEGLView() {
	return [MetalGLView sharedEGLView];
}

MetalCCDirectorCaller* c_sharedDirectorCaller() {
	return [MetalCCDirectorCaller sharedDirectorCaller];
}

static id s_sharedAppController;

@implementation AppController2

+(id) sharedAppController {
	if (s_sharedAppController == nil) {
		s_sharedAppController = [[AppController2 alloc] init];
	}

	return s_sharedAppController;
}

-(bool) isControllerConnected {
	return SDL_HasGamepad();
}

-(void) gameDidSave {
	if (SDLManager::get().m_closeAfterSave) {
		SDLManager::get().m_stopped = true;

		// send a quit event to prompt a return from sdl
		SDL_Event ev{};
		ev.type = SDL_EVENT_QUIT;
		SDL_PushEvent(&ev);
	}
}

-(void) toggleFullScreenOn {
	auto window = SDLManager::get().m_window;

	check_exclusive_fullscreen();
	SDL_SetWindowFullscreen(window, true);
}

-(void) toggleFullScreenOff {
	auto window = SDLManager::get().m_window;

	toggle_exclusive_fullscreen(false, true);
	SDL_SetWindowFullscreen(window, false);
}

-(void) shutdownGame {
	SDLManager::get().m_closeAfterSave = true;
	AppDelegate::get()->trySaveGame(true);
}

-(void) updateMousePosition {
	update_mouse_position();
}

-(void) toggleMouseControl:(bool)on {
	if (!on) {
		SDLManager::get().reset_controller_keys();
		update_mouse_position();
	}
}

-(void) queueWindowResize:(CGRect)sz Center:(bool)on {
	auto window = SDLManager::get().m_window;

	if (!SDLManager::get().m_unlockAspectRatio) {
		auto newTarget = sz.size.width / sz.size.height;
		SDL_SetWindowAspectRatio(window, newTarget, newTarget);

		// sync or the aspect ratio change messes up the final window size
		SDL_SyncWindow(window);
	}

	SDL_SetWindowSize(window, sz.size.width, sz.size.height);

	if (on) {
		SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}
}

-(cocos2d::CCSize) getDisplaySize {
	auto window = SDLManager::get().m_window;
	int width, height;
	SDL_GetWindowSizeInPixels(window, &width, &height);
	return {static_cast<float>(width), static_cast<float>(height)};
}

-(void) dispatchQueuedEvents {}

@end

static MetalGLView* s_metalGLView;

@implementation MetalGLView

+(id) sharedEGLView {
	if (s_metalGLView == nil) {
		s_metalGLView = [[MetalGLView alloc] init];
		s_metalGLView.frameZoomFactor = 1.0f;
		s_metalGLView->backingScaleFactor_ = 1.0f;
	}

	return s_metalGLView;
}

-(void) removeFromSuperview {}
-(void) swapBuffers {}
-(void) becomeFirstResponder {}
-(void) resignFirstResponder {}

-(float) getBackingFactor {
	return self->backingScaleFactor_;
}

-(void) setBackingFactor:(float)f {
	self->backingScaleFactor_ = f;
}

@end

static id s_sharedMetalDirectorCaller;

@implementation MetalCCDirectorCaller

+(id) sharedDirectorCaller {
	if (s_sharedMetalDirectorCaller == nil) {
		s_sharedMetalDirectorCaller = [[MetalCCDirectorCaller alloc] init];
	}

	return s_sharedMetalDirectorCaller;
}

-(void) setAnimationInterval:(double)interval {}
-(void) startMainLoop {}

-(void) dealloc {
	s_sharedMetalDirectorCaller = nil;

	[super dealloc];
}

-(void) end {}

@end

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

	{
		auto objc_class = objc_getClass("AppController");
		auto method = class_getInstanceMethod(objc_class, @selector(applicationShouldTerminate:));
		auto impl = method_getImplementation(method);

		auto h = geode::Mod::get()->hook(
			reinterpret_cast<void*>(impl),
			&applicationShouldTerminate,
			"[AppController applicationShouldTerminate:]",
			tulip::hook::TulipConvention::Default
		);
		if (!h) {
			geode::log::warn("failed to hook AppController applicationShouldTerminate: {}", h.unwrapErr());
		}
		h.unwrap()->setPriority(geode::Priority::Replace);
	}
}
