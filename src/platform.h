#pragma once

#include <cstdint>

void platform_shutdown_steam();
void platform_trigger_shutdown();
void platform_fixes();
void update_platform_scale(float scale);
std::uint64_t get_platform_time_ns();
