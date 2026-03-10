#include "base.h"

#include <Geode/Geode.hpp>
#include <Geode/utils/Keyboard.hpp>

using namespace geode::prelude;

KeyboardModifier modifiers_from_keymod(SDL_Keymod flags) {
	KeyboardModifier modifiers = KeyboardModifier::None;

	if ((flags & SDL_KMOD_SHIFT) == SDL_KMOD_SHIFT) {
		modifiers |= KeyboardModifier::Shift;
	}
	if ((flags & SDL_KMOD_CTRL) == SDL_KMOD_CTRL) {
		modifiers |= KeyboardModifier::Control;
	}
	if ((flags & SDL_KMOD_ALT) == SDL_KMOD_ALT) {
		modifiers |= KeyboardModifier::Alt;
	}
	if ((flags & SDL_KMOD_GUI) == SDL_KMOD_GUI) {
		modifiers |= KeyboardModifier::Super;
	}

	return modifiers;
}

KeyboardModifier get_current_modifiers() {
	return modifiers_from_keymod(SDL_GetModState());
}

double translate_timestamp(std::uint64_t x) {
	auto relativeTime = SDLManager::get().m_timeDiff + x;
	return relativeTime/1'000'000'000.0;
}

cocos2d::CCPoint translate_display_point(const cocos2d::CCPoint x) {
	auto display_scale = 1.0f;
	if constexpr (!SDLManager::s_cocosHandlesScaling) {
		display_scale = SDLManager::get().m_displayScale;
	}

	return x * display_scale;
}

void dispatch_mouse_input_event(MouseInputData::Button event_button, bool down, double timestamp, int mouse_id, cocos2d::CCPoint base_point) {
	KeyboardModifier modifiers = get_current_modifiers();

	MouseInputData data(
		event_button,
		down
			? MouseInputData::Action::Press
			: MouseInputData::Action::Release,
		timestamp,
		modifiers
	);

	auto point = translate_display_point(base_point);

	Loader::get()->queueInMainThread([data = std::move(data), mouse_id, point] mutable {
		if (MouseInputEvent().send(data) != ListenerResult::Propagate) return;

		if (data.button == MouseInputData::Button::Left) {
			if (data.action == MouseInputData::Action::Press) {
				cocos2d::CCDirector::sharedDirector()->m_pobOpenGLView->handleTouchesBegin(1, &mouse_id, &point.x, &point.y, data.timestamp);
			} else {
				cocos2d::CCDirector::sharedDirector()->m_pobOpenGLView->handleTouchesEnd(1, &mouse_id, &point.x, &point.y, data.timestamp);
			}
		}
	});
}

void on_mouse_input(SDL_MouseButtonEvent& event) {
	auto event_button = MouseInputData::Button::Left;
	switch (event.button) {
		case SDL_BUTTON_RIGHT:
			event_button = MouseInputData::Button::Right;
			break;
		case SDL_BUTTON_MIDDLE:
			event_button = MouseInputData::Button::Middle;
			break;
		case SDL_BUTTON_X1:
			event_button = MouseInputData::Button::Button4;
			break;
		case SDL_BUTTON_X2:
			event_button = MouseInputData::Button::Button5;
			break;
	}

	double timestamp = translate_timestamp(event.timestamp);
	auto mouse_id = static_cast<int>(event.which);

	dispatch_mouse_input_event(event_button, event.down, timestamp, mouse_id, {event.x, event.y});
}

void on_mouse_move(SDL_MouseMotionEvent& event) {
	auto point = translate_display_point({event.x, event.y});

	bool is_left = (event.state & SDL_BUTTON_LEFT) != 0;
	auto mouse_id = static_cast<int>(event.which);
	auto timestamp = translate_timestamp(event.timestamp);

	Loader::get()->queueInMainThread([point, mouse_id, is_left, timestamp] mutable {
		if (MouseMoveEvent().send(point.x, point.y) != ListenerResult::Propagate) return;

		if (is_left) {
			cocos2d::CCDirector::sharedDirector()->m_pobOpenGLView->handleTouchesMove(1, &mouse_id, &point.x, &point.y, timestamp);
		}
	});
}

