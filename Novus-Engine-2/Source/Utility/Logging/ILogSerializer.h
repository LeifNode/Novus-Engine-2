/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

#include "Logger.h"

namespace novus
{

class ILogSerializer
{
public:
	virtual void Serialize(const Logger::LogEntry& entry) = 0;
};

};