#pragma once

#include <glew.h>
#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>
#include <mutex>

namespace Pengine
{

	class PENGINE_API Time
	{
	private:

		std::mutex m_Mtx;
		float m_Time;
		float m_LastTime;
		float m_DeltaTime;
		float m_AverageDeltaTime;

		Time() = default;
		Time(const Time&) = delete;
		Time& operator=(const Time&) { return *this; }
		~Time() = default;

		static Time& GetInstance()
		{
			static Time deltaTime;
			return deltaTime;
		}

		void CalculateDeltaTimeImpl()
		{
			std::lock_guard<std::mutex> lock(m_Mtx);
			float currentTime = (float)glfwGetTime();
			m_AverageDeltaTime = m_DeltaTime;
			m_DeltaTime = fabs(m_LastTime - currentTime);
			m_AverageDeltaTime = (m_AverageDeltaTime + m_DeltaTime) * 0.5f;
			m_LastTime = currentTime;
			m_Time += m_DeltaTime;
		}

	public:

		static inline float GetTime() { return Time::GetInstance().m_Time; };
		
		static inline float GetDeltaTime() { return Time::GetInstance().m_DeltaTime; };

		static inline float GetAverageDeltaTime() { return Time::GetInstance().m_AverageDeltaTime; };
		
		static inline float GetFps() { return 1.0f / Time::GetDeltaTime(); }

		static inline float GetAverageFps() { return 1.0f / Time::GetAverageDeltaTime(); }
		
		static inline void CalculateDeltaTime() { Time::GetInstance().CalculateDeltaTimeImpl(); }
	};

}