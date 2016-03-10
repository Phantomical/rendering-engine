#pragma once

#include "allocators.h"
#include <atomic>

namespace gldr
{
	namespace detail
	{
#ifdef GLDR_RELACY_TEST
#define LOAD(val, order) val($).load(order)
#define STORE(val, nval, order) val($).store(nval, order)
#define EXCHANGE(val, nval, order) val($).exchange(nval, order)
#else
#define LOAD(val, order) val.load(order)
#define STORE(val, nval, order) val.store(nval, order)
#define EXCHANGE(val, nval, order) val.exchange(nval, order)
#endif

		//Multiple Producer Single Consumer queue.
		//Adapted from http://www.1024cores.net/home/lock-free-algorithms/queues/intrusive-mpsc-node-based-queue
		//Implementation from https://github.com/mstump/queues/blob/master/include/mpsc-queue.hpp
		//This implementation has been modified
		//TODO: Exaustively test this with relacy
		template<typename vTy, typename vAlloc = allocators::standard>
		class mpsc_queue
		{
		public:
			typedef vAlloc alloc_type;
#ifdef GLDR_RELACY_TEST
			typedef relacy::val<vTy> value_type;
			template<typename aTy>
			using atomic_type = relacy::atomic<aTy>;
#else
			typedef vTy value_type;
			template<typename aTy>
			using atomic_type = std::atomic<aTy>;
#endif

			mpsc_queue(alloc_type* allocator) :
				_head(alloc_stub(allocator)),
				_tail(LOAD(_head, std::memory_order_relaxed)),
				_allocator(allocator)
			{
				buffer_node_t* front = LOAD(_head, std::memory_order_relaxed);
				STORE(front->next, nullptr, std::memory_order_relaxed);
			}
			~mpsc_queue()
			{
				value_type output;
				while (try_dequeue(output)) {}
				buffer_node_t* front = LOAD(_head, std::memory_order_relaxed);
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
				STORE(node->next, nullptr, std::memory_order_relaxed);

				buffer_node_t* prev_head = EXCHANGE(_head, node, std::memory_order_acq_rel);
				STORE(prev_head->next, node, std::memory_order_release);
				return true;
			}
			//Attempts to dequeue an item. Returns false if it failed returns true and places the result in output otherwise
			bool try_dequeue(value_type& output)
			{
				buffer_node_t* tail = LOAD(_tail, std::memory_order_relaxed);
				buffer_node_t* next = LOAD(tail->next, std::memory_order_acquire);

				if (next == nullptr) {
					return false;
				}

				output = next->data;
				STORE(_tail, next, std::memory_order_release);
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
		};

#undef LOAD
#undef STORE
#undef EXCHANGE
	}
}
