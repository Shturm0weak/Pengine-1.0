#include "Timer.h"

#include "Utils.h"

using namespace Pengine;

void Timer::UpdateCallbacks()
{
	for (size_t i = 0; i < s_Callbacks.size();)
	{
		s_Callbacks[i].second -= Time::GetDeltaTime();
		if (s_Callbacks[i].second < 0.0f)
		{
			s_Callbacks[i].first();
			s_Callbacks.erase(s_Callbacks.begin() + i);
			continue;
		}
		else
		{
			i++;
		}
	}
}

Timer::Timer(bool showTime, double* outTime) : m_ShowTime(showTime), m_OutTime(outTime)
{
	m_StartTimePoint = std::chrono::high_resolution_clock::now();
}

Timer::~Timer()
{
	Stop();
}

void Timer::Stop()
{
	auto endTimePoint = std::chrono::high_resolution_clock::now();
	auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch().count();
	auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();
	auto duration = end - start;
	double ms = duration * 0.01;

	if (m_ShowTime)
	{
		std::setprecision(6);
		std::cout << duration << "us (" << ms << "ms)\n";
	}

	if (m_OutTime)
	{
		*m_OutTime = ms;
	}
}