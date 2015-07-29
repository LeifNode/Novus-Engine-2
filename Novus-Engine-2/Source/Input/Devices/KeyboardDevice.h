/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

#include "IInputDevice.h"
#include <vector>

namespace novus
{

enum class KeyboardKey : uint8_t
{
	Backspace = 8,
	Tab,
	Return = 13,
	Enter = 13,
	Shift = 16,
	Control,
	Alt,

	Escape = 27,

	Space = 32,

	Left = 37,
	Up,
	Right,
	Down,

	Num0 = 48,
	Num1,
	Num2,
	Num3,
	Num4,
	Num5,
	Num6,
	Num7,
	Num8,
	Num9,

	A = 65,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,

	NumPad0 = 96,
	NumPad1,
	NumPad2,
	NumPad3,
	NumPad4,
	NumPad5,
	NumPad6,
	NumPad7,
	NumPad8,
	NumPad9,
	Multiply,
	Add,
	Seperator,
	Subtract,
	Decimal,
	Divide,

	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,

	LeftShift = 160,
	RightShift,
	LeftControl,
	RightControl,

	Tilde = 192,
	KEY_COUNT,
};

class KeyPressEvent : InputEvent
{
public:
	static const uint32_t EventID = 0xcb92;

	KeyPressEvent(KeyboardKey key)
		:InputEvent(EventID),
		Key(key)
	{}

private:
	KeyboardKey Key;
};

class KeyReleaseEvent : InputEvent
{
public:
	static const uint32_t EventID = 0x46c4;

	KeyReleaseEvent(KeyboardKey key)
		:InputEvent(EventID),
		Key(key)
	{}

private:
	KeyboardKey Key;
};

struct KeyboardState
{
	KeyboardState()
	{
		Keys.resize(static_cast<size_t>(KeyboardKey::KEY_COUNT), false);
	}

	bool IsKeyPressed(KeyboardKey key) const { return Keys[static_cast<size_t>(key)]; }
	void SetKey(KeyboardKey key, bool pressed) { Keys[static_cast<size_t>(key)] = pressed; }

	std::vector<bool> Keys;
};

class KeyboardDevice : public IInputDevice
{
public:
	const KeyboardState& GetLastKeyboardState() const { return LastKeyboardState; }
	const KeyboardState& GetCurrentKeyboardState() const { return CurrentKeyboardState; }


private:
	KeyboardState LastKeyboardState;
	KeyboardState CurrentKeyboardState;
};

}