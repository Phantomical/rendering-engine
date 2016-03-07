#pragma once

#include "allocators.h"

namespace gldr
{
	namespace detail
	{
		//Multiple Producer Single Consumer queue.
		//Adapted from http://www.1024cores.net/home/lock-free-algorithms/queues/intrusive-mpsc-node-based-queue
		//Implementation from https://github.com/mstump/queues/blob/master/include/mpsc-queue.hpp
		//TODO: Exaustively test this with relacy
		template<typename vTy, typename vAlloc = allocators::standard>
		class mpsc_queue
		{
		public:
			typedef vTy value_type;
			typedef vAlloc alloc_type;

			mpsc_queue(alloc_type* allocator) :
				_head(alloc_stub(allocator)),
				_tail(_head.load(std::memory_order_relaxed)),
				_allocator(allocator)
			{
				buffer_node_t* front = _head.load(std::memory_order_relaxed);
				front->next.store(nullptr, std::memory_order_relaxed);
			}
			~mpsc_queue()
			{
				value_type output;
				while (try_dequeue(output)) {}
				buffer_node_t* front = _head.load(std::memory_order_relaxed);
				delete front;
			}

			//Attempts to enqueue an item. Returns false if memory for the item could not be allocated, true otherwise
			bool enqueue(const value_type& input)
			{
				void* mem = _allocator->alloc(sizeof(buffer_node_t));
				if (!mem)
					return false;
				buffer_node_t* node = new(mem) buffer_node_t;
				node->data = input;
				node->next.store(nullptr, std::memory_order_relaxed);

				buffer_node_t* prev_head = _head.exchange(node, std::memory_order_acq_rel);
				prev_head->next.store(node, std::memory_order_release);
				return true;
			}
			//Attempts to dequeue an item. Returns false if it failed returns true and places the result in output otherwise
			bool try_dequeue(value_type& output)
			{
				buffer_node_t* tail = _tail.load(std::memory_order_relaxed);
				buffer_node_t* next = tail->next.load(std::memory_order_acquire);

				if (next == nullptr) {
					return false;
				}

				output = next->data;
				_tail.store(next, std::memory_order_release);
				tail->~buffer_node_t();
				_allocator->dealloc(tail);
				return true;
			}

			mpsc_queue(mpsc_queue&&) = delete;
			mpsc_queue(const mpsc_queue&) = delete;
			void operator=(const mpsc_queue&) = delete;

		private:
			struct buffer_node_t
			{
				value_type                  data;
				std::atomic<buffer_node_t*> next;
			};

			//Utility function for allocating a stub node
			static buffer_node_t* alloc_stub(alloc_type* alloc)
			{
				void* mem = alloc->alloc(sizeof(buffer_node_t));
				return new(mem) buffer_node_t;
			}

			std::atomic<buffer_node_t*> _head;
			std::atomic<buffer_node_t*> _tail;
			alloc_type* _allocator;
		};
	}
}
