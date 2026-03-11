#pragma once

#include <SDL3/SDL_events.h>

bool sdl_on_event(void* appstate, SDL_Event* event);

void update_mouse_position();
void perform_sdl_runapp();
void update_display_scale();
void resize_window(int w, int h);
void toggle_exclusive_fullscreen(bool enabled, bool force);
void check_exclusive_fullscreen();
