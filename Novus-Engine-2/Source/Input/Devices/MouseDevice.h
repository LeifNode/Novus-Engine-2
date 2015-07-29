/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

#include "IInputDevice.h"
#include "Math/Vector2.h"

namespace novus
{

struct MouseState
{
	MouseState()
		:Left(false),
		Middle(false),
		Right(false),
		XButton1(false),
		XButton2(false),
		ScrollPosition(0)
	{}

	bool Left, Middle, Right, XButton1, XButton2;
	int ScrollPosition;
	Vector2i Position;
};

class MouseDevice : public IInputDevice
{
public:
	const MouseState& GetLastMouseState() const { return LastMouseState; }
	const MouseState& GetCurrentMouseState() const { return CurrentMoustState; }

private:
	MouseState LastMouseState;
	MouseState CurrentMoustState;
};

}