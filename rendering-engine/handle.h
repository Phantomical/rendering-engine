#pragma once

#include <cstdint>

namespace gldr
{
	namespace detail
	{
		struct handle
		{
			uint16_t index;
			uint16_t counter;

			handle() :
				index(UINT16_MAX - 1)
			{

			}
			handle(uint16_t idx, uint16_t ctr) :
				index(idx),
				counter(ctr)
			{

			}
		};
	}
}
