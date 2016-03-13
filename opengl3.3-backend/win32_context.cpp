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

/*
	OpenGL context initialization and other platform specific context functions
	for MS Windows.
*/

#ifdef _WIN32

#include <Windows.h>
#include "context.h"
#include "gl_core.h"
#include "wgl_core.h"

namespace opengl_3_3_backend
{
	struct context
	{
		HWND wnd;
		HDC dc;
		HGLRC ctx;
	};

	context* create_context(const gldr::platform::window& wnd)
	{
		context ctx;
		ctx.wnd = wnd.win;
		ctx.dc = GetDC(wnd.win);

		if (!ctx.dc)
			return nullptr;

		{
			HGLRC tempctx = wglCreateContext(ctx.dc);
			wgl_LoadFunctions(ctx.dc);
			wglDeleteContext(tempctx);
		}

		int flags = 0, mask = 0;

		//This extension should be supported just about everywhere
		if (wgl_ext_ARB_create_context 
			&& wgl_ext_ARB_create_context_profile)
		{
			//We don't want legacy features
			mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;

			int attribs[] =
			{
				WGL_CONTEXT_RELEASE_BEHAVIOR_ARB, WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB,
				WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
				WGL_CONTEXT_MINOR_VERSION_ARB, 3,
				WGL_CONTEXT_FLAGS_ARB, flags,
				WGL_CONTEXT_PROFILE_MASK_ARB, mask,
				0, 0
			};

			ctx.ctx = wglCreateContextAttribsARB(ctx.dc, NULL, attribs);

			int major = ogl_GetMajorVersion();
			int minor = ogl_GetMinorVersion();

			if (major < 3 || (major == 3 && minor < 3))
				//We got a context that was a older version than OpenGL 3.3
				return nullptr;
		}
		else
		{
			ctx.ctx = wglCreateContext(ctx.dc);

			int major = ogl_GetMajorVersion();
			int minor = ogl_GetMinorVersion();

			if (major < 3 || (major == 3 && minor < 3))
				//We got a context that was a older version than OpenGL 3.3
				return nullptr;
		}

		ogl_LoadFunctions();

		return new context(ctx);
	}

	void make_context_current(context* ctx)
	{
		wglMakeCurrent(ctx->dc, ctx->ctx);
	}
	void swap_context_buffers(context* ctx)
	{
		glFinish();
		SwapBuffers(ctx->dc);
	}

	void delete_context(context* ctx)
	{
		wglDeleteContext(ctx->ctx);
		ReleaseDC(ctx->wnd, ctx->dc);
		delete ctx;
	}
}

#endif
