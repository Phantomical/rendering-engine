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

#include "platform.h"

#include "gl_core.h"
#include "gl_interface.h"
#include "interface_state.h"

#include <thread>

#define UNUSED(arg)

//The amount of threads that will be signaled
//with a sync or end_frame call. There should never
//be more than 1 thread waiting on a semaphore for
//this implementation
#define SEMA_SIGNAL_COUNT 1
#define DECLARE_CMD(type) auto cmd = static_cast<commands::type*>(data)

namespace gl_3_3_backend
{
	template<typename vTy>
	constexpr vTy max(vTy a, vTy b)
	{
		return a < b ? b : a;
	}
	template<typename vTy, typename... _Args>
	constexpr vTy max(vTy a, vTy b, _Args... _rest)
	{
		return max<vTy>(max<vTy>(a, b), _rest...);
	}

	namespace enums
	{
		//Allocated memory for all of the enums
		//Make sure that there aren't more enums than this
		static GLenum enum_mem[128];

		static GLenum* const internal_formats = enum_mem;
		static GLenum* const formats = internal_formats + (max(R8, RG8, RGB8, R32F, RG32F, RGB32F, RGBA32F) + 1);
		static GLenum* const data_types = formats + (max(RED, GREEN, BLUE, RG, RGB, RGBA, BGR, BGRA, DEPTH_COMPONENT, STENCIL_INDEX, DEPTH_STENCIL) + 1);
		static GLenum* const shader_stages = data_types + (max(UNSIGNED_BYTE, UNSIGNED_SHORT, UNSIGNED_INT, BYTE, SHORT, INT, FLOAT) + 1);
		static GLenum* const buffer_usages = shader_stages + (max(STREAM_DRAW, STREAM_READ, STREAM_COPY, STATIC_DRAW, STATIC_READ, STATIC_COPY, DYNAMIC_DRAW, DYNAMIC_READ, DYNAMIC_COPY) + 1);
		static GLenum* const buffer_access_flags = buffer_usages + (max(STREAM_DRAW, STREAM_READ, STREAM_COPY, STATIC_DRAW, STATIC_READ, STATIC_COPY, DYNAMIC_DRAW, DYNAMIC_READ, DYNAMIC_COPY) + 1);

		void init_enums()
		{
			internal_formats[R8] = GL_R8;
			internal_formats[RG8] = GL_RG8;
			internal_formats[RGB8] = GL_RGB8;
			internal_formats[RGBA8] = GL_RGBA8;
			internal_formats[R32F] = GL_R32F;
			internal_formats[RG32F] = GL_RG32F;
			internal_formats[RGB32F] = GL_RGB32F;
			internal_formats[RGBA32F] = GL_RGBA32F;

			formats[RED] = GL_RED;
			formats[GREEN] = GL_GREEN;
			formats[BLUE] = GL_BLUE;
			formats[RG] = GL_RG;
			formats[RGB] = GL_RGB;
			formats[RGBA] = GL_RGBA;
			formats[BGR] = GL_BGR;
			formats[BGRA] = GL_BGRA;
			formats[DEPTH_COMPONENT] = GL_DEPTH_COMPONENT;
			formats[STENCIL_INDEX] = GL_STENCIL_INDEX;
			formats[DEPTH_STENCIL] = GL_DEPTH_STENCIL;
			
			data_types[UNSIGNED_BYTE] = GL_UNSIGNED_BYTE;
			data_types[UNSIGNED_SHORT] = GL_UNSIGNED_SHORT;
			data_types[UNSIGNED_INT] = GL_UNSIGNED_INT;
			data_types[BYTE] = BYTE;
			data_types[SHORT] = GL_SHORT;
			data_types[INT] = GL_INT;
			data_types[FLOAT] = GL_FLOAT;
			
			shader_stages[VERTEX] = GL_VERTEX_SHADER;
			shader_stages[FRAGMENT] = GL_FRAGMENT_SHADER;
			shader_stages[GEOMETRY] = GL_GEOMETRY_SHADER;
			//None of these are supported
			shader_stages[TESS_EVALUATION] = 0;
			shader_stages[TESS_CONTROL] = 0;
			shader_stages[COMPUTE] = 0;

			buffer_usages[STREAM_DRAW] = GL_STREAM_DRAW;
			buffer_usages[STREAM_READ] = GL_STREAM_READ;
			buffer_usages[STREAM_COPY] = GL_STREAM_COPY;
			buffer_usages[STATIC_DRAW] = GL_STATIC_DRAW;
			buffer_usages[STATIC_READ] = GL_STATIC_READ;
			buffer_usages[STATIC_COPY] = GL_STATIC_COPY;
			buffer_usages[DYNAMIC_DRAW] = GL_DYNAMIC_DRAW;
			buffer_usages[DYNAMIC_READ] = GL_DYNAMIC_READ;
			buffer_usages[DYNAMIC_COPY] = GL_DYNAMIC_COPY;

			buffer_access_flags[READ] = GL_READ_ONLY;
			buffer_access_flags[WRITE] = GL_WRITE_ONLY;
			buffer_access_flags[READ_WRITE] = GL_READ_WRITE;
		}
	}

	state* global_state = nullptr;

