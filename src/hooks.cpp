#include "base.h"

#include <Geode/Geode.hpp>

#include <Geode/modify/PlatformToolbox.hpp>
#include <Geode/modify/CCTextFieldTTF.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/PlayLayer.hpp>

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
