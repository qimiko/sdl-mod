#pragma once

#include <Geode/utils/Keyboard.hpp>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_gamepad.h>

geode::KeyboardModifier modifiers_from_keymod(SDL_Keymod flags);
geode::KeyboardModifier get_current_modifiers();
cocos2d::enumKeyCodes translate_keycode(SDL_Keycode code);
cocos2d::enumKeyCodes translate_gamepad_button(SDL_GamepadButton button);
