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
#include "wgl_core.h"
#include "gl_core.h"
#include <cassert>


#ifdef _WIN32
static PROC WinGetProcAddress(const char *name);
#	define IntGetProcAddress(name) WinGetProcAddress(name)
#elif defined(__APPLE__)
static void* AppleGLGetProcAddress(const char *name);
#	define IntGetProcAddress(name) AppleGLGetProcAddress(name)
#elif defined(__sgi) || defined(__sun_)
static void* SunGetProcAddress(const GLubyte* name);
#	define IntGetProcAddress(name) SunGetProcAddress(name)
#else
#	include <GL/glx.h>
#	define IntGetProcAddress(name) (*glXGetProcAddressARB)((const GLubyte*)name)
#endif

#ifdef _WIN32

namespace opengl_3_3_backend
{
	struct context
	{
		HGLRC ctx;
		HWND win;
		HDC dc;

		context(HWND win) :
			win(win)
		{
			/*
			dc = GetDC(win);
			assert(dc != NULL);

			HGLRC tmpctx = wglCreateContext(dc);
			wglMakeCurrent(dc, tmpctx);

			wgl_LoadFunctions(dc);

			const int attribList[] =
			{
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
				WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
				WGL_COLOR_BITS_ARB, 32,
				WGL_DEPTH_BITS_ARB, 24,
				WGL_STENCIL_BITS_ARB, 8,
				0,        //End
			};

			int pixelFormat;
			UINT numFormats;

			wglChoosePixelFormatARB(dc, attribList, NULL, 1, &pixelFormat, &numFormats);

			*/
			
		}

		void make_current()
		{
			wglMakeCurrent(dc, ctx);
		}

		void swap_buffers()
		{
			glFinish();
			SwapBuffers(dc);
		}

	};
}
#else
#error "Unsupported platform"
#endif

#endif
