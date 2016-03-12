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
	SECTION("test that a non-empty queue indicates that is isn't empty")
	{
		queue.enqueue(1.0);
		REQUIRE(!queue.empty());
	}
}
