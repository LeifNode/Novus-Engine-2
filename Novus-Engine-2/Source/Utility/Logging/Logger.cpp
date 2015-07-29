#include "Logger.h"
#include "ILogSerializer.h"
#include <algorithm>
#include "ConsoleLogSerializer.h"

namespace novus
{

Logger* Logger::StaticInstance = nullptr;

Logger* Logger::GetInstance()
{
	if (StaticInstance == nullptr)
	{
		StaticInstance = new Logger();
	}

	return StaticInstance;
}

Logger::Logger()
{
	ConsoleSerializer = new ConsoleLogSerializer();
	AddSerializer(ConsoleSerializer);
}

Logger::~Logger()
{
	RemoveSerializer(ConsoleSerializer);
	delete ConsoleSerializer;
}

void Logger::Log(const wchar_t * message, const wchar_t * tag, LogLevel logLevel, const char * FileName, int lineNumber)
{
	LogEntry entry;
	entry.Message = message;
	entry.Tag = tag;
	entry.LogLevel = logLevel;
	entry.FileName = FileName;
	entry.LineNumber = lineNumber;

	LogHistory.push_back(entry);

	DispatchLogEvent(entry);
}

void Logger::AddSerializer(ILogSerializer * serializer)
{
	LogSerializers.push_back(serializer);
}

void Logger::RemoveSerializer(ILogSerializer * serializer)
{
	auto endIt = std::remove_if(LogSerializers.begin(), LogSerializers.end(),
		[serializer](const ILogSerializer* s) { return s == serializer; });

	LogSerializers.erase(endIt, LogSerializers.end());
}

void Logger::DispatchLogEvent(const LogEntry & entry) const
{
	for (const auto& serializer : LogSerializers)
	{
		serializer->Serialize(entry);
	}
}

};