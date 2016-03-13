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


//The amount of threads that will be signaled
//with a sync or end_frame call. There should never
//be more than 1 thread waiting on a semaphore for
//this implementation
#define SEMA_SIGNAL_COUNT 1
#define DECLARE_CMD(type) auto cmd = static_cast<commands::type*>(data)

namespace gl_3_3_backend
{
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
			//None of these are supported
			0, //GL_TESS_EVALUATION_SHADER,
			0, //GL_TESS_CONTROL_SHADER,
			0 //GL_COMPUTE_SHADER
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
		static const GLenum buffer_access_flags[] =
		{
			GL_READ_ONLY,
			GL_WRITE_ONLY,
			GL_READ_WRITE
		};
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
	void device_sync(state&, void*)
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

	void create_render_target(state& st, void* data)
	{
		//TODO: Implement
		//This isn't supported yet
		assert(false);
	}
	void delete_render_target(state& st, void* data)
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

