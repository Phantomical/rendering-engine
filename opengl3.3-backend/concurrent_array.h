//Copyright (C) 2016 Sean Lynch
/*
	This file is part of rendering-engine.
	
	rendering-engine is free software : you can redistribute it and / or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	rendering-engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with rendering-engine. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GLDR_PACKED_ARRAY_H
#define GLDR_PACKED_ARRAY_H

#include <stack>
#include <cassert>
#include <mutex>

#include "handle.h"

#ifdef _MSC_VER
//Disable spurious warning in remove_value
#	pragma warning(disable:4100)
#endif

namespace gl_3_3_backend
{
	namespace detail
	{
		template<typename vTy, size_t vSz = (1 << 14)>
		//An array were reserving a key can be done concurrently but 
		//putting in values must be done on a single thread
		//This container only permits random access
		class concurrent_ra_array
		{
		public:
			typedef vTy value_type;
			typedef uint16_t size_type;

			typedef value_type* pointer;
			typedef value_type& reference;
			typedef const value_type* const_pointer;
			typedef const value_type& const_reference;

			static constexpr size_t size_value = vSz;
			static constexpr size_type prealloc_value = std::numeric_limits<size_type>::max();
			static constexpr size_type invalid_value = prealloc_value - 1;

			static_assert(size_value < invalid_value, "The array cannot be bigger than 65534 values");

		private:
			struct index_type
			{
				union
				{
					size_type index;
					size_type next;
				};
				size_type generation;

				operator handle()
				{
					return handle{ index, generation };
				}
			};

			value_type _values[size_value];
			index_type _indices[size_value];

			size_type _num_values;
			size_type _free_start;
			std::mutex _mutex;

		public:
			concurrent_ra_array() :
				_num_values(0),
				_free_start(0)
			{
				std::memset(_values, 0, sizeof(value_type) * size_value);

				for (size_type i = 0; i < size_value; ++i)
				{
					_indices[i].generation = 0;
					_indices[i].next = i + 1;
				}
			}

			//Allocates an index slot but doesn't back it with a value
			handle alloc()
			{
				//Make sure that noone can touch the indices
				std::unique_lock<std::mutex> lock(_mutex);

				//Get the first index slot
				index_type& index = _indices[_free_start];

				//Update the head of the free list
				_free_start = index.next;

				//Update the generation
				++index.generation;

				index.index = prealloc_value;

				return static_cast<handle>(index);
			}
			//Removes an index that doesn't have a value backing it
			void dealloc(const handle& h)
			{
				//Protect the indices
				std::unique_lock<std::mutex> lock(_mutex);

				//An invalid handle was passed
				assert(h.index < size_value);

				index_type& index = _indices[h.index];

				if (index.generation != h.counter)
					//The handle has already been removed
					return;

				index.next = _free_start;
				_free_start = h.index;
			}
			//Backs an index with a value
			void attach_value(const handle& h, const_reference val)
			{
				new(&_values[h.index]) value_type(val);
				++_num_values;
			}
#ifdef _MSC_VER
			//Disable spurious warning in remove_value
#	pragma warning(suppress:4100)
#endif
			void remove_value(const handle& h)
			{
				_values[h.index].~value_type();
				--_num_values;
			}

			handle insert(const_reference val)
			{
				//Get a slot (this is thread safe)
				handle h = alloc();
				//Back it with a value (not thread safe)
				attach_value(h, val);
				return h;
			}
			void remove(const handle& h)
			{
				remove_value(h);
				dealloc(h);
			}

			//Doing accesses requires acquiring a mutex
			//For anything else, access through iterators
			reference at(const handle& h)
			{
				return _values[h.index];
			}

			size_type size() const
			{
				return _num_values;
			}
			size_type reserved() const
			{
				return size_value;
			}
		};
	}
}

#endif //GLDR_PACKED_ARRAY_H