cocos2d::enumKeyCodes translate_keycode(SDL_Keycode code) {
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

void on_key_event(SDL_KeyboardEvent& event) {
	auto rawKeycode = event.key;

	// auto sdlKeycode = SDL_GetKeyFromScancode(event.scancode, event.mod, true);
	auto keyCode = translate_keycode(rawKeycode);
	auto timestamp = translate_timestamp(event.timestamp);

	auto modifiers = modifiers_from_keymod(event.mod);

	auto fullKeyCode = SDL_GetKeyFromScancode(event.scancode, event.mod, false);
	if ((fullKeyCode & SDLK_EXTENDED_MASK) == SDLK_EXTENDED_MASK || (fullKeyCode & SDLK_SCANCODE_MASK) == SDLK_SCANCODE_MASK) {
		fullKeyCode = SDLK_UNKNOWN;
	}

	auto down = event.type == SDL_EVENT_KEY_DOWN;

	KeyboardInputData data(
		keyCode,
		down ?
			event.repeat
				? KeyboardInputData::Action::Repeat
				: KeyboardInputData::Action::Press
			: KeyboardInputData::Action::Release,
		{ event.raw },
		timestamp,
		modifiers
	);

	Loader::get()->queueInMainThread([data = std::move(data), fullKeyCode] mutable {
		if (KeyboardInputEvent(data.key).send(data) != ListenerResult::Propagate) return;

    auto imeDispatcher = CCIMEDispatcher::sharedDispatcher();
    auto keyboardDispatcher = CCKeyboardDispatcher::get();

		auto timestamp = data.timestamp;
		auto isDown = data.action != KeyboardInputData::Action::Release;
		auto isRepeat = data.action == KeyboardInputData::Action::Repeat;
		auto keyCode = data.key;
		auto modifiers = data.modifiers;

		if (keyCode != KEY_Unknown && (!imeDispatcher->hasDelegate() || keyCode == KEY_Escape || keyCode == KEY_Enter)) {
			keyboardDispatcher->dispatchKeyboardMSG(keyCode, isDown, isRepeat, timestamp);
		}

    keyboardDispatcher->updateModifierKeys(
			modifiers & KeyboardModifier::Shift,
			modifiers & KeyboardModifier::Control,
			modifiers & KeyboardModifier::Alt,
			modifiers & KeyboardModifier::Super
    );

		if (data.action != KeyboardInputData::Action::Press && data.action != KeyboardInputData::Action::Repeat) return;

		if (keyCode == KEY_Backspace) {
			imeDispatcher->dispatchDeleteBackward();
		} else if (keyCode == KEY_Delete) {
			imeDispatcher->dispatchDeleteForward();
		} else if (keyCode == KEY_V && (modifiers & KeyboardModifier::Super) != KeyboardModifier::None) {
			auto str = geode::utils::clipboard::read();
			for (auto ch : str) {
				imeDispatcher->dispatchInsertText(&ch, 1, keyCode);
			}
		} else if (keyCode == KEY_Escape) {
			auto window = SDLManager::get().m_window;
			if (SDL_TextInputActive(window)) {
				SDL_StopTextInput(window);
			}
		} else {
			auto isArrowKey = keyCode == KEY_Left || keyCode == KEY_Right;
			if (isArrowKey || (fullKeyCode != SDLK_UNKNOWN && !SDL_TextInputActive(SDLManager::get().m_window))) {
				char ch = fullKeyCode;
				imeDispatcher->dispatchInsertText(&ch, 1, keyCode);
			}
		}
	});
}

void on_wheel_event(SDL_MouseWheelEvent& event) {
	// auto flip_wheel = event.direction == SDL_MOUSEWHEEL_FLIPPED;
	// based on user feedback, we should not account for the user's scroll setting
	auto flip_wheel = true;

	float x = flip_wheel ? -event.x : event.x;
	float y = flip_wheel ? -event.y : event.y;

	Loader::get()->queueInMainThread([x, y] {
		if (ScrollWheelEvent().send(x, y) != ListenerResult::Propagate) return;

    cocos2d::CCDirector::sharedDirector()->getMouseDispatcher()->dispatchScrollMSG(y, x);
	});
}

cocos2d::enumKeyCodes translate_gamepad_button(SDL_GamepadButton button) {
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

void dispatch_keypad_event(double timestamp, cocos2d::enumKeyCodes keyCode, bool down, uint64_t native) {
	auto modifiers = get_current_modifiers();

	KeyboardInputData data(
		keyCode,
		down
			? KeyboardInputData::Action::Press
			: KeyboardInputData::Action::Release,
		{ native },
		timestamp,
		modifiers
	);

	Loader::get()->queueInMainThread([data = std::move(data)] mutable {
		if (KeyboardInputEvent(data.key).send(data) != ListenerResult::Propagate) return;

    auto imeDispatcher = CCIMEDispatcher::sharedDispatcher();
    auto keyboardDispatcher = CCKeyboardDispatcher::get();

		auto timestamp = data.timestamp;
		auto isDown = data.action != KeyboardInputData::Action::Release;
		auto isRepeat = data.action == KeyboardInputData::Action::Repeat;
		auto keyCode = data.key;
		auto modifiers = data.modifiers;

		if (keyCode != KEY_Unknown && (!imeDispatcher->hasDelegate() || keyCode == KEY_Escape || keyCode == KEY_Enter)) {
			keyboardDispatcher->dispatchKeyboardMSG(keyCode, isDown, isRepeat, timestamp);
		}

    keyboardDispatcher->updateModifierKeys(
			modifiers & KeyboardModifier::Shift,
			modifiers & KeyboardModifier::Control,
			modifiers & KeyboardModifier::Alt,
			modifiers & KeyboardModifier::Super
    );
	});
}

void on_gamepad_button(SDL_GamepadButtonEvent& event) {
	auto timestamp = translate_timestamp(event.timestamp);
	auto keyCode = translate_gamepad_button(static_cast<SDL_GamepadButton>(event.button));
	auto down = event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN;

	if (keyCode == CONTROLLER_A) {
		SDLManager::get().m_mouseDown = down;
	} else if (keyCode == CONTROLLER_Back) {
		SDLManager::get().m_backDown = down;
	}

	dispatch_keypad_event(timestamp, keyCode, down, event.button);
}

std::unordered_map<cocos2d::enumKeyCodes, bool> running_axes{};

void handle_trigger(cocos2d::enumKeyCodes key, std::int16_t value, double timestamp) {
	auto is_down = value >= (SDL_JOYSTICK_AXIS_MAX * 0.11);
	auto current_value = running_axes[key];

	if (is_down == current_value) {
		return;
	}

	running_axes[key] = is_down;
	dispatch_keypad_event(timestamp, key, is_down, 0);
}

std::int8_t normalized_value_for_axis(std::int16_t value) {
	auto threshold = SDL_JOYSTICK_AXIS_MAX * 0.3;

	if (value > threshold) {
		return 1;
	} else if (value < -threshold) {
		return -1;
	}

	return 0;
}

void handle_stick(cocos2d::enumKeyCodes pos_key, cocos2d::enumKeyCodes neg_key, std::int16_t value, double timestamp) {
	auto state = normalized_value_for_axis(value);

	bool neg_value = state < 0;
	bool pos_value = state > 0;

	if (running_axes[pos_key] != pos_value) {
		running_axes[pos_key] = pos_value;
		dispatch_keypad_event(timestamp, pos_key, pos_value, 0);
	}

	if (running_axes[neg_key] != neg_value) {
		running_axes[neg_key] = neg_value;
		dispatch_keypad_event(timestamp, neg_key, neg_value, 0);
	}
}

void on_gamepad_axis(SDL_GamepadAxisEvent& event) {
	auto timestamp = translate_timestamp(event.timestamp);

	switch (event.axis) {
		case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
			handle_trigger(CONTROLLER_LT, event.value, timestamp);
			break;
		case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
			handle_trigger(CONTROLLER_RT, event.value, timestamp);
			break;

		case SDL_GAMEPAD_AXIS_LEFTX:
			if (std::abs(event.value) > SDL_JOYSTICK_AXIS_MAX*0.05) {
				SDLManager::get().m_cursorHorizontal = event.value / static_cast<float>(SDL_JOYSTICK_AXIS_MAX);
			} else {
				SDLManager::get().m_cursorHorizontal = 0.0f;
			}

			handle_stick(CONTROLLER_LTHUMBSTICK_RIGHT, CONTROLLER_LTHUMBSTICK_LEFT, event.value, timestamp);
			break;
		case SDL_GAMEPAD_AXIS_LEFTY:
			if (std::abs(event.value) > SDL_JOYSTICK_AXIS_MAX*0.05) {
				SDLManager::get().m_cursorVertical = event.value / static_cast<float>(SDL_JOYSTICK_AXIS_MAX);
			} else {
				SDLManager::get().m_cursorVertical = 0.0f;
			}

			handle_stick(CONTROLLER_LTHUMBSTICK_DOWN, CONTROLLER_LTHUMBSTICK_UP, event.value, timestamp);
			break;

		case SDL_GAMEPAD_AXIS_RIGHTX:
			if (std::abs(event.value) > SDL_JOYSTICK_AXIS_MAX*0.05) {
				SDLManager::get().m_scrollHorizontal = event.value / static_cast<float>(SDL_JOYSTICK_AXIS_MAX);
			} else {
				SDLManager::get().m_scrollHorizontal = 0.0f;
			}

			handle_stick(CONTROLLER_RTHUMBSTICK_RIGHT, CONTROLLER_RTHUMBSTICK_LEFT, event.value, timestamp);
			break;
		case SDL_GAMEPAD_AXIS_RIGHTY:
			if (std::abs(event.value) > SDL_JOYSTICK_AXIS_MAX*0.05) {
				SDLManager::get().m_scrollVertical = event.value / static_cast<float>(SDL_JOYSTICK_AXIS_MAX);
			} else {
				SDLManager::get().m_scrollVertical = 0.0f;
			}

			handle_stick(CONTROLLER_RTHUMBSTICK_DOWN, CONTROLLER_RTHUMBSTICK_UP, event.value, timestamp);
			break;
	}
}

void handle_resize(SDL_WindowEvent& event) {
	auto window_w = event.data1;
	auto window_h = event.data2;

	Loader::get()->queueInMainThread([window_w, window_h] mutable {
		resize_window(window_w, window_h);
	});
}

SDL_AppResult SDLCALL my_event_callback(void *appstate, SDL_Event *event) {
	auto appManager = reinterpret_cast<SDLManager*>(appstate);

	if (appManager->m_stopped) {
		return SDL_APP_SUCCESS;
	}

	switch (event->type) {
		case SDL_EVENT_QUIT:
			Loader::get()->queueInMainThread([] {
				trigger_shutdown();
			});
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			on_mouse_input(event->button);
			break;
		case SDL_EVENT_MOUSE_MOTION:
			on_mouse_move(event->motion);
			break;
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
			on_key_event(event->key);
			break;
		case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
		case SDL_EVENT_GAMEPAD_BUTTON_UP:
			on_gamepad_button(event->gbutton);
			break;
		case SDL_EVENT_GAMEPAD_AXIS_MOTION:
			on_gamepad_axis(event->gaxis);
			break;
		case SDL_EVENT_GAMEPAD_ADDED: {
			auto which = event->gdevice.which;
			auto gamepad = SDL_OpenGamepad(which);

			if (!gamepad) {
				geode::log::warn("gamepad {} added, but failed to open {}", which, SDL_GetError());
			} else {
				geode::log::debug("gamepad {} ({}) added", which, SDL_GetGamepadName(gamepad));
			}
			break;
		}
		case SDL_EVENT_GAMEPAD_REMOVED: {
			auto which = event->gdevice.which;
			auto gamepad = SDL_GetGamepadFromID(which);
			if (gamepad)
				SDL_CloseGamepad(gamepad);
			break;
		}
		case SDL_EVENT_WINDOW_RESIZED:
			if constexpr (SDLManager::s_cocosHandlesScaling) {
				handle_resize(event->window);
			}
			break;
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			if constexpr (!SDLManager::s_cocosHandlesScaling) {
				handle_resize(event->window);
			}
			break;
		case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
			update_display_scale();
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			on_wheel_event(event->wheel);
			break;
		case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: {
			// with exclusive fullscreen, zoom status should be untied from exclusive status
			auto exclusive_fullscreen = geode::Mod::get()->getSettingValue<bool>("exclusive-fullscreen");
			if (!exclusive_fullscreen) {
				GameManager::sharedState()->setGameVariable(GameVar::WindowedMode, false);
			}
			break;
		}
    case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: {
			auto exclusive_fullscreen = geode::Mod::get()->getSettingValue<bool>("exclusive-fullscreen");
			if (!exclusive_fullscreen) {
				GameManager::sharedState()->setGameVariable(GameVar::WindowedMode, true);
			}
			break;
		}
		case SDL_EVENT_TEXT_INPUT: {
			auto imeDispatcher = CCIMEDispatcher::sharedDispatcher();
			for (auto c : std::string_view{event->text.text}) {
				auto ch = c;
				imeDispatcher->dispatchInsertText(&ch, 1, KEY_Unknown);
			}
			break;
		}
		/*
		case SDL_EVENT_TEXT_EDITING:
			geode::log::info("SDL_EVENT_TEXT_EDITING: {} {} {}", event->edit.text, event->edit.start, event->edit.length);
			break;
		*/
		// knowingly ignoring these events
		/*
		case SDL_EVENT_WINDOW_SHOWN:
		case SDL_EVENT_DISPLAY_USABLE_BOUNDS_CHANGED:
		case SDL_EVENT_WINDOW_EXPOSED:
		case SDL_EVENT_WINDOW_MOVED:
		case SDL_EVENT_WINDOW_MINIMIZED:
		case SDL_EVENT_WINDOW_MAXIMIZED:
		case SDL_EVENT_WINDOW_RESTORED:
		case SDL_EVENT_WINDOW_RESIZED:
		case SDL_EVENT_WINDOW_MOUSE_ENTER:
		case SDL_EVENT_WINDOW_MOUSE_LEAVE:
		case SDL_EVENT_WINDOW_FOCUS_GAINED:
		case SDL_EVENT_WINDOW_FOCUS_LOST:
		case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
		case SDL_EVENT_WINDOW_OCCLUDED:
		case SDL_EVENT_KEYBOARD_ADDED:
		case SDL_EVENT_MOUSE_ADDED:
		case SDL_EVENT_CLIPBOARD_UPDATE:
			break;
		default:
			geode::log::debug("unknown sdl event recv {:#x}", event->type);
			break;
		*/
	}

	return SDL_APP_CONTINUE;
}

void update_mouse_position() {
	if (!SDLManager::get().m_cursorHidden) {
		auto cursorVertical = SDLManager::get().m_cursorVertical;
		auto cursorHorizontal = SDLManager::get().m_cursorHorizontal;

		static auto sentMouseState = false;
		static auto sentBackState = false;

		auto currentBackState = SDLManager::get().m_backDown;
		if (currentBackState != sentBackState) {
			auto timestamp = geode::utils::getInputTimestamp();
			dispatch_keypad_event(timestamp, KEY_Escape, currentBackState, 0);

			sentBackState = currentBackState;
		}

		auto currentMouseState = SDLManager::get().m_mouseDown;
		if (currentMouseState != sentMouseState) {
			float x, y;
			SDL_GetMouseState(&x, &y);

			dispatch_mouse_input_event(
				MouseInputData::Button::Left,
				currentMouseState,
				geode::utils::getInputTimestamp(),
				1,
				{x, y}
			);

			sentMouseState = currentMouseState;
		}

		if (cursorVertical != 0.0f || cursorHorizontal != 0.0f) {
			constexpr auto cursorScaleFactor = 12.0f;

			auto window = SDLManager::get().m_window;
			auto pushHoriz = cursorHorizontal * cursorScaleFactor;
			auto pushVert = cursorVertical * cursorScaleFactor;

			float x, y;
			SDL_GetMouseState(&x, &y);
			SDL_WarpMouseInWindow(window, x + pushHoriz, y + pushVert);

			auto point = translate_display_point({x, y});

			if (MouseMoveEvent().send(point.x, point.y) == ListenerResult::Propagate && currentMouseState) {
				auto timestamp = geode::utils::getInputTimestamp();

				auto mouse_id = 1;
				cocos2d::CCDirector::sharedDirector()->m_pobOpenGLView->handleTouchesMove(1, &mouse_id, &point.x, &point.y, timestamp);
			}
		}

		auto scrollVertical = SDLManager::get().m_scrollVertical;
		auto scrollHorizontal = SDLManager::get().m_scrollHorizontal;
		if (scrollVertical != 0.0f || scrollHorizontal != 0.0f) {
			constexpr auto scrollScaleFactor = 8.0f;

			auto x = -scrollHorizontal * scrollScaleFactor;
			auto y = scrollVertical * scrollScaleFactor;

			if (geode::ScrollWheelEvent().send(x, y) != geode::ListenerResult::Propagate) return;
			cocos2d::CCDirector::sharedDirector()->getMouseDispatcher()->dispatchScrollMSG(y, x);
		}
	}
}

void SDLManager::reset_controller_keys() {
	m_cursorHidden = false;
	m_cursorVertical = 0.0f;
	m_cursorHorizontal = 0.0f;
	m_scrollVertical = 0.0f;
	m_scrollHorizontal = 0.0f;
	m_mouseDown = false;
	m_backDown = false;
}
