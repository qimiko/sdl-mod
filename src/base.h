#pragma once

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cstdint>

SDL_AppResult SDLCALL my_event_callback(void *appstate, SDL_Event *event);

class SDLManager {
private:
	SDLManager() {}

public:
	SDL_Window* m_window = nullptr;
	bool m_stopped = false;
	bool m_closeAfterSave = false;
	std::uint64_t m_timeDiff = 0;
	float m_displayScale = 1.0f;

	static SDLManager& get();
};

void trigger_shutdown();
void update_display_scale();
void resize_window(int w, int h);
