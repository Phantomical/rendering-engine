#ifndef OPENGL_3_3_BACKEND_IMPLEMENTATION_H
#define OPENGL_3_3_BACKEND_IMPLEMENTATION_H

#include "interface.h"
#include "sema.h"

#define CALL_CONV GLDR_BACKEND_CALL_CONV
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

typedef DefaultSemaphoreType semaphore;

using namespace gldr;
namespace gl_3_3_backend
{
	struct state;
	typedef void(*command_func)(state& state, void* data);
	namespace enums
	{
		enum shader_stage : uint8_t
		{
			VERTEX,
			FRAGMENT,
			GEOMETRY,
			TESS_EVALUATION,
			TESS_CONTROL,
			COMPUTE,
		};
		enum buffer_access_flags : uint8_t
		{
			READ = 0x1,
			WRITE = 0x2,
			READ_WRITE = READ | WRITE,
		};
		enum buffer_usage : uint8_t
		{
			STREAM_DRAW,
			STREAM_READ,
			STREAM_COPY,
			STATIC_DRAW,
			STATIC_READ,
			STATIC_COPY,
			DYNAMIC_DRAW,
			DYNAMIC_READ,
			DYNAMIC_COPY,
		};
		enum internal_format : uint8_t
		{
			R8,
			RG8,
			RGB8,
			RGBA8,
			R32F,
			RG32F,
			RGB32F,
			RGBA32F,
		};
		enum image_format : uint8_t
		{
			RED,
			GREEN,
			BLUE,
			RG,
			RGB,
			RGBA,
			BGR,
			BGRA,
			DEPTH_COMPONENT,
			STENCIL_INDEX,
			DEPTH_STENCIL,
		};
		enum data_type : uint8_t
		{
			UNSIGNED_BYTE,
			UNSIGNED_SHORT,
			UNSIGNED_INT,
			BYTE,
			SHORT,
			INT,
			FLOAT,
		};
	}
	namespace commands
	{
		struct create_buffer
		{
			size_t size;
			const void* data;
			buffer_usage usage;
			detail::handle _handle;
		};
		struct create_shader
		{
			size_t num_stages;
			const std::pair<shader_stage, const char*>* stages;
			detail::handle _handle;
		};
		struct create_texture_2d
		{
			uint16_t width;
			uint16_t height;
			internal_format iformat;
			image_format format;
			data_type type;
			const void* data;
			detail::handle _handle;
		};
		struct create_texture_3d
		{
			uint16_t width;
			uint16_t height;
			uint16_t depth;
			internal_format iformat;
			image_format format;
			data_type type;
			const void* data;
			detail::handle _handle;
		};
		struct create_texture_cubemap
		{
			uint16_t width;
			uint16_t height;
			internal_format iformat;
			image_format format;
			data_type type;
			const void* const* data;
			detail::handle _handle;
		};
		struct create_render_target
		{
			detail::handle _handle;
		};
		struct create_mesh
		{
			size_t num_buffers;
			std::pair<unsigned, buffer_handle>* handles;
			size_t num_layouts;
			buffer_layout* layouts;
			detail::handle _handle;
		};
		struct delete_buffer
		{
			buffer_handle buffer;
		};
		struct delete_shader
		{
			shader_handle shader;
		};
		struct delete_texture
		{
			texture_handle image;
		};
		struct delete_render_target
		{
			render_target_handle render_target;
		};
		struct delete_mesh
		{
			mesh_handle mesh;
		};
		struct set_buffer_data
		{
			buffer_handle buffer;
			size_t size;
			const void* data;
		};
		struct swap_buffers
		{
			semaphore* sema;
		};
		struct sync
		{
			semaphore* sema;
		};
	}
	struct command
	{
		command_func func;
		union
		{
			char ptr[1];
			
