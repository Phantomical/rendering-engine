
#include "interface.h"
#include "concurrent_array.h"
#include "mpsc_queue.h"
#include "allocators.h"

#include "gl_core.h"
#include "sema.h"

#include <thread>

typedef DefaultSemaphoreType semaphore;

//The amount of threads that will be signaled
//with a sync or end_frame call. There should never
//be more than 1 thread waiting on a semaphore for
//this implementation
#define SEMA_SIGNAL_COUNT 1
#define DECLARE_CMD(type) commands::type* cmd = static_cast<commands::type*>(data)

namespace gldr
{
	struct state;

	typedef void(*command_func)(state& state, void* data);

	namespace enums
	{
		static constexpr GLenum internal_formats[] =
		{
			GL_R8,
			GL_RG8,
			GL_RGB8,
			GL_RGBA8,
			GL_R32F,
			GL_RG32F,
			GL_RGB32F,
			GL_RGBA32F
		};
		static constexpr GLenum formats[] =
		{
			GL_RED,
			GL_GREEN,
			GL_BLUE,
			GL_RG,
			GL_RGB,
			GL_RGBA,
			GL_BGR,
			GL_BGRA,
			GL_DEPTH_COMPONENT,
			GL_STENCIL_INDEX,
			GL_DEPTH_STENCIL
		};
		static constexpr GLenum data_types[] =
		{
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_INT,
			GL_BYTE,
			GL_SHORT,
			GL_INT,
			GL_FLOAT
		};
		static constexpr GLenum shader_stages[] =
		{
			GL_VERTEX_SHADER,
			GL_FRAGMENT_SHADER,
			GL_GEOMETRY_SHADER,
			GL_TESS_EVALUATION_SHADER,
			GL_TESS_CONTROL_SHADER,
			GL_COMPUTE_SHADER
		};
		static constexpr GLenum buffer_usages[] =
		{
			GL_STREAM_DRAW,
			GL_STREAM_READ,
			GL_STREAM_COPY,
			GL_STATIC_DRAW,
			GL_STATIC_READ,
			GL_STATIC_COPY,
			GL_DYNAMIC_DRAW,
			GL_DYNAMIC_READ,
			GL_DYNAMIC_COPY
		};
	}
	namespace commands
	{
		struct sync
		{
			semaphore* sema;
		};
		struct end_frame
		{
			semaphore* sema;
		};
		struct create_buffer
		{
			void* data;
			size_t size;
			detail::handle handle;
			buffer_usage usage;
		};
		struct create_shader
		{
			std::pair<shader_stage, std::string>* stages;
			detail::handle handle;
			//There is no reason for any shader to have more than 2^16 shaders
			//There is probably no reason for more than 2^8 but the space isn't
			//needed in the message so it is ok.
			uint16_t num_stages;
		};
		struct create_texture_2d
		{
			void* data;
			detail::handle handle;
			//OpenGL 4.1 Mandates a maximum texture size of 16384
			//so we don't need to use size_t for texture sizes
			//NOTE: Having values of this size also means that a command is 32 bytes
			uint16_t width;
			uint16_t height;
			internal_format iformat;
			image_format format;
			data_type type;
		};
		struct create_texture_3d
		{
			void* data;
			detail::handle handle;
			//For why these are uint16_t see create_texture_2d
			uint16_t width;
			uint16_t height;
			uint16_t depth;
			internal_format iformat;
			image_format format;
			data_type type;
		};
		struct create_texture_cubemap
		{
			const void** data;
			detail::handle handle;
			//For why these are uint16_t see create_texture_2d
			uint16_t width;
			uint16_t height;
			internal_format iformat;
			image_format format;
			data_type type;
		};
		struct delete_buffer
		{
			detail::handle handle;
		};
		struct delete_shader
		{
			detail::handle handle;
		};
		struct delete_texture
		{
			detail::handle handle;
		};

	}

	struct command
	{
		command_func func;

		union
		{
			//Pointer to the data
			//This will be passed into the function with this command
			char ptr[1];

			commands::sync sync;
			commands::end_frame end_frame;
			commands::create_buffer create_buffer;
			commands::create_shader create_shader;
			commands::create_texture_2d create_texture_2d;
			commands::create_texture_3d create_texture_3d;
			commands::create_texture_cubemap create_texture_cubemap;
			commands::delete_buffer delete_buffer;
			commands::delete_shader delete_shader;
			commands::delete_texture delete_texture;
		};
	};

	constexpr size_t cmdsz = sizeof(command);

	typedef GLuint buffer;
	typedef GLuint shader;
	typedef GLuint texture;

	struct state
	{
		allocators::linear_atomic alloc;
		detail::mpsc_queue<command, decltype(alloc)> command_queue;

