#include <Geode/Geode.hpp>

#include "base.h"
#include "objc.h"

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

-(void) updateMousePosition {}

-(void) toggleMouseControl:(bool)on {
	geode::log::debug("toggleMouseControl: {}", on);
}

-(void) queueWindowResize:(CGRect)sz Center:(bool)on {
	auto window = SDLManager::get().m_window;
	SDL_SetWindowSize(window, sz.size.width, sz.size.height);
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

	// static_assert(offsetof(cocos2d::CCDirector, m_pobOpenGLView) == 0xb8);

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