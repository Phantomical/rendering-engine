#pragma once

#include "allocators.h"

#ifdef GLDR_RELACY_TEST
#include <relacy/relacy.hpp>
#else
#include <atomic>
#endif

namespace gldr
{
	namespace detail
	{
#ifdef GLDR_RELACY_TEST
#	undef memory_order_relaxed
#	undef memory_order_acq_rel
#	undef memory_order_acquire
#	undef memory_order_release
#	define memory_order_relaxed mo_relaxed
#	define memory_order_acq_rel mo_acq_rel
#	define memory_order_acquire mo_acquire
#	define memory_order_release mo_release
#	define LOAD(val, order) val($).load(rl::order)
#	define STORE(val, nval, order) val($).store(nval, rl::order)
#	define EXCHANGE(val, nval, order) val($).exchange(nval, rl::order)
#else
#	define LOAD(val, order) val.load(std::order)
#	define STORE(val, nval, order) val.store(nval, std::order)
#	define EXCHANGE(val, nval, order) val.exchange(nval, std::order)
#endif

		//Multiple Producer Single Consumer queue.
		//Original implementation from https://github.com/mstump/queues/blob/master/include/mpsc-queue.hpp
		//Original License:
		// This is free and unencumbered software released into the public domain.

		// Anyone is free to copy, modify, publish, use, compile, sell, or
		// distribute this software, either in source code form or as a compiled
		// binary, for any purpose, commercial or non-commercial, and by any
		// means.

		// In jurisdictions that recognize copyright laws, the author or authors
		// of this software dedicate any and all copyright interest in the
		// software to the public domain. We make this dedication for the benefit
		// of the public at large and to the detriment of our heirs and
		// successors. We intend this dedication to be an overt act of
		// relinquishment in perpetuity of all present and future rights to this
		// software under copyright law.

		// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
		// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
		// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
		// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
		// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
		// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
		// OTHER DEALINGS IN THE SOFTWARE.

		// For more information, please refer to <http://unlicense.org/>

		//This implementation has been modified
		//TODO: Exaustively test this with relacy
		//typedef int vTy;
		//typedef allocators::standard vAlloc;
		template<typename vTy, typename vAlloc = allocators::standard>
		class mpsc_queue
		{
		public:
			typedef vAlloc alloc_type;
			typedef vTy value_type;
#ifdef GLDR_RELACY_TEST
			template<typename aTy>
			using atomic_type = rl::atomic<aTy>;
#else
			template<typename aTy>
			using atomic_type = std::atomic<aTy>;
#endif

			mpsc_queue(alloc_type* allocator) :
				_head(alloc_stub(allocator)),
				_tail(LOAD(_head, memory_order_relaxed)),
				_allocator(allocator)
			{
				buffer_node_t* front = LOAD(_head, memory_order_relaxed);
				STORE(front->next, nullptr, memory_order_relaxed);
			}
			~mpsc_queue()
			{
				buffer_node_t* node = LOAD(_tail, memory_order_relaxed);
				if (node)
				{
					do
					{
						auto prev = node;
						node = LOAD(node->next, memory_order_relaxed);
						prev->~buffer_node_t();
						_allocator->dealloc(prev);
					} while (node);
				}
			}

			//Attempts to enqueue an item. Returns false if memory for the item could not be allocated, true otherwise
			bool enqueue(const value_type& input)
			{

				void* mem = _allocator->alloc(sizeof(buffer_node_t));
				if (!mem)
					return false;
				buffer_node_t* node = new(mem) buffer_node_t;
				node->data = input;
				STORE(node->next, nullptr, memory_order_relaxed);

				buffer_node_t* prev_head = EXCHANGE(_head, node, memory_order_acq_rel);
				STORE(prev_head->next, node, memory_order_release);
				return true;
			}
			//Attempts to dequeue an item. Returns false if it failed returns true and places the result in output otherwise
			bool try_dequeue(value_type& output)
			{
				buffer_node_t* tail = LOAD(_tail, memory_order_relaxed);
				buffer_node_t* next = LOAD(tail->next, memory_order_acquire);

				if (next == nullptr) {
					return false;
				}

				output = next->data;
				STORE(_tail, next, memory_order_release);
				tail->~buffer_node_t();
				_allocator->dealloc(tail);
				return true;
			}

			bool empty() const
			{
				buffer_node_t* tail = LOAD(_tail, memory_order_relaxed);
				//If the tail is NULL the queue is empty
				return tail == nullptr;
			}

		private:
			struct buffer_node_t
			{
				value_type                  data;
				atomic_type<buffer_node_t*> next;
			};

			//Utility function for allocating a stub node
			static buffer_node_t* alloc_stub(alloc_type* alloc)
			{
				void* mem = alloc->alloc(sizeof(buffer_node_t));
				return new(mem) buffer_node_t;
			}

			atomic_type<buffer_node_t*> _head;
			atomic_type<buffer_node_t*> _tail;
			alloc_type* _allocator;

#undef delete
			mpsc_queue(mpsc_queue&&) = delete;
			mpsc_queue(const mpsc_queue&) = delete;
			void operator=(const mpsc_queue&) = delete;
		};

#undef LOAD
#undef STORE
#undef EXCHANGE
	}
}
