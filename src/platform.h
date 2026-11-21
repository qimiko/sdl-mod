#pragma once

#include <cstdint>

void platform_shutdown_steam();
void platform_trigger_shutdown();
void platform_fixes();
void update_platform_scale(float scale);
void update_platform_cursor_pos(float x, float y);
std::uint64_t get_platform_time_ns();
