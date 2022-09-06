#pragma once

#include "../Core/Core.h"

namespace Pengine
{

	enum class PENGINE_API EventType
	{
		ONSTART,
		ONUPDATE,
		ONCLOSE,
		ONWINDOWRESIZE,
		ONMAINTHREADPROCESS,
		ONSETSCROLL
	};

}