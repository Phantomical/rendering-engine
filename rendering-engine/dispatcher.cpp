
#include "allocators.h"
#include "interface.h"

#ifdef _WIN32
#	define CALL_CONV __cdecl
#endif

using namespace gldr;

typedef void(CALL_CONV*initialize_proc)();
typedef void(CALL_CONV*terminate_proc)();

typedef buffer_handle(CALL_CONV*create_buffer_proc)(size_t, const void*, buffer_usage);
typedef shader_handle(CALL_CONV*create_shader_proc)(size_t, std::pair<shader_stage, std::string>*);
typedef texture_handle(CALL_CONV*create_texture_2d_proc)(size_t, size_t, internal_format, image_format, data_type, const void*);
typedef texture_handle(CALL_CONV*create_texture_3d_proc)(size_t, size_t, size_t, internal_format, image_format, data_type, const void*);
typedef texture_handle(CALL_CONV*create_texture_cubemap_proc)(size_t, size_t, internal_format, image_format, data_type, const void**);

typedef void(CALL_CONV*delete_buffer_proc)(buffer_handle);
typedef void(CALL_CONV*delete_shader_proc)(shader_handle);
typedef void(CALL_CONV*delete_texture_proc)(texture_handle);
typedef void(CALL_CONV*delete_render_target_proc)(render_target_handle);

typedef void(CALL_CONV*set_buffer_data_proc)(buffer_handle, size_t, const void*);

typedef void(CALL_CONV*swap_buffers_proc)();
typedef void(CALL_CONV*sync_proc)();

create_buffer_proc create_buffer_func;
create_shader_proc create_shader_func;
create_texture_2d_proc create_texture_2d_func;
create_texture_3d_proc create_texture_3d_func;
create_texture_cubemap_proc create_texture_cubemap_func;

delete_buffer_proc delete_buffer_func;
delete_shader_proc delete_shader_func;
delete_texture_proc delete_texture_func;
delete_render_target_proc delete_render_target_func;

set_buffer_data_proc set_buffer_data_func;

swap_buffers_proc swap_buffers_func;
sync_proc sync_func;

namespace gldr
{
	allocators::linear_atomic alloc{ 1 << 13 };

	buffer_handle create_buffer(size_t size, const void* data, buffer_usage usage_hint)
	{
		return create_buffer_func(size, data, usage_hint);
	}
	shader_handle create_shader(const std::initializer_list<std::pair<shader_stage, std::string>>& stages)
	{
		size_t sz = stages.size();
		std::pair<shader_stage, std::string>* array = (std::pair<shader_stage, std::string>*)
			alloc.alloc(stages.size() * sizeof(std::pair<shader_stage, std::string>));

		std::memset(array, 0, sizeof(std::pair<shader_stage, std::string>) * sz);
		std::copy(stages.begin(), stages.end(), array);

		return create_shader(sz, array);
	}
	shader_handle create_shader(size_t num_stages, std::pair<shader_stage, std::string>* stages)
	{
		return create_shader_func(num_stages, stages);
	}
	texture_handle create_texture_2d(size_t width, size_t height, internal_format iformat, image_format format, data_type type, const void* data)
	{
		return create_texture_2d_func(width, height, iformat, format, type, data);
	}
	texture_handle create_texture_3d(size_t width, size_t height, size_t depth, internal_format iformat, image_format format, data_type type, const void* data)
	{
		return create_texture_3d_func(width, height, depth, iformat, format, type, data);
	}
	//Order of faces in the data array is as follows: top, bottom, front, back, right, left
	texture_handle create_texture_cubemap(size_t width, size_t height, internal_format iformat, image_format format, data_type type, const std::array<void*, 6>& data)
	{
		const void** array = (const void**)alloc.alloc(sizeof(void*) * 6);
		std::memcpy(array, data.data(), sizeof(const void*) * 6);

		return create_texture_cubemap_func(width, height, iformat, format, type, array);
	}

	void delete_buffer(buffer_handle buffer)
	{
		delete_buffer_func(buffer);
	}
	void delete_shader(shader_handle shader)
	{
		delete_shader_func(shader);
	}
	void delete_texture(texture_handle image)
	{
		delete_texture_func(image);
	}
	void delete_render_target(render_target_handle render_target)
	{
		delete_render_target_func(render_target);
	}

	void set_buffer_data(buffer_handle buffer, size_t size, const void* data)
	{
		set_buffer_data_func(buffer, size, data);
	}

	void swap_buffers()
	{
		swap_buffers_func();
	}
	void sync()
	{
		sync_func();
	}
}
