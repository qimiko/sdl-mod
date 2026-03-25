#pragma once

#include <cstdint>

#include <SDL3/SDL_video.h>

class SDLManager {
private:
	SDLManager() {}

public:
	static constexpr bool s_cocosHandlesScaling = false;

	SDL_Window* m_window = nullptr;
	bool m_stopped = false;
	bool m_closeAfterSave = false;
	std::uint64_t m_timeDiff = 0;
	float m_displayScale = 1.0f;
	bool m_useIME = false;
	bool m_unlockAspectRatio = false;

	int m_targetFramerate = 0;
	bool m_vsyncEnabled = false;

	bool m_cursorHidden = false;
	float m_cursorVertical = 0.0f;
	float m_cursorHorizontal = 0.0f;
	float m_scrollVertical = 0.0f;
	float m_scrollHorizontal = 0.0f;
	bool m_mouseDown = false;
	bool m_backDown = false;

	bool m_pauseOnFocusLoss = false;

	static SDLManager& get();

	void reset_controller_keys();
};
