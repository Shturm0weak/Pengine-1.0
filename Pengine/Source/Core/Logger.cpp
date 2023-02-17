#include "Logger.h"

#include <ctime>
#include <iostream>

using namespace Pengine;

void Logger::UpdateTime()
{
	std::time_t time = std::time(0);
	Logger::s_CurrentTime = std::localtime(&time);
	Logger::s_TimeString = "["
		+ std::to_string(Logger::s_CurrentTime->tm_hour)
		+ ":" + std::to_string(Logger::s_CurrentTime->tm_min)
		+ ":" + std::to_string(Logger::s_CurrentTime->tm_sec)
		+ "]";
}

void Logger::Log(const char* message, const char* color, const char* type)
{
	std::cout << color << Logger::s_TimeString << type << ":" << message << RESET << "\n";
}

void Logger::Log(const char* message, const char* type, const char* name, const char* color, const char* mode)
{
	std::cout << color << Logger::s_TimeString << mode << ":" << NAMECOLOR << type << color << ": <" << NAMECOLOR << name << color << "> " << message << "\n" << RESET;
}

void Logger::Error(const char* message)
{
	Logger::Log(message, BOLDRED, "ERROR");
}

void Logger::Error(const char* message, const char* type, const char* name)
{
	Log(message, type, name, BOLDRED, "ERROR");
}

void Logger::Warning(const char* message)
{
	Logger::Log(message, BOLDYELLOW, "WARNING");
}

void Logger::Warning(const char* message, const char* type, const char* name)
{
	Log(message, type, name, BOLDYELLOW, "WARNING");
}

void Logger::Success(const char* message)
{
	Logger::Log(message, BOLDGREEN, "SUCCESS");
}

void Logger::Success(const char* message, const char* type, const char* name)
{
	Log(message, type, name, BOLDGREEN, "SUCCESS");
}
