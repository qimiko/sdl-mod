#include "base.h"

#include <Geode/Geode.hpp>

#include <Geode/modify/PlatformToolbox.hpp>
#include <Geode/modify/CCTextFieldTTF.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/PlayLayer.hpp>

struct $modify(PlatformToolbox) {
	static void showCursor() {
		geode::log::info("showCursor");
		SDL_ShowCursor();
		SDL_SetWindowRelativeMouseMode(SDLManager::get().m_window, false);
	}

	static void hideCursor() {
		geode::log::info("hideCursor");
		SDL_HideCursor();
	}
};

SDL_Rect s_imeCandidate{};
bool s_hasIMECandidate = false;

/*
struct $modify(cocos2d::CCTextFieldTTF) {
	bool attachWithIME() {
		if (this->getParent()) {
			auto worldPos = this->getParent()->convertToWorldSpace(this->getPosition());

			s_hasIMECandidate = true;
			s_imeCandidate = SDL_Rect {
				static_cast<int>(worldPos.x),
				static_cast<int>(worldPos.y),
				static_cast<int>(this->getScaledContentWidth()),
				static_cast<int>(this->getScaledContentHeight())
			};
		}

		return CCTextFieldTTF::attachWithIME();
	}

	bool detachWithIME() {
		s_hasIMECandidate = false;

		return CCTextFieldTTF::detachWithIME();
	}
};
*/

struct $modify(cocos2d::CCEGLView) {
	void setIMEKeyboardState(bool activated) override {
		CCEGLView::setIMEKeyboardState(activated);

		if (SDLManager::get().m_useIME) {
			auto window = SDLManager::get().m_window;
			if (activated) {
				if (s_hasIMECandidate) {
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
