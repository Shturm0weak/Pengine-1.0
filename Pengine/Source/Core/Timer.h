#pragma once

#include "Core.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <functional>

namespace Pengine
{

	class PENGINE_API Timer 
	{
	private:

		static std::vector<std::pair<std::function<void()>, float>> s_Callbacks;

		double* m_OutTime = nullptr;

		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimePoint;
	
		bool m_ShowTime = false;

	public:

		static void SetCallback(std::function<void()> callback, float time) { s_Callbacks.push_back(std::make_pair(callback, time)); };

		static void UpdateCallbacks();

		Timer(bool showTime = true, double* outTime = nullptr);

		~Timer();

		void Stop();
	};

}