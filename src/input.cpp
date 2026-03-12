#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_events.h>

#include <Geode/Geode.hpp>
#include <Geode/utils/Keyboard.hpp>

#include "base.h"
#include "keymap.h"
#include "platform.h"
#include "events.h"

using namespace geode::prelude;

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

void on_key_event(SDL_KeyboardEvent& event) {
	auto rawKeycode = event.key;

	// auto sdlKeycode = SDL_GetKeyFromScancode(event.scancode, event.mod, true);
	auto keyCode = translate_keycode(rawKeycode);
	auto timestamp = translate_timestamp(event.timestamp);

	auto modifiers = modifiers_from_keymod(event.mod);

	auto fullKeyCode = SDL_GetKeyFromScancode(event.scancode, event.mod, false);
	if ((fullKeyCode & SDLK_EXTENDED_MASK) != 0 || (fullKeyCode & SDLK_SCANCODE_MASK) != 0) {
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
			imeDispatcher->dispatchInsertText(str.data(), str.size(), keyCode);
		} else if (keyCode == KEY_Escape) {
			auto window = SDLManager::get().m_window;
			if (SDL_TextInputActive(window)) {
				SDL_StopTextInput(window);
			}
		} else {
			auto isArrowKey = keyCode == KEY_Left || keyCode == KEY_Right;
			if (isArrowKey) {
				auto ch = 'a';
				imeDispatcher->dispatchInsertText(&ch, 1, keyCode);
			} else if (fullKeyCode != SDLK_UNKNOWN && !SDL_TextInputActive(SDLManager::get().m_window)) {
				char outputBuf[4];
				auto end = SDL_UCS4ToUTF8(fullKeyCode, outputBuf);
				std::string_view toSend{outputBuf, end};

				imeDispatcher->dispatchInsertText(toSend.data(), toSend.size(), keyCode);
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

bool sdl_on_event(void* appstate, SDL_Event* event) {
	auto appManager = reinterpret_cast<SDLManager*>(appstate);

	if (appManager->m_stopped) {
		return false;
	}

	switch (event->type) {
		case SDL_EVENT_QUIT:
			Loader::get()->queueInMainThread([] {
				platform_trigger_shutdown();
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
				Loader::get()->queueInMainThread([] {
					GameManager::sharedState()->setGameVariable(GameVar::WindowedMode, false);
				});
			}
			break;
		}
    case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: {
			auto exclusive_fullscreen = geode::Mod::get()->getSettingValue<bool>("exclusive-fullscreen");
			if (!exclusive_fullscreen) {
				Loader::get()->queueInMainThread([] {
					GameManager::sharedState()->setGameVariable(GameVar::WindowedMode, true);
				});
			}
			break;
		}
		case SDL_EVENT_TEXT_INPUT: {
			std::string data{event->text.text};
			Loader::get()->queueInMainThread([data = std::move(data)] {
				auto imeDispatcher = CCIMEDispatcher::sharedDispatcher();
				imeDispatcher->dispatchInsertText(data.data(), data.size(), KEY_Unknown);
			});
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

	return true;
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
