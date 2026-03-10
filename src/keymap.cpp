#include "keymap.h"

#include <SDL3/SDL_keyboard.h>

geode::KeyboardModifier modifiers_from_keymod(SDL_Keymod flags) {
	geode::KeyboardModifier modifiers = geode::KeyboardModifier::None;

	if ((flags & SDL_KMOD_SHIFT) != 0) {
		modifiers |= geode::KeyboardModifier::Shift;
	}
	if ((flags & SDL_KMOD_CTRL) != 0) {
		modifiers |= geode::KeyboardModifier::Control;
	}
	if ((flags & SDL_KMOD_ALT) != 0) {
		modifiers |= geode::KeyboardModifier::Alt;
	}
	if ((flags & SDL_KMOD_GUI) != 0) {
		modifiers |= geode::KeyboardModifier::Super;
	}

	return modifiers;
}

geode::KeyboardModifier get_current_modifiers() {
	return modifiers_from_keymod(SDL_GetModState());
}

cocos2d::enumKeyCodes translate_keycode(SDL_Keycode code) {
	using namespace cocos2d;

	switch (code) {
		case SDLK_RETURN: return KEY_Enter;
		case SDLK_ESCAPE: return KEY_Escape;
		case SDLK_BACKSPACE: return KEY_Backspace;
		case SDLK_TAB: return KEY_Tab;
		case SDLK_SPACE: return KEY_Space;
		// case SDLK_EXCLAIM:
		// case SDLK_DBLAPOSTROPHE:
		// case SDLK_HASH:
		// case SDLK_DOLLAR:
		// case SDLK_PERCENT:
		// case SDLK_AMPERSAND:
		case SDLK_APOSTROPHE: return KEY_Apostrophe;
		// case SDLK_LEFTPAREN:
		// case SDLK_RIGHTPAREN:
		// case SDLK_ASTERISK:
		case SDLK_PLUS: return KEY_OEMPlus;
		case SDLK_COMMA: return KEY_OEMComma;
		case SDLK_MINUS: return KEY_OEMMinus;
		case SDLK_PERIOD: return KEY_OEMPeriod;
		case SDLK_SLASH: return KEY_Slash;
		case SDLK_0: return KEY_Zero;
		case SDLK_1: return KEY_One;
		case SDLK_2: return KEY_Two;
		case SDLK_3: return KEY_Three;
		case SDLK_4: return KEY_Four;
		case SDLK_5: return KEY_Five;
		case SDLK_6: return KEY_Six;
		case SDLK_7: return KEY_Seven;
		case SDLK_8: return KEY_Eight;
		case SDLK_9: return KEY_Nine;
		// case SDLK_COLON:
		case SDLK_SEMICOLON: return KEY_Semicolon;
		// case SDLK_LESS:
		case SDLK_EQUALS: return KEY_Equal;
		// case SDLK_GREATER:
		// case SDLK_QUESTION:
		// case SDLK_AT:
		case SDLK_LEFTBRACKET: return KEY_LeftBracket;
		case SDLK_BACKSLASH: return KEY_Backslash;
		case SDLK_RIGHTBRACKET: return KEY_RightBracket;
		// case SDLK_CARET:
		// case SDLK_UNDERSCORE:
		case SDLK_GRAVE: return KEY_GraveAccent;
		case SDLK_A: return KEY_A;
		case SDLK_B: return KEY_B;
		case SDLK_C: return KEY_C;
		case SDLK_D: return KEY_D;
		case SDLK_E: return KEY_E;
		case SDLK_F: return KEY_F;
		case SDLK_G: return KEY_G;
		case SDLK_H: return KEY_H;
		case SDLK_I: return KEY_I;
		case SDLK_J: return KEY_J;
		case SDLK_K: return KEY_K;
		case SDLK_L: return KEY_L;
		case SDLK_M: return KEY_M;
		case SDLK_N: return KEY_N;
		case SDLK_O: return KEY_O;
		case SDLK_P: return KEY_P;
		case SDLK_Q: return KEY_Q;
		case SDLK_R: return KEY_R;
		case SDLK_S: return KEY_S;
		case SDLK_T: return KEY_T;
		case SDLK_U: return KEY_U;
		case SDLK_V: return KEY_V;
		case SDLK_W: return KEY_W;
		case SDLK_X: return KEY_X;
		case SDLK_Y: return KEY_Y;
		case SDLK_Z: return KEY_Z;
		// case SDLK_LEFTBRACE:
		// case SDLK_PIPE:
		// case SDLK_RIGHTBRACE:
		// case SDLK_TILDE:
		case SDLK_DELETE: return KEY_Delete;
		// case SDLK_PLUSMINUS:
		// case SDLK_CAPSLOCK:
		case SDLK_F1: return KEY_F1;
		case SDLK_F2: return KEY_F2;
		case SDLK_F3: return KEY_F3;
		case SDLK_F4: return KEY_F4;
		case SDLK_F5: return KEY_F5;
		case SDLK_F6: return KEY_F6;
		case SDLK_F7: return KEY_F7;
		case SDLK_F8: return KEY_F8;
		case SDLK_F9: return KEY_F9;
		case SDLK_F10: return KEY_F10;
		case SDLK_F11: return KEY_F11;
		case SDLK_F12: return KEY_F12;
		case SDLK_PRINTSCREEN: return KEY_PrintScreen;
		case SDLK_SCROLLLOCK: return KEY_ScrollLock;
		case SDLK_PAUSE: return KEY_Pause;
		case SDLK_INSERT: return KEY_Insert;
		case SDLK_HOME: return KEY_Home;
		case SDLK_PAGEUP: return KEY_PageUp;
		case SDLK_END: return KEY_End;
		case SDLK_PAGEDOWN: return KEY_PageDown;
		case SDLK_RIGHT: return KEY_Right;
		case SDLK_LEFT: return KEY_Left;
		case SDLK_DOWN: return KEY_Down;
		case SDLK_UP: return KEY_Up;
		// case SDLK_NUMLOCKCLEAR:
		case SDLK_KP_DIVIDE: return KEY_Divide;
		case SDLK_KP_MULTIPLY: return KEY_Multiply;
		case SDLK_KP_MINUS: return KEY_OEMMinus;
		case SDLK_KP_PLUS: return KEY_OEMPlus;
		case SDLK_KP_ENTER: return KEY_NumEnter;
		case SDLK_KP_1: return KEY_NumPad1;
		case SDLK_KP_2: return KEY_NumPad2;
		case SDLK_KP_3: return KEY_NumPad3;
		case SDLK_KP_4: return KEY_NumPad4;
		case SDLK_KP_5: return KEY_NumPad5;
		case SDLK_KP_6: return KEY_NumPad6;
		case SDLK_KP_7: return KEY_NumPad7;
		case SDLK_KP_8: return KEY_NumPad8;
		case SDLK_KP_9: return KEY_NumPad9;
		case SDLK_KP_0: return KEY_NumPad0;
		case SDLK_KP_PERIOD: return KEY_Decimal;
		// case SDLK_APPLICATION:
		// case SDLK_POWER:
		case SDLK_KP_EQUALS: return KEY_OEMEqual;
		case SDLK_F13: return KEY_F13;
		case SDLK_F14: return KEY_F14;
		case SDLK_F15: return KEY_F15;
		case SDLK_F16: return KEY_F16;
		case SDLK_F17: return KEY_F17;
		case SDLK_F18: return KEY_F18;
		case SDLK_F19: return KEY_F19;
		case SDLK_F20: return KEY_F20;
		case SDLK_F21: return KEY_F21;
		case SDLK_F22: return KEY_F22;
		case SDLK_F23: return KEY_F23;
		case SDLK_F24: return KEY_F24;
		// case SDLK_EXECUTE:
		// case SDLK_HELP:
		// case SDLK_MENU:
		// case SDLK_SELECT:
		// case SDLK_STOP:
		// case SDLK_AGAIN:
		// case SDLK_UNDO:
		// case SDLK_CUT:
		// case SDLK_COPY:
		// case SDLK_PASTE:
		// case SDLK_FIND:
		case SDLK_MUTE: return KEY_VolumeMute;
		case SDLK_VOLUMEUP: return KEY_VolumeUp;
		case SDLK_VOLUMEDOWN: return KEY_VolumeDown;
		case SDLK_KP_COMMA: return KEY_OEMComma;
		// case SDLK_KP_EQUALSAS400:
		// case SDLK_ALTERASE:
		// case SDLK_SYSREQ:
		// case SDLK_CANCEL:
		case SDLK_CLEAR: return KEY_Clear;
		// case SDLK_PRIOR:
		// case SDLK_RETURN2:
		case SDLK_SEPARATOR: return KEY_Seperator;
		// case SDLK_OUT:
		// case SDLK_OPER:
		// case SDLK_CLEARAGAIN:
		// case SDLK_CRSEL:
		// case SDLK_EXSEL:
		// case SDLK_KP_00:
		// case SDLK_KP_000:
		// case SDLK_THOUSANDSSEPARATOR:
		// case SDLK_DECIMALSEPARATOR:
		// case SDLK_CURRENCYUNIT:
		// case SDLK_CURRENCYSUBUNIT:
		// case SDLK_KP_LEFTPAREN:
		// case SDLK_KP_RIGHTPAREN:
		// case SDLK_KP_LEFTBRACE:
		// case SDLK_KP_RIGHTBRACE:
		case SDLK_KP_TAB: return KEY_Tab;
		case SDLK_KP_BACKSPACE: return KEY_Backspace;
		// case SDLK_KP_A:
		// case SDLK_KP_B:
		// case SDLK_KP_C:
		// case SDLK_KP_D:
		// case SDLK_KP_E:
		// case SDLK_KP_F:
		// case SDLK_KP_XOR:
		// case SDLK_KP_POWER:
		// case SDLK_KP_PERCENT:
		// case SDLK_KP_LESS:
		// case SDLK_KP_GREATER:
		// case SDLK_KP_AMPERSAND:
		// case SDLK_KP_DBLAMPERSAND:
		// case SDLK_KP_VERTICALBAR:
		// case SDLK_KP_DBLVERTICALBAR:
		// case SDLK_KP_COLON:
		// case SDLK_KP_HASH:
		case SDLK_KP_SPACE: return KEY_Space;
		// case SDLK_KP_AT:
		// case SDLK_KP_EXCLAM:
		// case SDLK_KP_MEMSTORE:
		// case SDLK_KP_MEMRECALL:
		// case SDLK_KP_MEMCLEAR:
		// case SDLK_KP_MEMADD:
		// case SDLK_KP_MEMSUBTRACT:
		// case SDLK_KP_MEMMULTIPLY:
		// case SDLK_KP_MEMDIVIDE:
		// case SDLK_KP_PLUSMINUS:
		case SDLK_KP_CLEAR: return KEY_Clear;
		// case SDLK_KP_CLEARENTRY:
		// case SDLK_KP_BINARY:
		// case SDLK_KP_OCTAL:
		// case SDLK_KP_DECIMAL:
		// case SDLK_KP_HEXADECIMAL:

		// option = alt, cmd = gui
		case SDLK_LCTRL: return KEY_LeftControl;
		case SDLK_LSHIFT: return KEY_LeftShift;
		case SDLK_LALT: return KEY_LeftMenu;
		case SDLK_LGUI: return KEY_LeftWindowsKey;
		case SDLK_RCTRL: return KEY_RightControl;
		case SDLK_RSHIFT: return KEY_RightShift;
		case SDLK_RALT: return KEY_RightMenu;
		case SDLK_RGUI: return KEY_RightWindowsKey;

		// case SDLK_MODE:
		// case SDLK_SLEEP:
		// case SDLK_WAKE:
		// case SDLK_CHANNEL_INCREMENT:
		// case SDLK_CHANNEL_DECREMENT:
		// case SDLK_MEDIA_PLAY:
		// case SDLK_MEDIA_PAUSE:
		// case SDLK_MEDIA_RECORD:
		// case SDLK_MEDIA_FAST_FORWARD:
		// case SDLK_MEDIA_REWIND:
		case SDLK_MEDIA_NEXT_TRACK: return KEY_NextTrack;
		case SDLK_MEDIA_PREVIOUS_TRACK: return KEY_PreviousTrack;
		case SDLK_MEDIA_STOP: return KEY_StopMedia;
		// case SDLK_MEDIA_EJECT:
		case SDLK_MEDIA_PLAY_PAUSE: return KEY_PlayPause;
		case SDLK_MEDIA_SELECT: return KEY_SelectMedia;
		// case SDLK_AC_NEW:
		// case SDLK_AC_OPEN:
		// case SDLK_AC_CLOSE:
		// case SDLK_AC_EXIT:
		// case SDLK_AC_SAVE:
		// case SDLK_AC_PRINT:
		// case SDLK_AC_PROPERTIES:
		case SDLK_AC_SEARCH: return KEY_BrowserSearch;
		case SDLK_AC_HOME: return KEY_BrowserHome;
		case SDLK_AC_BACK: return KEY_BrowserBack;
		case SDLK_AC_FORWARD: return KEY_BrowserForward;
		case SDLK_AC_STOP: return KEY_BrowserStop;
		case SDLK_AC_REFRESH: return KEY_BrowserRefresh;
		case SDLK_AC_BOOKMARKS: return KEY_BrowserFavorites;
		// case SDLK_SOFTLEFT:
		// case SDLK_SOFTRIGHT:
		// case SDLK_CALL:
		// case SDLK_ENDCALL:
		// case SDLK_LEFT_TAB:
		// case SDLK_LEVEL5_SHIFT:
		// case SDLK_MULTI_KEY_COMPOSE:
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		// case SDLK_LHYPER:
		// case SDLK_RHYPER:
		default:
		case SDLK_UNKNOWN: return KEY_Unknown;
	}
}

cocos2d::enumKeyCodes translate_gamepad_button(SDL_GamepadButton button) {
	using namespace cocos2d;

	switch (button) {
		case SDL_GAMEPAD_BUTTON_SOUTH: return CONTROLLER_A;
		case SDL_GAMEPAD_BUTTON_EAST: return CONTROLLER_B;
		case SDL_GAMEPAD_BUTTON_WEST: return CONTROLLER_X;
		case SDL_GAMEPAD_BUTTON_NORTH: return CONTROLLER_Y;

    case SDL_GAMEPAD_BUTTON_BACK: return CONTROLLER_Back;
    case SDL_GAMEPAD_BUTTON_START: return CONTROLLER_Start;

    case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER: return CONTROLLER_LB;
    case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER: return CONTROLLER_RB;

    case SDL_GAMEPAD_BUTTON_DPAD_UP: return CONTROLLER_Up;
    case SDL_GAMEPAD_BUTTON_DPAD_DOWN: return CONTROLLER_Down;
    case SDL_GAMEPAD_BUTTON_DPAD_LEFT: return CONTROLLER_Left;
    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT: return CONTROLLER_Right;

		case SDL_GAMEPAD_BUTTON_INVALID:
		default: return KEY_Unknown;
	}
}
