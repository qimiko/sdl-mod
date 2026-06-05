#pragma once

#include <Geode/utils/Keyboard.hpp>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_gamepad.h>

geode::KeyboardModifier modifiers_from_keymod(SDL_Keymod flags);
geode::KeyboardModifier get_current_modifiers();
cocos2d::enumKeyCodes translate_keycode(SDL_Keycode code);
cocos2d::enumKeyCodes translate_scancode(SDL_Scancode code);
cocos2d::enumKeyCodes translate_gamepad_button(SDL_GamepadButton button);

enum class ExtraEnumKeyCodes {
	// thumbstick buttons are part of geode v5.7.2+
	Controller_L3 = 0x1201,
	Controller2_L3 = 0x1202,
	Controller_R3 = 0x1203,
	Controller2_R3 = 0x1204,

	Controller_Misc1 = 0x1551,
	Controller2_Misc1 = 0x1552,
	Controller_Misc2 = 0x1553,
	Controller2_Misc2 = 0x1554,
	Controller_Misc3 = 0x1555,
	Controller2_Misc3 = 0x1556,
	Controller_Misc4 = 0x1557,
	Controller2_Misc4 = 0x1558,
	Controller_Misc5 = 0x1559,
	Controller2_Misc5 = 0x155A,
	Controller_Misc6 = 0x155B,
	Controller2_Misc6 = 0x155C,

	Controller_RightPaddle1 = 0x155D,
	Controller2_RightPaddle1 = 0x155E,

	Controller_LeftPaddle1 = 0x155F,
	Controller2_LeftPaddle1 = 0x1560,

	Controller_RightPaddle2 = 0x1561,
	Controller2_RightPaddle2 = 0x1562,

	Controller_LeftPaddle2 = 0x1563,
	Controller2_LeftPaddle2 = 0x1564,

	Controller_Touchpad = 0x1565,
	Controller2_Touchpad = 0x1566,

	Controller_Guide = 0x1567,
	Controller2_Guide = 0x1568
};
