#include "allocators.h"
#include "interface.h"

#define CALL_CONV GLDR_BACKEND_CALL_CONV

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
	
	struct backend::state
	{
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
		void(*terminate_func)();
		void* handle;
	};
	
	buffer_handle backend::create_buffer(size_t arg0, const void* arg1, buffer_usage arg2)
	{
		return _state->create_buffer_func(arg0, arg1, arg2);
	}
	shader_handle backend::create_shader(size_t arg0, std::pair<shader_stage, std::string>* arg1)
	{
		return _state->create_shader_func(arg0, arg1);
	}
	texture_handle backend::create_texture_2d(size_t arg0, size_t arg1, internal_format arg2, image_format arg3, data_type arg4, const void* arg5)
	{
		return _state->create_texture_2d_func(arg0, arg1, arg2, arg3, arg4, arg5);
	}
	texture_handle backend::create_texture_3d(size_t arg0, size_t arg1, size_t arg2, internal_format arg3, image_format arg4, data_type arg5, const void* arg6)
	{
		return _state->create_texture_3d_func(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
	}
	texture_handle backend::create_texture_cubemap(size_t arg0, size_t arg1, internal_format arg2, image_format arg3, data_type arg4, const void* const* arg5)
	{
		return _state->create_texture_cubemap_func(arg0, arg1, arg2, arg3, arg4, arg5);
	}
	render_target_handle backend::create_render_target()
	{
		return _state->create_render_target_func();
	}
	void backend::delete_buffer(buffer_handle arg0)
	{
		return _state->delete_buffer_func(arg0);
	}
	void backend::delete_shader(shader_handle arg0)
	{
		return _state->delete_shader_func(arg0);
	}
	void backend::delete_texture(texture_handle arg0)
	{
		return _state->delete_texture_func(arg0);
	}
	void backend::delete_render_target(render_target_handle arg0)
	{
		return _state->delete_render_target_func(arg0);
	}
	void backend::set_buffer_data(buffer_handle arg0, size_t arg1, const void* arg2)
	{
		return _state->set_buffer_data_func(arg0, arg1, arg2);
	}
	void backend::swap_buffers()
	{
		return _state->swap_buffers_func();
	}
	void backend::sync()
	{
		return _state->sync_func();
	}
	
	void backend::init(const std::string& lib)
	{
		_state->handle = load_init(lib.c_str());
		_state->create_buffer_func = static_cast<create_buffer_proc>(load_func(_state->handle, "create_buffer"));
		_state->create_shader_func = static_cast<create_shader_proc>(load_func(_state->handle, "create_shader"));
		_state->create_texture_2d_func = static_cast<create_texture_2d_proc>(load_func(_state->handle, "create_texture_2d"));
		_state->create_texture_3d_func = static_cast<create_texture_3d_proc>(load_func(_state->handle, "create_texture_3d"));
		_state->create_texture_cubemap_func = static_cast<create_texture_cubemap_proc>(load_func(_state->handle, "create_texture_cubemap"));
		_state->create_render_target_func = static_cast<create_render_target_proc>(load_func(_state->handle, "create_render_target"));
		_state->delete_buffer_func = static_cast<delete_buffer_proc>(load_func(_state->handle, "delete_buffer"));
		_state->delete_shader_func = static_cast<delete_shader_proc>(load_func(_state->handle, "delete_shader"));
		_state->delete_texture_func = static_cast<delete_texture_proc>(load_func(_state->handle, "delete_texture"));
		_state->delete_render_target_func = static_cast<delete_render_target_proc>(load_func(_state->handle, "delete_render_target"));
		_state->set_buffer_data_func = static_cast<set_buffer_data_proc>(load_func(_state->handle, "set_buffer_data"));
		_state->swap_buffers_func = static_cast<swap_buffers_proc>(load_func(_state->handle, "swap_buffers"));
		_state->sync_func = static_cast<sync_proc>(load_func(_state->handle, "sync"));
		_state->terminate_func = static_cast<decltype(_state->terminate_func)>(load_func(_state->handle, "terminate"));
		static_cast<void(*)()>(load_func(_state->handle, "init"))();
	}
	void backend::terminate()
	{
		_state->terminate_func();
		load_terminate(_state->handle);
	}
}
