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

cocos2d::enumKeyCodes translate_scancode(SDL_Scancode code) {
	using namespace cocos2d;

	switch (code) {
		case SDL_SCANCODE_A: return KEY_A;
		case SDL_SCANCODE_B: return KEY_B;
		case SDL_SCANCODE_C: return KEY_C;
		case SDL_SCANCODE_D: return KEY_D;
		case SDL_SCANCODE_E: return KEY_E;
		case SDL_SCANCODE_F: return KEY_F;
		case SDL_SCANCODE_G: return KEY_G;
		case SDL_SCANCODE_H: return KEY_H;
		case SDL_SCANCODE_I: return KEY_I;
		case SDL_SCANCODE_J: return KEY_J;
		case SDL_SCANCODE_K: return KEY_K;
		case SDL_SCANCODE_L: return KEY_L;
		case SDL_SCANCODE_M: return KEY_M;
		case SDL_SCANCODE_N: return KEY_N;
		case SDL_SCANCODE_O: return KEY_O;
		case SDL_SCANCODE_P: return KEY_P;
		case SDL_SCANCODE_Q: return KEY_Q;
		case SDL_SCANCODE_R: return KEY_R;
		case SDL_SCANCODE_S: return KEY_S;
		case SDL_SCANCODE_T: return KEY_T;
		case SDL_SCANCODE_U: return KEY_U;
		case SDL_SCANCODE_V: return KEY_V;
		case SDL_SCANCODE_W: return KEY_W;
		case SDL_SCANCODE_X: return KEY_X;
		case SDL_SCANCODE_Y: return KEY_Y;
		case SDL_SCANCODE_Z: return KEY_Z;

		case SDL_SCANCODE_1: return KEY_One;
		case SDL_SCANCODE_2: return KEY_Two;
		case SDL_SCANCODE_3: return KEY_Three;
		case SDL_SCANCODE_4: return KEY_Four;
		case SDL_SCANCODE_5: return KEY_Five;
		case SDL_SCANCODE_6: return KEY_Six;
		case SDL_SCANCODE_7: return KEY_Seven;
		case SDL_SCANCODE_8: return KEY_Eight;
		case SDL_SCANCODE_9: return KEY_Nine;
		case SDL_SCANCODE_0: return KEY_Zero;

		case SDL_SCANCODE_RETURN: return KEY_Enter;
		case SDL_SCANCODE_ESCAPE: return KEY_Escape;
		case SDL_SCANCODE_BACKSPACE: return KEY_Backspace;
		case SDL_SCANCODE_TAB: return KEY_Tab;
		case SDL_SCANCODE_SPACE: return KEY_Space;

		case SDL_SCANCODE_MINUS: return KEY_OEMMinus;
		case SDL_SCANCODE_EQUALS: return KEY_Equal;
		case SDL_SCANCODE_LEFTBRACKET: return KEY_LeftBracket;
		case SDL_SCANCODE_RIGHTBRACKET: return KEY_RightBracket;
		case SDL_SCANCODE_BACKSLASH: return KEY_Backslash;
		case SDL_SCANCODE_NONUSHASH: return KEY_Backslash;
		case SDL_SCANCODE_SEMICOLON: return KEY_Semicolon;
		case SDL_SCANCODE_APOSTROPHE: return KEY_Apostrophe;
		case SDL_SCANCODE_GRAVE: return KEY_GraveAccent;
		case SDL_SCANCODE_COMMA: return KEY_OEMComma;
		case SDL_SCANCODE_PERIOD: return KEY_OEMPeriod;
		case SDL_SCANCODE_SLASH: return KEY_Slash;

		case SDL_SCANCODE_CAPSLOCK: return KEY_CapsLock;

		case SDL_SCANCODE_F1: return KEY_F1;
		case SDL_SCANCODE_F2: return KEY_F2;
		case SDL_SCANCODE_F3: return KEY_F3;
		case SDL_SCANCODE_F4: return KEY_F4;
		case SDL_SCANCODE_F5: return KEY_F5;
		case SDL_SCANCODE_F6: return KEY_F6;
		case SDL_SCANCODE_F7: return KEY_F7;
		case SDL_SCANCODE_F8: return KEY_F8;
		case SDL_SCANCODE_F9: return KEY_F9;
		case SDL_SCANCODE_F10: return KEY_F10;
		case SDL_SCANCODE_F11: return KEY_F11;
		case SDL_SCANCODE_F12: return KEY_F12;

		case SDL_SCANCODE_PRINTSCREEN: return KEY_PrintScreen;
		case SDL_SCANCODE_SCROLLLOCK: return KEY_ScrollLock;
		case SDL_SCANCODE_PAUSE: return KEY_Pause;
		case SDL_SCANCODE_INSERT: return KEY_Insert;
		case SDL_SCANCODE_HOME: return KEY_Home;
		case SDL_SCANCODE_PAGEUP: return KEY_PageUp;
		case SDL_SCANCODE_DELETE: return KEY_Delete;
		case SDL_SCANCODE_END: return KEY_End;
		case SDL_SCANCODE_PAGEDOWN: return KEY_PageDown;
		case SDL_SCANCODE_RIGHT: return KEY_Right;
		case SDL_SCANCODE_LEFT: return KEY_Left;
		case SDL_SCANCODE_DOWN: return KEY_Down;
		case SDL_SCANCODE_UP: return KEY_Up;

		case SDL_SCANCODE_NUMLOCKCLEAR: return KEY_Clear;

		case SDL_SCANCODE_KP_DIVIDE: return KEY_Divide;
		case SDL_SCANCODE_KP_MULTIPLY: return KEY_Multiply;
		case SDL_SCANCODE_KP_MINUS: return KEY_OEMMinus;
		case SDL_SCANCODE_KP_PLUS: return KEY_OEMPlus;
		case SDL_SCANCODE_KP_ENTER: return KEY_NumEnter;
		case SDL_SCANCODE_KP_1: return KEY_NumPad1;
		case SDL_SCANCODE_KP_2: return KEY_NumPad2;
		case SDL_SCANCODE_KP_3: return KEY_NumPad3;
		case SDL_SCANCODE_KP_4: return KEY_NumPad4;
		case SDL_SCANCODE_KP_5: return KEY_NumPad5;
		case SDL_SCANCODE_KP_6: return KEY_NumPad6;
		case SDL_SCANCODE_KP_7: return KEY_NumPad7;
		case SDL_SCANCODE_KP_8: return KEY_NumPad8;
		case SDL_SCANCODE_KP_9: return KEY_NumPad9;
		case SDL_SCANCODE_KP_0: return KEY_NumPad0;
		case SDL_SCANCODE_KP_PERIOD: return KEY_Decimal;

		// case SDL_SCANCODE_NONUSBACKSLASH: return;
		case SDL_SCANCODE_APPLICATION: return KEY_ApplicationsKey;
		// case SDL_SCANCODE_POWER: return;
		case SDL_SCANCODE_KP_EQUALS: return KEY_OEMEqual;
		case SDL_SCANCODE_F13: return KEY_F13;
		case SDL_SCANCODE_F14: return KEY_F14;
		case SDL_SCANCODE_F15: return KEY_F15;
		case SDL_SCANCODE_F16: return KEY_F16;
		case SDL_SCANCODE_F17: return KEY_F17;
		case SDL_SCANCODE_F18: return KEY_F18;
		case SDL_SCANCODE_F19: return KEY_F19;
		case SDL_SCANCODE_F20: return KEY_F20;
		case SDL_SCANCODE_F21: return KEY_F21;
		case SDL_SCANCODE_F22: return KEY_F22;
		case SDL_SCANCODE_F23: return KEY_F23;
		case SDL_SCANCODE_F24: return KEY_F24;
		case SDL_SCANCODE_EXECUTE: return KEY_Execute;
		case SDL_SCANCODE_HELP: return KEY_Help;
		// i'm not really sure how to map these tbh but whatever
		// case SDL_SCANCODE_MENU: return;
		case SDL_SCANCODE_SELECT: return KEY_Select;
		// case SDL_SCANCODE_STOP: return;
		// case SDL_SCANCODE_AGAIN: return;
		// case SDL_SCANCODE_UNDO: return;
		// case SDL_SCANCODE_CUT: return;
		// case SDL_SCANCODE_COPY: return;
		// case SDL_SCANCODE_PASTE: return;
		// case SDL_SCANCODE_FIND: return;
		case SDL_SCANCODE_MUTE: return KEY_VolumeMute;
		case SDL_SCANCODE_VOLUMEUP: return KEY_VolumeUp;
		case SDL_SCANCODE_VOLUMEDOWN: return KEY_VolumeDown;
		case SDL_SCANCODE_KP_COMMA: return KEY_Seperator;
		// case SDL_SCANCODE_KP_EQUALSAS400: return;

		// case SDL_SCANCODE_INTERNATIONAL1: return;
		// case SDL_SCANCODE_INTERNATIONAL2: return;
		// case SDL_SCANCODE_INTERNATIONAL3: return;
		// case SDL_SCANCODE_INTERNATIONAL4: return;
		// case SDL_SCANCODE_INTERNATIONAL5: return;
		// case SDL_SCANCODE_INTERNATIONAL6: return;
		// case SDL_SCANCODE_INTERNATIONAL7: return;
		// case SDL_SCANCODE_INTERNATIONAL8: return;
		// case SDL_SCANCODE_INTERNATIONAL9: return;
		// case SDL_SCANCODE_LANG1: return;
		// case SDL_SCANCODE_LANG2: return;
		// case SDL_SCANCODE_LANG3: return;
		// case SDL_SCANCODE_LANG4: return;
		// case SDL_SCANCODE_LANG5: return;
		// case SDL_SCANCODE_LANG6: return;
		// case SDL_SCANCODE_LANG7: return;
		// case SDL_SCANCODE_LANG8: return;
		// case SDL_SCANCODE_LANG9: return;

		// i wouldn't be surprised if these had cocos binds too, but differently named
		// case SDL_SCANCODE_ALTERASE: return;
		// case SDL_SCANCODE_SYSREQ: return;
		// case SDL_SCANCODE_CANCEL: return;
		case SDL_SCANCODE_CLEAR: return KEY_Clear;
		// case SDL_SCANCODE_PRIOR: return;
		// case SDL_SCANCODE_RETURN2: return;
		case SDL_SCANCODE_SEPARATOR: return KEY_Seperator;
		// case SDL_SCANCODE_OUT: return;
		// case SDL_SCANCODE_OPER: return;
		// case SDL_SCANCODE_CLEARAGAIN: return;
		case SDL_SCANCODE_CRSEL: return KEY_CrSel;
		case SDL_SCANCODE_EXSEL: return KEY_ExSel;

		// case SDL_SCANCODE_KP_00: return;
		// case SDL_SCANCODE_KP_000: return;
		// case SDL_SCANCODE_THOUSANDSSEPARATOR: return;
		// case SDL_SCANCODE_DECIMALSEPARATOR: return;
		// case SDL_SCANCODE_CURRENCYUNIT: return;
		// case SDL_SCANCODE_CURRENCYSUBUNIT: return;
		// case SDL_SCANCODE_KP_LEFTPAREN: return;
		// case SDL_SCANCODE_KP_RIGHTPAREN: return;
		// case SDL_SCANCODE_KP_LEFTBRACE: return;
		// case SDL_SCANCODE_KP_RIGHTBRACE: return;
		case SDL_SCANCODE_KP_TAB: return KEY_Tab;
		case SDL_SCANCODE_KP_BACKSPACE: return KEY_Backspace;
		// case SDL_SCANCODE_KP_A: return;
		// case SDL_SCANCODE_KP_B: return;
		// case SDL_SCANCODE_KP_C: return;
		// case SDL_SCANCODE_KP_D: return;
		// case SDL_SCANCODE_KP_E: return;
		// case SDL_SCANCODE_KP_F: return;
		// case SDL_SCANCODE_KP_XOR: return;
		// case SDL_SCANCODE_KP_POWER: return;
		// case SDL_SCANCODE_KP_PERCENT: return;
		// case SDL_SCANCODE_KP_LESS: return;
		// case SDL_SCANCODE_KP_GREATER: return;
		// case SDL_SCANCODE_KP_AMPERSAND: return;
		// case SDL_SCANCODE_KP_DBLAMPERSAND: return;
		// case SDL_SCANCODE_KP_VERTICALBAR: return;
		// case SDL_SCANCODE_KP_DBLVERTICALBAR: return;
		// case SDL_SCANCODE_KP_COLON: return;
		// case SDL_SCANCODE_KP_HASH: return;
		case SDL_SCANCODE_KP_SPACE: return KEY_Space;
		// case SDL_SCANCODE_KP_AT: return;
		// case SDL_SCANCODE_KP_EXCLAM: return;
		// case SDL_SCANCODE_KP_MEMSTORE: return;
		// case SDL_SCANCODE_KP_MEMRECALL: return;
		// case SDL_SCANCODE_KP_MEMCLEAR: return;
		// case SDL_SCANCODE_KP_MEMADD: return;
		// case SDL_SCANCODE_KP_MEMSUBTRACT: return;
		// case SDL_SCANCODE_KP_MEMMULTIPLY: return;
		// case SDL_SCANCODE_KP_MEMDIVIDE: return;
		// case SDL_SCANCODE_KP_PLUSMINUS: return;
		case SDL_SCANCODE_KP_CLEAR: return KEY_Clear;
		// case SDL_SCANCODE_KP_CLEARENTRY: return;
		// case SDL_SCANCODE_KP_BINARY: return;
		// case SDL_SCANCODE_KP_OCTAL: return;
		// case SDL_SCANCODE_KP_DECIMAL: return;
		// case SDL_SCANCODE_KP_HEXADECIMAL: return;

		case SDL_SCANCODE_LCTRL: return KEY_LeftControl;
		case SDL_SCANCODE_LSHIFT: return KEY_LeftShift;
		case SDL_SCANCODE_LALT: return KEY_LeftMenu;
		case SDL_SCANCODE_LGUI: return KEY_LeftWindowsKey;
		case SDL_SCANCODE_RCTRL: return KEY_RightControl;
		case SDL_SCANCODE_RSHIFT: return KEY_RightShift;
		case SDL_SCANCODE_RALT: return KEY_RightMenu;
		case SDL_SCANCODE_RGUI: return KEY_RightWindowsKey;

		// case SDL_SCANCODE_MODE: return;

		case SDL_SCANCODE_SLEEP: return KEY_Sleep;
		// case SDL_SCANCODE_WAKE: return;

		// case SDL_SCANCODE_CHANNEL_INCREMENT: return;
		// case SDL_SCANCODE_CHANNEL_DECREMENT: return;

		case SDL_SCANCODE_MEDIA_PLAY: return KEY_Play;
		case SDL_SCANCODE_MEDIA_PAUSE: return KEY_Pause;
		// case SDL_SCANCODE_MEDIA_RECORD: return;
		// case SDL_SCANCODE_MEDIA_FAST_FORWARD: return;
		// case SDL_SCANCODE_MEDIA_REWIND: return;
		case SDL_SCANCODE_MEDIA_NEXT_TRACK: return KEY_NextTrack;
		case SDL_SCANCODE_MEDIA_PREVIOUS_TRACK: return KEY_PreviousTrack;
		case SDL_SCANCODE_MEDIA_STOP: return KEY_StopMedia;
		// case SDL_SCANCODE_MEDIA_EJECT: return;
		case SDL_SCANCODE_MEDIA_PLAY_PAUSE: return KEY_PlayPause;
		case SDL_SCANCODE_MEDIA_SELECT: return KEY_SelectMedia;

		// case SDL_SCANCODE_AC_NEW: return;
		// case SDL_SCANCODE_AC_OPEN: return;
		// case SDL_SCANCODE_AC_CLOSE: return;
		// case SDL_SCANCODE_AC_EXIT: return;
		// case SDL_SCANCODE_AC_SAVE: return;
		// case SDL_SCANCODE_AC_PRINT: return;
		// case SDL_SCANCODE_AC_PROPERTIES: return;

		case SDL_SCANCODE_AC_SEARCH: return KEY_BrowserSearch;
		case SDL_SCANCODE_AC_HOME: return KEY_BrowserHome;
		case SDL_SCANCODE_AC_BACK: return KEY_BrowserBack;
		case SDL_SCANCODE_AC_FORWARD: return KEY_BrowserForward;
		case SDL_SCANCODE_AC_STOP: return KEY_BrowserStop;
		case SDL_SCANCODE_AC_REFRESH: return KEY_BrowserRefresh;
		case SDL_SCANCODE_AC_BOOKMARKS: return KEY_BrowserFavorites;
		default: return KEY_Unknown;
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

		case SDL_GAMEPAD_BUTTON_LEFT_STICK:
			return static_cast<cocos2d::enumKeyCodes>(ExtraEnumKeyCodes::Controller_L3);
		case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
			return static_cast<cocos2d::enumKeyCodes>(ExtraEnumKeyCodes::Controller_R3);

		case SDL_GAMEPAD_BUTTON_GUIDE:
			return static_cast<cocos2d::enumKeyCodes>(ExtraEnumKeyCodes::Controller_Guide);
		case SDL_GAMEPAD_BUTTON_MISC1:
			return static_cast<cocos2d::enumKeyCodes>(ExtraEnumKeyCodes::Controller_Misc1);
    case SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1:
			return static_cast<cocos2d::enumKeyCodes>(ExtraEnumKeyCodes::Controller_RightPaddle1);
		case SDL_GAMEPAD_BUTTON_LEFT_PADDLE1:
			return static_cast<cocos2d::enumKeyCodes>(ExtraEnumKeyCodes::Controller_LeftPaddle1);
    case SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2:
			return static_cast<cocos2d::enumKeyCodes>(ExtraEnumKeyCodes::Controller_RightPaddle2);
    case SDL_GAMEPAD_BUTTON_LEFT_PADDLE2:
			return static_cast<cocos2d::enumKeyCodes>(ExtraEnumKeyCodes::Controller_LeftPaddle2);
    case SDL_GAMEPAD_BUTTON_TOUCHPAD:
			return static_cast<cocos2d::enumKeyCodes>(ExtraEnumKeyCodes::Controller_Touchpad);
    case SDL_GAMEPAD_BUTTON_MISC2:
			return static_cast<cocos2d::enumKeyCodes>(ExtraEnumKeyCodes::Controller_Misc2);
    case SDL_GAMEPAD_BUTTON_MISC3:
			return static_cast<cocos2d::enumKeyCodes>(ExtraEnumKeyCodes::Controller_Misc3);
    case SDL_GAMEPAD_BUTTON_MISC4:
			return static_cast<cocos2d::enumKeyCodes>(ExtraEnumKeyCodes::Controller_Misc4);
    case SDL_GAMEPAD_BUTTON_MISC5:
			return static_cast<cocos2d::enumKeyCodes>(ExtraEnumKeyCodes::Controller_Misc5);
    case SDL_GAMEPAD_BUTTON_MISC6:
			return static_cast<cocos2d::enumKeyCodes>(ExtraEnumKeyCodes::Controller_Misc6);

		case SDL_GAMEPAD_BUTTON_INVALID:
		default: return KEY_Unknown;
	}
}
