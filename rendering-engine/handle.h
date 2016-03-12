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

			bool operator ==(const handle& o) const
			{
				return index == o.index && counter == o.counter;
			}
			bool operator !=(const handle& o) const
			{
				return !(*this == o);
			}

			bool valid() const
			{
				//Invalid value is UINT16_MAX - 1 therefore all implementations using handles
				//have a maximum size of 65533 elements
				return index == UINT16_MAX - 1;
			}
		};
	}
}
