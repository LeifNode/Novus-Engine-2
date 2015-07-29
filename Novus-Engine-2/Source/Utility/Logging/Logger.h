/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

#include <cstdint>
#include <vector>

#define NE_LOG(message, tag, logLevel) (novus::Logger::GetInstance()->Log(message, tag, logLevel, __FILE__, __LINE__))
#define NE_MESSAGE(message, tag) (novus::Logger::GetInstance()->Log(message, tag, novus::LogLevel::Message, __FILE__, __LINE__))
#define NE_WARN(message, tag) (novus::Logger::GetInstance()->Log(message, tag, novus::LogLevel::Warning, __FILE__, __LINE__))
#define NE_ERROR(message, tag) (novus::Logger::GetInstance()->Log(message, tag, novus::LogLevel::Error, __FILE__, __LINE__))
#define NE_CRITICAL(message, tag) (novus::Logger::GetInstance()->Log(message, tag, novus::LogLevel::Critical, __FILE__, __LINE__))

namespace novus
{

class ILogSerializer;

enum class LogLevel : uint8_t
{
	Message,
	Warning,
	Error,
	Critical
};

class Logger
{
public:
	struct LogEntry
	{
		std::wstring Message;
		std::wstring Tag;
		LogLevel LogLevel;
		std::string FileName;
		int LineNumber;
	};

public:
	static Logger* GetInstance();

	void Log(const wchar_t* message, const wchar_t* tag, LogLevel logLevel, const char* fileName, int lineNumber);

	void AddSerializer(ILogSerializer* serializer);
	void RemoveSerializer(ILogSerializer* serializer);

private:
	//Only allow access via GetInstance
	Logger();
	~Logger();
	Logger(const Logger&) {}
	Logger& operator= (const Logger&) {}

	void DispatchLogEvent(const LogEntry& entry) const;

private:
	static Logger* StaticInstance;

	ILogSerializer* ConsoleSerializer;

	std::vector<LogEntry> LogHistory;
	std::vector<ILogSerializer*> LogSerializers;
};

};