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

	context* create_context(HWND wnd)
	{
		context ctx;
		ctx.wnd = wnd;
		ctx.dc = GetDC(wnd);

		if (!ctx.dc)
			return nullptr;

		{
			HGLRC tempctx = wglCreateContext(ctx.dc);
			wgl_LoadFunctions(ctx.dc);
			wglDeleteContext(tempctx);
		}

		int flags = 0, mask = 0;

		if (wgl_ext_ARB_create_context)
		{
			flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;

			mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
			mask |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;

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
		}
		else
		{
			ctx.ctx = wglCreateContext(ctx.dc);
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