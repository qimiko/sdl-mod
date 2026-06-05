#include "base.h"

#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_keyboard.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlatformToolbox.hpp>
#include <Geode/modify/CCTextFieldTTF.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

#include "keymap.h"

struct $modify(PlatformToolbox) {
	static void showCursor() {
		SDLManager::get().m_cursorHidden = false;

		SDL_ShowCursor();
		SDL_SetWindowRelativeMouseMode(SDLManager::get().m_window, false);
	}

	static void hideCursor() {
		SDLManager::get().m_cursorHidden = true;

		SDL_HideCursor();
	}
};

SDL_Rect s_imeCandidate{};
bool s_hasIMECandidate = false;

struct $modify(cocos2d::CCTextFieldTTF) {
	bool attachWithIME() {
		if (this->getParent()) {
			auto displayScale = 1.0f;
			if constexpr (!SDLManager::s_cocosHandlesScaling) {
				displayScale = SDLManager::get().m_displayScale;
			}

			// sdl origin is upper left, while cocos origin is bottom left

			auto frameSize = cocos2d::CCEGLView::sharedOpenGLView()->getFrameSize();
			auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

			auto scaleFactor = (frameSize.width / winSize.width) / displayScale;

			auto worldPos = this->convertToWorldSpace({0.0f, 0.0f});
			auto uiPos = cocos2d::CCPoint {worldPos.x, winSize.height - worldPos.y} * scaleFactor;

			auto windowWidth = this->getScaledContentWidth() * scaleFactor;
			auto windowHeight = this->getScaledContentHeight() * scaleFactor;

			// attempt to center the ime on the text field
			// not perfect unfortunately...
			s_hasIMECandidate = true;
			s_imeCandidate = {
				static_cast<int>(uiPos.x + windowWidth / 2),
				static_cast<int>(uiPos.y - windowHeight),
				static_cast<int>(windowWidth/2),
				static_cast<int>(windowHeight),
			};
		}

		return CCTextFieldTTF::attachWithIME();
	}

	bool detachWithIME() {
		s_hasIMECandidate = false;

		return CCTextFieldTTF::detachWithIME();
	}
};

struct $modify(cocos2d::CCEGLView) {
	void setIMEKeyboardState(bool activated) override {
		CCEGLView::setIMEKeyboardState(activated);

		if (SDLManager::get().m_useIME) {
			auto window = SDLManager::get().m_window;
			if (activated) {
				if (s_hasIMECandidate) {
					// we would've used the cursor offset here, but it didn't seem to work
					SDL_SetTextInputArea(window, &s_imeCandidate, 0);
				}
				SDL_StartTextInput(window);
			} else {
				SDL_StopTextInput(window);
			}
		}
	}
};

void enableLockCursor() {
	SDLManager::get().m_cursorHidden = true;

	SDL_SetWindowRelativeMouseMode(SDLManager::get().m_window, true);
}

struct $modify(PlayLayer) {
	bool init(GJGameLevel* level, bool p1, bool p2) {
		if (!PlayLayer::init(level, p1, p2)) {
			return false;
		}

		if (GameManager::sharedState()->getGameVariable(GameVar::LockCursor)) {
			enableLockCursor();
		}

		return true;
	}

	void resume() {
		if (AppDelegate::get()->m_gamePaused) {
			if (GameManager::sharedState()->getGameVariable(GameVar::LockCursor)) {
				enableLockCursor();
			}
		}

		PlayLayer::resume();
	}

	void resumeAndRestart(bool p1) {
		if (AppDelegate::get()->m_gamePaused) {
			if (GameManager::sharedState()->getGameVariable(GameVar::LockCursor)) {
				enableLockCursor();
			}
		}

		PlayLayer::resumeAndRestart(p1);
	}
};

struct $modify(cocos2d::CCKeyboardDispatcher) {
	const char* keyToString(cocos2d::enumKeyCodes key) {
		switch (static_cast<ExtraEnumKeyCodes>(key)) {
			case ExtraEnumKeyCodes::Controller_L3: return "Controller_L3";
			case ExtraEnumKeyCodes::Controller2_L3: return "Controller2_L3";
			case ExtraEnumKeyCodes::Controller_R3: return "Controller_R3";
			case ExtraEnumKeyCodes::Controller2_R3: return "Controller2_R3";

			case ExtraEnumKeyCodes::Controller_Misc1: return "Controller_Misc1";
			case ExtraEnumKeyCodes::Controller2_Misc1: return "Controller2_Misc1";
			case ExtraEnumKeyCodes::Controller_Misc2: return "Controller_Misc2";
			case ExtraEnumKeyCodes::Controller2_Misc2: return "Controller2_Misc2";
			case ExtraEnumKeyCodes::Controller_Misc3: return "Controller_Misc3";
			case ExtraEnumKeyCodes::Controller2_Misc3: return "Controller2_Misc3";
			case ExtraEnumKeyCodes::Controller_Misc4: return "Controller_Misc4";
			case ExtraEnumKeyCodes::Controller2_Misc4: return "Controller2_Misc4";
			case ExtraEnumKeyCodes::Controller_Misc5: return "Controller_Misc5";
			case ExtraEnumKeyCodes::Controller2_Misc5: return "Controller2_Misc5";
			case ExtraEnumKeyCodes::Controller_Misc6: return "Controller_Misc6";
			case ExtraEnumKeyCodes::Controller2_Misc6: return "Controller2_Misc6";
			case ExtraEnumKeyCodes::Controller_RightPaddle1: return "Controller_RightPaddle1";
			case ExtraEnumKeyCodes::Controller2_RightPaddle1: return "Controller2_RightPaddle1";
			case ExtraEnumKeyCodes::Controller_LeftPaddle1: return "Controller_LeftPaddle1";
			case ExtraEnumKeyCodes::Controller2_LeftPaddle1: return "Controller2_LeftPaddle1";
			case ExtraEnumKeyCodes::Controller_RightPaddle2: return "Controller_RightPaddle2";
			case ExtraEnumKeyCodes::Controller2_RightPaddle2: return "Controller2_RightPaddle2";
			case ExtraEnumKeyCodes::Controller_LeftPaddle2: return "Controller_LeftPaddle2";
			case ExtraEnumKeyCodes::Controller2_LeftPaddle2: return "Controller2_LeftPaddle2";
			case ExtraEnumKeyCodes::Controller_Touchpad: return "Controller_Touchpad";
			case ExtraEnumKeyCodes::Controller2_Touchpad: return "Controller2_Touchpad";
			case ExtraEnumKeyCodes::Controller_Guide: return "Controller_Guide";
			case ExtraEnumKeyCodes::Controller2_Guide: return "Controller2_Guide";
			default: return CCKeyboardDispatcher::keyToString(key);
		}
	}

	static void onModify(auto& self) {
		if (!self.setHookPriorityBeforePre("cocos2d::CCKeyboardDispatcher::keyToString", "geode.loader")) {
			geode::log::warn("Failed to set hook priority.");
		}
	}
};
