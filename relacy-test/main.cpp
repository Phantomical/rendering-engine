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

#define GLDR_RELACY_TEST

#ifdef _MSC_VER
#	pragma warning (disable:4311 4302 4267 4312 4290)
#endif

#include "mpsc_queue.h"

using namespace gldr::detail;
using namespace gldr::allocators;

struct queue_test : rl::test_suite<queue_test, 4>
{
	standard allocator;
	mpsc_queue<int> queue;

	queue_test() :
		queue(&allocator)
	{

	}

	void before() { }
	void after() { }

	void thread(unsigned index)
	{
		if (index == 0)
		{
			int res = 0;
			queue.try_dequeue(res);
			queue.try_dequeue(res);
		}
		else
		{
			queue.enqueue(index);
		}
	}
};

int main(int, const char*)
{
	rl::test_params params;
	//For most test we don't need to search everything
	//params.search_type = rl::sched_full;
	params.iteration_count = 100000;
	return rl::simulate<queue_test>(params) ? 0 : 1;
}
