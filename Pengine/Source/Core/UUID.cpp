#include "UUID.h"

#include "Utils.h"
#include "Logger.h"

#include <algorithm>
#include <random>

namespace Pengine
{

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<size_t> s_UniformDistribution;

	size_t UUID::Generate()
	{
		size_t uuid = s_UniformDistribution(s_Engine);
		if (!std::binary_search(s_UUIDs.begin(), s_UUIDs.end(), uuid))
		{
			s_UUIDs.push_back(uuid);
			std::sort(s_UUIDs.begin(), s_UUIDs.end());
			return uuid;
		}
		else
		{
			return Generate();
		}
	}

	UUID::UUID(size_t uuid)
	{
		if (uuid == -1)
		{
			m_UUID = Generate();
		}
		else if (!std::binary_search(s_UUIDs.begin(), s_UUIDs.end(), uuid))
		{
			s_UUIDs.push_back(uuid);
			std::sort(s_UUIDs.begin(), s_UUIDs.end());
			m_UUID = uuid;
		}
		else
		{
			m_UUID = Generate();
		}
	}

	void UUID::Clear()
	{
		if (m_UUID != -1)
		{
			Utils::Erase<size_t>(s_UUIDs, m_UUID);
			m_UUID = -1;
		}
	}
}