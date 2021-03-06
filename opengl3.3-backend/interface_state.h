#ifndef GL_3_3_BACKEND_INTERFACE_STATE_H
#define GL_3_3_BACKEND_INTERFACE_STATE_H

#include "mpsc_queue.h"
#include "allocators.h"
#include "concurrent_array.h"
#include "gl_interface.h"
#include "context.h"

#include <condition_variable>

namespace gl_3_3_backend
{
	struct state;

	struct buffer
	{
		GLuint id;
		GLenum usage;
	};
	typedef GLuint shader;
	typedef GLuint texture;
	typedef GLuint rt;
	typedef GLuint mesh;

	void executor_thread(state* st);

	struct state
	{
		gldr::allocators::standard alloc;
		gldr::detail::mpsc_queue<command, decltype(alloc)> command_queue;
		gldr::detail::concurrent_ra_array<buffer> buffers;
		gldr::detail::concurrent_ra_array<shader> shaders;
		gldr::detail::concurrent_ra_array<texture> textures;
		gldr::detail::concurrent_ra_array<mesh> meshes;
		gldr::detail::concurrent_ra_array<rt> render_targets;

		context* gl_context;
		semaphore sema;
		std::atomic_bool terminate;
		std::thread thread;

		state(platform::window* win) :
			//alloc(1 << 16),
			command_queue(&alloc),
			gl_context(create_context(*win)),
			sema(0),
			terminate(false),
			thread(executor_thread, this)
		{
			std::atomic_thread_fence(std::memory_order_release);
			sema.signal();
		}

		~state()
		{
			//Make sure that the thread wakes up
			//so it sees the terminate signal
			sema.signal(128);
			terminate.store(true);
			thread.join();
			delete_context(gl_context);
		}
	};
}

#endif
