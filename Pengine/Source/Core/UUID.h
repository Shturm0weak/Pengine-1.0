#pragma once

#include "../Core/Core.h"

#include <string>
#include <vector>

namespace Pengine
{

	class PENGINE_API UUID
	{
	private:

		static std::vector<std::string> s_UUIDs;

		std::string m_UUID;
	public:

		void Generate();

		UUID() = default;
		~UUID() = default;
		UUID(const std::string& uuid) { m_UUID = uuid; }
		UUID(const UUID& uuid) { m_UUID = uuid.m_UUID; }
		
		void operator=(std::string uuid) { m_UUID = uuid; }
		void operator=(const UUID& uuid) { m_UUID = uuid.m_UUID; }

		operator std::string() const { return m_UUID; }
		std::string Get() const { return m_UUID; }
	};

}