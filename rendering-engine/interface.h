#pragma once

#include "handle.h"

#include <initializer_list>
#include <utility>
#include <string>
#include <array>

#define GLDR_DECLARE_HANDLE(name) struct name { detail::handle handle; }

namespace gldr
{
	GLDR_DECLARE_HANDLE(buffer_handle);
	GLDR_DECLARE_HANDLE(shader_handle);
	GLDR_DECLARE_HANDLE(texture_handle);
	GLDR_DECLARE_HANDLE(render_target_handle);

	enum class shader_stage : uint8_t
	{
		VERTEX,
		FRAGMENT,
		GEOMETRY,
		TESS_EVALUATION,
		TESS_CONTROL,
		COMPUTE
	};
	enum class buffer_access_flags : uint8_t
	{
		READ = 0x1,
		WRITE = 0x2,
		READ_WRITE = READ | WRITE
	};
	enum class buffer_usage : uint8_t
	{
		STREAM_DRAW,
		STREAM_READ,
		STREAM_COPY,
		STATIC_DRAW,
		STATIC_READ,
		STATIC_COPY,
		DYNAMIC_DRAW,
		DYNAMIC_READ,
		DYNAMIC_COPY
	};
	enum class internal_format : uint8_t
	{
		R8,
		RG8,
		RGB8,
		RGBA8,
		R32F,
		RG32F,
		RGB32F,
		RGBA32F
	};
	enum class image_format : uint8_t
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
		DEPTH_STENCIL
	};
	enum class data_type : uint8_t
	{
		UNSIGNED_BYTE,
		UNSIGNED_SHORT,
		UNSIGNED_INT,
		BYTE,
		SHORT,
		INT,
		FLOAT
	};

	//All functions in the unsynched namespace have an undefined
	//value for their out parameter until a sync occurs.
	//A sync occurs when sync() is called or swap_buffer() is called.
	namespace unsynced
	{
		buffer_handle create_buffer(size_t size, const void* data, buffer_usage usage_hint);
		shader_handle create_shader(const std::initializer_list<std::pair<shader_stage, std::string>>& stages);
		shader_handle create_shader(size_t num_stages, std::pair<shader_stage, std::string>* stages);
		texture_handle create_texture_2d(size_t width, size_t height, internal_format iformat, image_format format, data_type type, const void* data);
		texture_handle create_texture_3d(size_t width, size_t height, size_t depth, internal_format iformat, image_format format, data_type type, const void* data);
		//Order of faces in the data array is as follows: top, bottom, front, back, right, left
		texture_handle create_texture_cubemap(size_t width, size_t height, internal_format iformat, image_format format, data_type type, const std::array<void*, 6>& data);
		void create_rendertarget(render_target_handle* out /*, TODO */);

		void delete_buffer(buffer_handle buffer);
		void delete_shader(shader_handle shader);
		void delete_texture(texture_handle image);
		void delete_rendertarget(render_target_handle render_target);

		void map_buffer_range(void** out, buffer_handle buffer, size_t start, size_t end, buffer_access_flags access);
		//Unmaps the buffer while publishing any changes made to GPU memory
		void unmap_buffer(buffer_handle buffer);
		//Flushes the buffer to GPU memory. Using this buffer handle in any way will
		//result in undefined behaviour
		void flush_buffer(buffer_handle buffer);

		//TODO: Draw Commands
	}

	void swap_buffers();
	void sync();

	render_target_handle get_default_render_target();
}
