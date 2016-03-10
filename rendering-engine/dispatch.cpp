#include "allocators.h"
#include "interface.h"

#ifdef _WIN32
#	define CALL_CONV __cdecl
#endif

void* load_init(const char*);
void* load_func(void*, const char*);
void* load_terminate(void*);

namespace gldr
{
	typedef buffer_handle(CALL_CONV*create_buffer_proc)(size_t arg0, const void* arg1, buffer_usage arg2);
	typedef shader_handle(CALL_CONV*create_shader_proc)(size_t arg0, std::pair<shader_stage, std::string>* arg1);
	typedef texture_handle(CALL_CONV*create_texture_2d_proc)(size_t arg0, size_t arg1, internal_format arg2, image_format arg3, data_type arg4, const void* arg5);
	typedef texture_handle(CALL_CONV*create_texture_3d_proc)(size_t arg0, size_t arg1, size_t arg2, internal_format arg3, image_format arg4, data_type arg5, const void* arg6);
	typedef texture_handle(CALL_CONV*create_texture_cubemap_proc)(size_t arg0, size_t arg1, internal_format arg2, image_format arg3, data_type arg4, const void* const* arg5);
	typedef render_target_handle(CALL_CONV*create_render_target_proc)();
	typedef void(CALL_CONV*delete_buffer_proc)(buffer_handle arg0);
	typedef void(CALL_CONV*delete_shader_proc)(shader_handle arg0);
	typedef void(CALL_CONV*delete_texture_proc)(texture_handle arg0);
	typedef void(CALL_CONV*delete_render_target_proc)(render_target_handle arg0);
	typedef void(CALL_CONV*set_buffer_data_proc)(buffer_handle arg0, size_t arg1, const void* arg2);
	typedef void(CALL_CONV*swap_buffers_proc)();
	typedef void(CALL_CONV*sync_proc)();
	
	create_buffer_proc create_buffer_func;
	create_shader_proc create_shader_func;
	create_texture_2d_proc create_texture_2d_func;
	create_texture_3d_proc create_texture_3d_func;
	create_texture_cubemap_proc create_texture_cubemap_func;
	create_render_target_proc create_render_target_func;
	delete_buffer_proc delete_buffer_func;
	delete_shader_proc delete_shader_func;
	delete_texture_proc delete_texture_func;
	delete_render_target_proc delete_render_target_func;
	set_buffer_data_proc set_buffer_data_func;
	swap_buffers_proc swap_buffers_func;
	sync_proc sync_func;
	
	buffer_handle create_buffer(size_t arg0, const void* arg1, buffer_usage arg2)
	{
		return create_buffer_func(arg0, arg1, arg2);
	}
	shader_handle create_shader(size_t arg0, std::pair<shader_stage, std::string>* arg1)
	{
		return create_shader_func(arg0, arg1);
	}
	texture_handle create_texture_2d(size_t arg0, size_t arg1, internal_format arg2, image_format arg3, data_type arg4, const void* arg5)
	{
		return create_texture_2d_func(arg0, arg1, arg2, arg3, arg4, arg5);
	}
	texture_handle create_texture_3d(size_t arg0, size_t arg1, size_t arg2, internal_format arg3, image_format arg4, data_type arg5, const void* arg6)
	{
		return create_texture_3d_func(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
	}
	texture_handle create_texture_cubemap(size_t arg0, size_t arg1, internal_format arg2, image_format arg3, data_type arg4, const void* const* arg5)
	{
		return create_texture_cubemap_func(arg0, arg1, arg2, arg3, arg4, arg5);
	}
	render_target_handle create_render_target()
	{
		return create_render_target_func();
	}
	void delete_buffer(buffer_handle arg0)
	{
		return delete_buffer_func(arg0);
	}
	void delete_shader(shader_handle arg0)
	{
		return delete_shader_func(arg0);
	}
	void delete_texture(texture_handle arg0)
	{
		return delete_texture_func(arg0);
	}
	void delete_render_target(render_target_handle arg0)
	{
		return delete_render_target_func(arg0);
	}
	void set_buffer_data(buffer_handle arg0, size_t arg1, const void* arg2)
	{
		return set_buffer_data_func(arg0, arg1, arg2);
	}
	void swap_buffers()
	{
		return swap_buffers_func();
	}
	void sync()
	{
		return sync_func();
	}
	
	void(*terminate_func)();
	void* handle;
	void init(const std::string& lib)
	{
		handle = load_init(lib.c_str());
		create_buffer_func = static_cast<decltype(create_buffer_func)>(load_func(handle, "create_buffer"));
		create_shader_func = static_cast<decltype(create_shader_func)>(load_func(handle, "create_shader"));
		create_texture_2d_func = static_cast<decltype(create_texture_2d_func)>(load_func(handle, "create_texture_2d"));
		create_texture_3d_func = static_cast<decltype(create_texture_3d_func)>(load_func(handle, "create_texture_3d"));
		create_texture_cubemap_func = static_cast<decltype(create_texture_cubemap_func)>(load_func(handle, "create_texture_cubemap"));
		create_render_target_func = static_cast<decltype(create_render_target_func)>(load_func(handle, "create_render_target"));
		delete_buffer_func = static_cast<decltype(delete_buffer_func)>(load_func(handle, "delete_buffer"));
		delete_shader_func = static_cast<decltype(delete_shader_func)>(load_func(handle, "delete_shader"));
		delete_texture_func = static_cast<decltype(delete_texture_func)>(load_func(handle, "delete_texture"));
		delete_render_target_func = static_cast<decltype(delete_render_target_func)>(load_func(handle, "delete_render_target"));
		set_buffer_data_func = static_cast<decltype(set_buffer_data_func)>(load_func(handle, "set_buffer_data"));
		swap_buffers_func = static_cast<decltype(swap_buffers_func)>(load_func(handle, "swap_buffers"));
		sync_func = static_cast<decltype(sync_func)>(load_func(handle, "sync"));
		terminate_func = static_cast<decltype(terminate_func)>(load_func(handle, "terminate"));
		static_cast<void(*)()>(load_func(handle, "init"))();
	}
	void terminate()
	{
		terminate_func();
		load_terminate(handle);
	}
}
