#ifndef RE_INTERFACE_H
#define RE_INTERFACE_H

#include "handle.h"
#include "allocators.h"
#include <initializer_list>
#include <utility>
#include <string>
#include <array>

#define GLDR_DECLARE_HANDLE(name) struct name { detail::handle handle; }
//This is the calling convention that MUST be used when creating renderer backends
#ifdef _WIN32
#	define GLDR_BACKEND_CALL_CONV __cdecl
#else
#	define GLDR_BACKEND_CALL_CONV
#endif

namespace gldr
{
	GLDR_DECLARE_HANDLE(mesh_handle);
	GLDR_DECLARE_HANDLE(buffer_handle);
	GLDR_DECLARE_HANDLE(shader_handle);
	GLDR_DECLARE_HANDLE(texture_handle);
	GLDR_DECLARE_HANDLE(render_target_handle);
	
	enum shader_stage
	{
		VERTEX,
		FRAGMENT,
		GEOMETRY,
		TESS_EVALUATION,
		TESS_CONTROL,
		COMPUTE,
	};
	enum buffer_access_flags
	{
		READ = 0x1,
		WRITE = 0x2,
		READ_WRITE = READ | WRITE,
	};
	enum buffer_usage
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
	enum internal_format
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
	enum image_format
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
	enum data_type
	{
		UNSIGNED_BYTE,
		UNSIGNED_SHORT,
		UNSIGNED_INT,
		BYTE,
		SHORT,
		INT,
		FLOAT,
	};
	
	class backend
	{
	private:
		struct state;
		state* _state;
		void init(const std::string& backend_lib);
		void terminate();
		void sync_callback();
		allocators::linear_atomic* allocator();
		
	public:
		backend(const std::string& backend_lib);
		~backend();
		backend(const backend&) = delete;
		void operator =(const backend&) = delete;
		
		bool is_valid() const;
		
		buffer_handle create_buffer(size_t size, const void* data, buffer_usage usage);
		shader_handle create_shader(size_t num_stages, std::pair<shader_stage, const char*>* stages);
		shader_handle create_shader(const std::initializer_list<std::pair<shader_stage, const char*>>& stages);
		texture_handle create_texture_2d(size_t width, size_t height, internal_format iformat, image_format format, data_type type, const void* data);
		texture_handle create_texture_3d(size_t width, size_t height, size_t depth, internal_format iformat, image_format format, data_type type, const void* data);
		texture_handle create_texture_cubemap(size_t width, size_t height, internal_format iformat, image_format format, data_type type, const std::array<const void*, 6>& data);
		texture_handle create_texture_cubemap(size_t width, size_t height, internal_format iformat, image_format format, data_type type, const void* const* data);
		render_target_handle create_render_target();
		void delete_buffer(buffer_handle buffer);
		void delete_shader(shader_handle shader);
		void delete_texture(texture_handle image);
		void delete_render_target(render_target_handle render_target);
		void set_buffer_data(buffer_handle buffer, size_t size, const void* data);
		void swap_buffers();
		void sync();
	};
}

#endif
