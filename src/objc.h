#pragma once

#define CommentType CommentTypeDummy
#import <Cocoa/Cocoa.h>
#include <objc/runtime.h>
#include <objc/message.h>
#undef CommentType

@interface AppController2 : NSObject <NSApplicationDelegate> {
	void* _window; // CCWindow*
	void* _glView; // EAGLView*
	bool _shouldClose;
	bool _saveCalled;
	bool _forceClose;
	bool _liveResizing;
	bool _forceCenter;
	bool _mouseDown;
	CGSize designSize_;
	CGRect windowedSize_;
	CGRect _savedSize;
	NSArray* joysticks;
	CGRect originalWinRect_;
	CGRect originalWinFrame_;
	double _menuBarHeight;
	cocos2d::CCPoint _mouseVel;
	float _scrollVel;
	bool _mouseEnabled;
	bool _isControllerConnected;
	CGRect _queuedWindowFrame;
	bool _queuedWindowCenter;
}

+(id) sharedAppController;
-(bool) isControllerConnected;
-(void) gameDidSave;
-(void) toggleFullScreenOn;
-(void) toggleFullScreenOff;
-(void) shutdownGame;
-(void) updateMousePosition;
-(void) toggleMouseControl:(bool)on;
-(void) dispatchQueuedEvents;

-(void) queueWindowResize:(CGRect)sz Center:(bool)on;
-(cocos2d::CCSize) getDisplaySize;
@end

@interface AppControllerManager2 {
	AppController2* _controller;
}

-(void)setController:(AppController2*)controller;
-(AppController2*)controller;

+(AppControllerManager2*)sharedInstance;

@end

@interface MetalCCDirectorCaller : NSObject {
}
// i don't think these are ever called
// @property (readwrite) int interval;
// -(void) doCaller: (id)sender;
-(void) setAnimationInterval:(double)interval;

-(void) startMainLoop;
-(void) end;

+(id) sharedDirectorCaller;
@end

@protocol MacEventDelegate <NSObject>
- (void)mouseDown:(NSEvent *)theEvent;
- (void)mouseUp:(NSEvent *)theEvent;
- (void)mouseMoved:(NSEvent *)theEvent;
- (void)mouseDragged:(NSEvent *)theEvent;
- (void)rightMouseDown:(NSEvent*)event;
- (void)rightMouseDragged:(NSEvent*)event;
- (void)rightMouseUp:(NSEvent*)event;
- (void)otherMouseDown:(NSEvent*)event;
- (void)otherMouseDragged:(NSEvent*)event;
- (void)otherMouseUp:(NSEvent*)event;
- (void)scrollWheel:(NSEvent *)theEvent;
- (void)mouseEntered:(NSEvent *)theEvent;
- (void)mouseExited:(NSEvent *)theEvent;

- (void)keyDown:(NSEvent *)theEvent;
- (void)keyUp:(NSEvent *)theEvent;
- (void)flagsChanged:(NSEvent *)theEvent;

- (void)touchesBeganWithEvent:(NSEvent *)event;
- (void)touchesMovedWithEvent:(NSEvent *)event;
- (void)touchesEndedWithEvent:(NSEvent *)event;
- (void)touchesCancelledWithEvent:(NSEvent *)event;

// this is the only real new thing it adds
- (void)queueEvent:(NSEvent*)event selector:(SEL)selector;

@end

@interface MetalGLView : NSView {
	id<MacEventDelegate> eventDelegate_;

	float backingScaleFactor_;
}

@property (nonatomic, readwrite, assign) id<MacEventDelegate> eventDelegate;

// whether or not the view is in fullscreen mode
@property (nonatomic, readonly) BOOL isFullScreen;

@property (nonatomic, readwrite) float frameZoomFactor;

// get the view object
+(id) sharedEGLView;

// -(int) getWidth;
// -(int) getHeight;
-(void) swapBuffers;

// -(void) setFullScreen:(BOOL)fullscreen borderless:(BOOL)isBorderless;

-(void) removeFromSuperview;
-(void) becomeFirstResponder;
-(void) resignFirstResponder;

-(float) getBackingFactor;
-(void) setBackingFactor:(float)f;

@end