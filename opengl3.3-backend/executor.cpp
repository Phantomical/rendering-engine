//Copyright (c) 2016 Sean Lynch
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

#include "gl_core.h"
#include "context.h"
#include "gl_interface.h"
#include "interface_state.h"

using namespace gldr;

namespace gl_3_3_backend
{
	void executor_thread(state* st)
	{
		make_context_current(st->gl_context);

		command cmd;
		st->sema.wait();
		std::atomic_thread_fence(std::memory_order_acquire);

		while (!st->terminate.load(std::memory_order_relaxed))
		{
			st->sema.wait();

			while (st->command_queue.try_dequeue(cmd))
			{
				//Even though the pointer points to an array of size 1
				//this is fine since the pointer aliases all the command
				//types. None of the command types should access memory 
				//that is out of bounds.
				cmd.func(*st, cmd.ptr);
			}
		}

		while (st->command_queue.try_dequeue(cmd))
		{
			//See comment in main loop
			cmd.func(*st, cmd.ptr);
		}
	}
}