		detail::concurrent_ra_array<buffer> buffers;
		detail::concurrent_ra_array<shader> shaders;
		detail::concurrent_ra_array<texture> textures;

		std::thread thread;
	};

	void sync_func(state&, void* data)
	{
		typedef commands::sync cmd;
		cmd* sync = static_cast<cmd*>(data);

		sync->sema->signal(SEMA_SIGNAL_COUNT);
	}
	void end_frame_func(state&, void* data)
	{
		typedef commands::end_frame cmd;
		cmd* sync = static_cast<cmd*>(data);

		sync->sema->signal(SEMA_SIGNAL_COUNT);
	}

	void create_buffer(state& st, void* data)
	{
		DECLARE_CMD(create_buffer);
		buffer buf;

		glGenBuffers(1, &buf);
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, cmd->size, cmd->data, enums::buffer_usages[(uint8_t)cmd->usage]);

		st.buffers.attach_value(cmd->handle, buf);
	}
	void create_shader(state& st, void* data)
	{
		DECLARE_CMD(create_shader);

		//We don't want to allocate this on the heap so we use alloca for speed
		GLuint* stages = static_cast<GLuint*>(alloca(sizeof(GLuint) * cmd->num_stages));

		shader program = glCreateProgram();
		for (size_t i = 0; i < cmd->num_stages; ++i)
		{
			const char* str = cmd->stages[i].second.c_str();
			stages[i] = glCreateShader(enums::shader_stages[(uint8_t)cmd->stages[i].first]);
			glShaderSource(stages[i], 1, &str, nullptr);
			glCompileShader(stages[i]);
		}

		for (size_t i = 0; i < cmd->num_stages; ++i)
		{
			glAttachShader(program, stages[i]);
		}

		glLinkProgram(program);

		for (size_t i = 0; i < cmd->num_stages; ++i)
		{
			glDetachShader(program, stages[i]);
			glDeleteShader(stages[i]);
		}

		st.shaders.attach_value(cmd->handle, program);
	}
	void create_texture_2d(state& st, void* data)
	{
		DECLARE_CMD(create_texture_2d);
		texture id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, enums::internal_formats[(uint8_t)cmd->iformat],
			cmd->width, cmd->height, 0, enums::formats[(uint8_t)cmd->format],
			enums::data_types[(uint8_t)cmd->type], cmd->data);
		st.textures.attach_value(cmd->handle, id);
	}
	void create_texture_3d(state& st, void* data)
	{
		DECLARE_CMD(create_texture_3d);
		texture id;
		glGenBuffers(1, &id);
		glBindTexture(GL_TEXTURE_3D, id);
		glTexImage3D(GL_TEXTURE_3D, 0, enums::internal_formats[(uint8_t)cmd->iformat],
			cmd->width, cmd->height, cmd->depth, 0, enums::formats[(uint8_t)cmd->format],
			enums::data_types[(uint8_t)cmd->type], cmd->data);
		st.textures.attach_value(cmd->handle, id);
	}
	void create_texture_cubemap(state& st, void* data)
	{
		DECLARE_CMD(create_texture_cubemap);
		texture id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0,
			enums::internal_formats[(uint8_t)cmd->iformat], cmd->width, cmd->height, 0,
			enums::formats[(uint8_t)cmd->format], enums::data_types[(uint8_t)cmd->type],
			cmd->data[0]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0,
			enums::internal_formats[(uint8_t)cmd->iformat], cmd->width, cmd->height, 0,
			enums::formats[(uint8_t)cmd->format], enums::data_types[(uint8_t)cmd->type],
			cmd->data[1]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0,
			enums::internal_formats[(uint8_t)cmd->iformat], cmd->width, cmd->height, 0,
			enums::formats[(uint8_t)cmd->format], enums::data_types[(uint8_t)cmd->type],
			cmd->data[2]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0,
			enums::internal_formats[(uint8_t)cmd->iformat], cmd->width, cmd->height, 0,
			enums::formats[(uint8_t)cmd->format], enums::data_types[(uint8_t)cmd->type],
			cmd->data[3]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
			enums::internal_formats[(uint8_t)cmd->iformat], cmd->width, cmd->height, 0,
			enums::formats[(uint8_t)cmd->format], enums::data_types[(uint8_t)cmd->type],
			cmd->data[4]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0,
			enums::internal_formats[(uint8_t)cmd->iformat], cmd->width, cmd->height, 0,
			enums::formats[(uint8_t)cmd->format], enums::data_types[(uint8_t)cmd->type],
			cmd->data[5]);

		st.textures.attach_value(cmd->handle, id);
	}



}
