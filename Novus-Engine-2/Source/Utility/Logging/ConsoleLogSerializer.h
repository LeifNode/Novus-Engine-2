/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

#include "ILogSerializer.h"

namespace novus
{

class ConsoleLogSerializer : public ILogSerializer
{
public:
	void Serialize(const Logger::LogEntry& entry) override;
};

}