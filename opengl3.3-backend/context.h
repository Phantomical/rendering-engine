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

#ifndef OPENGL_3_3_BACKEND_CONTEXT_H
#define OPENGL_3_3_BACKEND_CONTEXT_H

#include "platform.h"

namespace gl_3_3_backend
{
	struct context;

	context* create_context(const re::platform::window& win);
	void make_context_current(context* ctx);
	void swap_context_buffers(context* ctx);
	void delete_context(context* ctx);

}

#endif
