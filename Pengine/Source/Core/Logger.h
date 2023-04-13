#pragma once

#include "Core.h"
#include "../Enums/ColoredOutput.h"

namespace Pengine
{

	class PENGINE_API Logger
	{
	private:

		static std::tm* s_CurrentTime;
		static std::string s_TimeString;

		static void UpdateTime();

		friend class Window;
	public:

		static std::string GetTimeString() { return s_TimeString; }

		static void Log(const char* message, const char* color = RESET, const char* type = "LOG");
		
		static void Log(const char* message, const char* type, const char* name, const char* color, const char* mode = "LOG");
		
		static void Error(const char* message);
		
		static void Error(const char* message, const char* type, const char* name);
		
		static void Warning(const char* message);
		
		static void Warning(const char* message, const char* type, const char* name);
		
		static void Success(const char* message);
		
		static void Success(const char* message, const char* type, const char* name);
	};

}