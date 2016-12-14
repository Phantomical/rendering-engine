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

#include "gl_interface.h"
#include "interface_state.h"
#include "draw_command.h"
#include "gl_core.h"

namespace gl_3_3_backend
{
	struct render_command
	{
		void(*func)(void*);
		char data[24];

		void call()
		{
			func(data);
		}
	};

	struct renderlist
	{

	};

	//Rendering commands
	namespace rcmd
	{
		struct set_state_cmd
		{
			drawstate new_state;
		};
		void set_state(void* data)
		{
			set_state_cmd* cmd = (set_state_cmd*)data;
			//Get all the changes in the state
			drawstate changes = drawstate(global_state->drawstate._key_val ^ cmd->new_state._key_val);
			global_state->drawstate = cmd->new_state;

			if (changes.blend)
				glEnable(GL_BLEND);
			if (changes.cull_face)
				glEnable(GL_CULL_FACE);
			if (changes.debug_output)
				; //Skip
			if (changes.depth_clamp)
				glEnable(GL_DEPTH_CLAMP);

		}
	}
}

using namespace gl_3_3_backend;

extern "C" void _dispatch_commands(command_queue queue)
{
	//Get all the changes in the state
	drawstate stchanges = drawstate(global_state->drawstate._key_val ^ queue.state._key_val);
	

	for (size_t i = 0; i < queue.num_commands; ++i)
	{
		
	}
}