			commands::create_buffer create_buffer;
			commands::create_shader create_shader;
			commands::create_texture_2d create_texture_2d;
			commands::create_texture_3d create_texture_3d;
			commands::create_texture_cubemap create_texture_cubemap;
			commands::create_render_target create_render_target;
			commands::create_mesh create_mesh;
			commands::delete_buffer delete_buffer;
			commands::delete_shader delete_shader;
			commands::delete_texture delete_texture;
			commands::delete_render_target delete_render_target;
			commands::delete_mesh delete_mesh;
			commands::set_buffer_data set_buffer_data;
			commands::swap_buffers swap_buffers;
			commands::sync sync;
		};
		command() { }
		command(command_func func) : func(func) { }
	};
	detail::handle alloc_mesh_handle();
	detail::handle alloc_buffer_handle();
	detail::handle alloc_shader_handle();
	detail::handle alloc_texture_handle();
	detail::handle alloc_render_target_handle();
	void enqueue(const command&);
	void create_buffer(state&, void*);
	void create_shader(state&, void*);
	void create_texture_2d(state&, void*);
	void create_texture_3d(state&, void*);
	void create_texture_cubemap(state&, void*);
	void create_render_target(state&, void*);
	void create_mesh(state&, void*);
	void delete_buffer(state&, void*);
	void delete_shader(state&, void*);
	void delete_texture(state&, void*);
	void delete_render_target(state&, void*);
	void delete_mesh(state&, void*);
	void set_buffer_data(state&, void*);
	void swap_buffers(state&, void*);
	void sync(state&, void*);
}
using namespace gl_3_3_backend;
using detail::handle;
extern "C" void _create_buffer(buffer_handle* _retval, size_t size, const void* data, buffer_usage usage)
{
	handle _handle = alloc_buffer_handle();
	command cmd(create_buffer);
	cmd.create_buffer.size = size;
	cmd.create_buffer.data = data;
	cmd.create_buffer.usage = usage;
	cmd.create_buffer._handle = _handle;
	_retval->handle = _handle;
	enqueue(cmd);
}
extern "C" void _create_shader(shader_handle* _retval, size_t num_stages, const std::pair<shader_stage, const char*>* stages)
{
	handle _handle = alloc_shader_handle();
	command cmd(create_shader);
	cmd.create_shader.num_stages = num_stages;
	cmd.create_shader.stages = stages;
	cmd.create_shader._handle = _handle;
	_retval->handle = _handle;
	enqueue(cmd);
}
extern "C" void _create_texture_2d(texture_handle* _retval, uint16_t width, uint16_t height, internal_format iformat, image_format format, data_type type, const void* data)
{
	handle _handle = alloc_texture_handle();
	command cmd(create_texture_2d);
	cmd.create_texture_2d.width = width;
	cmd.create_texture_2d.height = height;
	cmd.create_texture_2d.iformat = iformat;
	cmd.create_texture_2d.format = format;
	cmd.create_texture_2d.type = type;
	cmd.create_texture_2d.data = data;
	cmd.create_texture_2d._handle = _handle;
	_retval->handle = _handle;
	enqueue(cmd);
}
extern "C" void _create_texture_3d(texture_handle* _retval, uint16_t width, uint16_t height, uint16_t depth, internal_format iformat, image_format format, data_type type, const void* data)
{
	handle _handle = alloc_texture_handle();
	command cmd(create_texture_3d);
	cmd.create_texture_3d.width = width;
	cmd.create_texture_3d.height = height;
	cmd.create_texture_3d.depth = depth;
	cmd.create_texture_3d.iformat = iformat;
	cmd.create_texture_3d.format = format;
	cmd.create_texture_3d.type = type;
	cmd.create_texture_3d.data = data;
	cmd.create_texture_3d._handle = _handle;
	_retval->handle = _handle;
	enqueue(cmd);
}
extern "C" void _create_texture_cubemap(texture_handle* _retval, uint16_t width, uint16_t height, internal_format iformat, image_format format, data_type type, const void* const* data)
{
	handle _handle = alloc_texture_handle();
	command cmd(create_texture_cubemap);
	cmd.create_texture_cubemap.width = width;
	cmd.create_texture_cubemap.height = height;
	cmd.create_texture_cubemap.iformat = iformat;
	cmd.create_texture_cubemap.format = format;
	cmd.create_texture_cubemap.type = type;
	cmd.create_texture_cubemap.data = data;
	cmd.create_texture_cubemap._handle = _handle;
	_retval->handle = _handle;
	enqueue(cmd);
}
extern "C" void _create_render_target(render_target_handle* _retval)
{
	handle _handle = alloc_render_target_handle();
	command cmd(create_render_target);
	cmd.create_render_target._handle = _handle;
	_retval->handle = _handle;
	enqueue(cmd);
}
extern "C" void _create_mesh(mesh_handle* _retval, size_t num_buffers, std::pair<unsigned, buffer_handle>* handles, size_t num_layouts, buffer_layout* layouts)
{
	handle _handle = alloc_mesh_handle();
	command cmd(create_mesh);
	cmd.create_mesh.num_buffers = num_buffers;
	cmd.create_mesh.handles = handles;
	cmd.create_mesh.num_layouts = num_layouts;
	cmd.create_mesh.layouts = layouts;
	cmd.create_mesh._handle = _handle;
	_retval->handle = _handle;
	enqueue(cmd);
}
extern "C" void _delete_buffer(buffer_handle buffer)
{
	command cmd(delete_buffer);
	cmd.delete_buffer.buffer = buffer;
	enqueue(cmd);
}
extern "C" void _delete_shader(shader_handle shader)
{
	command cmd(delete_shader);
	cmd.delete_shader.shader = shader;
	enqueue(cmd);
}
extern "C" void _delete_texture(texture_handle image)
{
	command cmd(delete_texture);
	cmd.delete_texture.image = image;
	enqueue(cmd);
}
extern "C" void _delete_render_target(render_target_handle render_target)
{
	command cmd(delete_render_target);
	cmd.delete_render_target.render_target = render_target;
	enqueue(cmd);
}
extern "C" void _delete_mesh(mesh_handle mesh)
{
	command cmd(delete_mesh);
	cmd.delete_mesh.mesh = mesh;
	enqueue(cmd);
}
extern "C" void _set_buffer_data(buffer_handle buffer, size_t size, const void* data)
{
	command cmd(set_buffer_data);
	cmd.set_buffer_data.buffer = buffer;
	cmd.set_buffer_data.size = size;
	cmd.set_buffer_data.data = data;
	enqueue(cmd);
}
extern "C" void _swap_buffers()
{
	command cmd(swap_buffers);
	semaphore sema;
	cmd.swap_buffers.sema = &sema;
	enqueue(cmd);
	sema.wait();
}
extern "C" void _sync()
{
	command cmd(sync);
	semaphore sema;
	cmd.sync.sema = &sema;
	enqueue(cmd);
	sema.wait();
}

#endif
