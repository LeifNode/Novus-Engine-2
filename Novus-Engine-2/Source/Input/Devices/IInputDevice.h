/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

#include <stdint.h>

namespace novus
{

class InputEvent
{
protected:
	InputEvent(uint32_t id)
		:EventId(id)
	{}

public:
	uint32_t GetEventID() const { return EventId; }

private:
	uint32_t EventId;
};

class IInputDevice
{
public:
	/**
	 *	Poll the input device for the state of its buttons and other inputs
	 */
	virtual void PollInputDevice() = 0;

	virtual void Init();

	virtual bool IsAvailable(uint32_t deviceIndex = 0) const = 0;
	virtual bool IsEnabled(uint32_t deviceIndex = 0) const = 0;
};

};