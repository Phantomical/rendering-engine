#pragma once

#include <cstdlib>
#include <atomic>

#include "defs.h"

namespace RE_NAMESPACE
{
	namespace allocators
	{
		//A linear allocator, note that this is not thread safe
		//If you need a thread safe allocator, use linear_atomic
		class linear
		{
		private:
			const size_t _size;
			//We could store this as a char*, but it plays hell with the debugger which
			//can lock up when trying to show it as a string
			void* const _memptr;
			size_t _pos;

		public:
			linear(size_t size) :
				//Round up to nearest multiple of 8
				_size((size + 7) & ~7),
				_memptr(std::malloc(_size)),
				_pos(0)
			{

			}
			~linear()
			{
				std::free(_memptr);
			}

			//Allocators are not moveable
			linear(linear&&) = delete;
			//Allocators are not copyable
			linear(const linear&) = delete;
			//Allocators are not copyable
			void operator =(const linear&) = delete;

			//Allocates the specified amount of memory and returns a pointer to it
			//returns null otherwise
			void* alloc(size_t size)
			{
				//Round up to nearest multiple of 8
				size_t alloc_sz = (size + 7) & ~7;
				size_t offset = _pos;
				_pos += alloc_sz;

				if (offset + alloc_sz >= _size)
				{
					_pos = _size + 1;
					return nullptr;
				}

				return static_cast<char*>(_memptr) + offset;
			}
			//Does nothing, stub for use with possible future templates
			void dealloc(void*) { }
			//Deallocates everything in the pool
			void reset()
			{
				_pos = 0;
			}
		};
		//A thread safe linear allocator, this uses atomic counters
		//but this means that you have to be careful when resetting
		//the allocator
		class linear_atomic
		{
		private:
			const size_t _size;
			void* const _memptr;
			std::atomic_size_t _pos;

		public:
			linear_atomic(size_t size) :
				//Round up to nearest multiple of 8
				_size((size + 7) & ~7),
				_memptr(std::malloc(_size)),
				_pos(0)
			{

			}
			~linear_atomic()
			{
				std::free(_memptr);
			}

			//Allocators are not moveable
			linear_atomic(linear_atomic&&) = delete;
			//Allocators are not copyable
			linear_atomic(const linear_atomic&) = delete;
			void operator =(const linear_atomic&) = delete;

			//Allocates new memory in a thread safe manner
			void* alloc(size_t size)
			{
				size_t alloc_sz = (size + 7) & ~7;
				//We can probably use std::memory_order_relaxed, but until that can
				//be determined to be safe, use sequential consistency
				size_t offset = _pos.fetch_add(alloc_sz, std::memory_order_seq_cst);

				if (offset + alloc_sz >= _size)
					//If we reset _pos like in the non-atomic version we could trample a reset
					return nullptr;

				return static_cast<char*>(_memptr) + offset;
			}
			//Does nothing, linear allocators only support deallocation through calling reset()
			void dealloc(void*) { }
			//Deallocates everything.
			//Note: This is thread safe, but if you call this and threads are still
			//      attempting to use the memory you will have pulled the data out
			//      from underneath them. This pool doesn't overwrite the memory in
			//      release builds, making this a really hard to track down bug.
			void reset()
			{
				//Simply reset the counter to the start
				_pos.store(0, std::memory_order_seq_cst);
#ifdef _DEBUG
				std::memset(_memptr, 0xFD, _size);
				std::atomic_thread_fence(std::memory_order_release);
#endif
			}
		};

		//The standard library allocator. This is just a wrapper around malloc and free.
		class standard
		{
		public:
			void* alloc(size_t size)
			{
				return std::malloc(size);
			}
			void dealloc(void* mem)
			{
				std::free(mem);
			}

			standard() = default;
			//Allocators are not moveable
			standard(standard&&) = delete;
			//Allocators are not copyable
			standard(const standard&) = delete;
			void operator =(const standard&) = delete;
		};
	}
}
