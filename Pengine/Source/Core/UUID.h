#pragma once

#include "../Core/Core.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API UUID
	{
	private:

		static std::vector<size_t> s_UUIDs;

		size_t m_UUID = -1;
	public:

		static size_t Generate();

		UUID() = default;
		UUID(size_t uuid);
		UUID(const UUID&) = default;

		void Clear();

		operator size_t() const { return m_UUID; }
		void operator=(size_t uuid) { Clear(); m_UUID = uuid; }
		void operator=(const UUID& uuid) { Clear(); m_UUID = uuid.m_UUID; }
		friend class Editor;
	};

}

namespace std
{

	template<>
	struct hash<Pengine::UUID>
	{
		std::size_t operator()(const Pengine::UUID& uuid) const
		{
			return hash<size_t>()((size_t)uuid);
		}
	};

}