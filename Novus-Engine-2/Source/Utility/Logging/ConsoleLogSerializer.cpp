#include "ConsoleLogSerializer.h"
#include <iostream>

void novus::ConsoleLogSerializer::Serialize(const novus::Logger::LogEntry& entry)
{
	std::wcout << L"[" << entry.Tag.c_str() << L"] " << entry.Message.c_str() << std::endl;
}