	void enqueue(const command& cmd)
	{
		global_state->command_queue.enqueue(cmd);
		global_state->sema.signal();
	}

	detail::handle alloc_mesh_handle()
	{
		return global_state->meshes.alloc();
	}
	detail::handle alloc_buffer_handle()
	{
		return global_state->buffers.alloc();
	}
	detail::handle alloc_shader_handle()
	{
		return global_state->shaders.alloc();
	}
	detail::handle alloc_texture_handle()
	{
		return global_state->textures.alloc();
	}
	detail::handle alloc_render_target_handle()
	{
		return global_state->render_targets.alloc();
	}

	void sync(state&, void* data)
	{
		typedef commands::sync cmd;
		cmd* sync = static_cast<cmd*>(data);

		sync->sema->signal(SEMA_SIGNAL_COUNT);
	}
	void swap_buffers(state& st, void* data)
	{
		typedef commands::swap_buffers cmd;
		cmd* sync = static_cast<cmd*>(data);

		sync->sema->signal(SEMA_SIGNAL_COUNT);
		swap_context_buffers(st.gl_context);
	}
	void device_sync(state& UNUSED(st), void* UNUSED(data))
	{
		glFinish();
	}

	void create_mesh(state& st, void* data)
	{
		DECLARE_CMD(create_mesh);

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		for (size_t i = 0; i < cmd->num_buffers; ++i)
		{
			auto buf = cmd->buffers[i];
			buffer b = st.buffers.at(buf.handle);
			glBindBuffer(GL_ARRAY_BUFFER, b.id);

			for (size_t j = 0; j < cmd->num_layouts; ++j)
			{
				if (cmd->layouts[j].buffer == buf)
				{
					glVertexAttribPointer(
						cmd->layouts[j].index,
						cmd->layouts[j].size,
						enums::data_types[cmd->layouts[j].type],
						cmd->layouts[i].normalized,
						static_cast<GLsizei>(cmd->layouts[j].stride),
						(void*)cmd->layouts[j].offset);
				}
			}
		}

		auto buf = cmd->elements;
		if (buf.handle.valid())
		{
			buffer b = st.buffers.at(buf.handle);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b.id);
		}
	}
	void create_buffer(state& st, void* data)
	{
		DECLARE_CMD(create_buffer);
		buffer buf;
		buf.usage = enums::buffer_usages[(uint8_t)cmd->usage];

		glGenBuffers(1, &buf.id);
		glBindBuffer(GL_ARRAY_BUFFER, buf.id);
		glBufferData(GL_ARRAY_BUFFER, cmd->size, cmd->data, buf.usage);

		st.buffers.attach_value(cmd->_handle, buf);
	}
	void create_shader(state& st, void* data)
	{
		DECLARE_CMD(create_shader);

		//We don't want to allocate this on the heap so we use alloca for speed
		GLuint* stages = static_cast<GLuint*>(alloca(sizeof(GLuint) * cmd->num_stages));

		shader program = glCreateProgram();
		for (size_t i = 0; i < cmd->num_stages; ++i)
		{
			const char* str = cmd->stages[i].second;
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

		st.shaders.attach_value(cmd->_handle, program);
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
		st.textures.attach_value(cmd->_handle, id);
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
		st.textures.attach_value(cmd->_handle, id);
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

		st.textures.attach_value(cmd->_handle, id);
	}

	void delete_buffer(state& st, void* data)
	{
		DECLARE_CMD(delete_buffer);

		GLuint id = st.buffers.at(cmd->buffer.handle).id;
		glDeleteBuffers(1, &id);
		st.buffers.remove(cmd->buffer.handle);
	}
	void delete_shader(state& st, void* data)
	{
		DECLARE_CMD(delete_shader);

		GLuint id = st.shaders.at(cmd->shader.handle);
		glDeleteProgram(id);
		st.shaders.remove(cmd->shader.handle);
	}
	void delete_texture(state& st, void* data)
	{
		DECLARE_CMD(delete_texture);

		GLuint id = st.buffers.at(cmd->image.handle).id;
		glDeleteBuffers(1, &id);
		st.buffers.remove(cmd->image.handle);
	}
	void delete_mesh(state& st, void* data)
	{
		DECLARE_CMD(delete_mesh);

		mesh m = st.meshes.at(cmd->mesh.handle);
		glDeleteVertexArrays(1, &m);
	}

	void set_buffer_data(state& st, void* data)
	{
		DECLARE_CMD(set_buffer_data);

		buffer buf = st.buffers.at(cmd->buffer.handle);
		glBindBuffer(GL_ARRAY_BUFFER, buf.id);
		glBufferData(GL_ARRAY_BUFFER, cmd->size, cmd->data, buf.usage);
	}

	void create_render_target(state& UNUSED(st), void* UNUSED(data))
	{
		//TODO: Implement
		//This isn't supported yet
		assert(false);
	}
	void delete_render_target(state& UNUSED(st), void* UNUSED(data))
	{
		//TODO: Implement
		//This isn't supported yet
		assert(false);
	}
}

using namespace gl_3_3_backend;

extern "C" void _init(platform::window* win)
{
	global_state = new state(win);
}
extern "C" void _terminate()
{
	delete global_state;
}

extern "C" void _dispatch_commands(command_queue queue)
{

}

