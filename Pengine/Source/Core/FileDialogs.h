#pragma once

#include "Core.h"

#include <optional>

namespace Pengine {

	class PENGINE_API FileDialogs {
	public:

		static std::optional<std::string> OpenFile(const char* filter);
		static std::optional<std::string> SaveFile(const char* filter);
	};

}

