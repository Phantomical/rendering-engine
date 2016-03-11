#include "catch.hpp"
#include "mpsc_queue.h"

TEST_CASE("test that all items can be dequeued from the queue with basic types")
{
	gldr::allocators::standard alloc;
	gldr::detail::mpsc_queue<double> queue(&alloc);

	SECTION("test that the same number items are dequeued as are enqueued")
	{
		for (size_t i = 0; i < 5; ++i)
			queue.enqueue(0.0);
		double d;
		size_t cnt = 0;
		while (queue.try_dequeue(d)) { ++cnt; }

		REQUIRE(cnt == 5);
	}

	SECTION("test that an empty queue indicates that it is empty")
	{
		REQUIRE(queue.empty());
	}
}
