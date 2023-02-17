#include "UUID.h"

#include <random>
#include <sstream>

#include "Logger.h"

namespace Pengine
{

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<size_t> s_UniformDistributionUUID1(0, 15);
	static std::uniform_int_distribution<size_t> s_UniformDistributionUUID2(8, 11);

	std::string UUID::Generate()
	{
		std::stringstream uuid;
		uuid << std::hex;
		
		for (size_t i = 0; i < 8; i++)
		{
			uuid << s_UniformDistributionUUID1(s_Engine);
		}

		uuid << "-";

		for (size_t i = 0; i < 4; i++)
		{
			uuid << s_UniformDistributionUUID1(s_Engine);
		}

		uuid << "-4";

		for (size_t i = 0; i < 3; i++)
		{
			uuid << s_UniformDistributionUUID1(s_Engine);
		}

		uuid << "-";
		uuid << s_UniformDistributionUUID2(s_Engine);

		for (size_t i = 0; i < 3; i++)
		{
			uuid << s_UniformDistributionUUID1(s_Engine);
		}

		uuid << "-";
		
		for (size_t i = 0; i < 12; i++)
		{
			uuid << s_UniformDistributionUUID1(s_Engine);
		}

		m_UUID = uuid.str();
		return m_UUID;
	}

}