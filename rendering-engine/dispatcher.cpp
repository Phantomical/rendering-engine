#include "allocators.h"
#include "interface.h"
#include <functional>

#define CALL_CONV GLDR_BACKEND_CALL_CONV

void* load_init(const char*);
void* load_func(void*, const char*);
void load_terminate(void*);

namespace gldr
{
	typedef void(CALL_CONV*create_buffer_proc)(buffer_handle*, size_t arg0, const void* arg1, buffer_usage arg2);
	typedef void(CALL_CONV*create_shader_proc)(shader_handle*, size_t arg0, const std::pair<shader_stage, const char*>* arg1);
	typedef void(CALL_CONV*create_texture_2d_proc)(texture_handle*, uint16_t arg0, uint16_t arg1, internal_format arg2, image_format arg3, data_type arg4, const void* arg5);
	typedef void(CALL_CONV*create_texture_3d_proc)(texture_handle*, uint16_t arg0, uint16_t arg1, uint16_t arg2, internal_format arg3, image_format arg4, data_type arg5, const void* arg6);
	typedef void(CALL_CONV*create_texture_cubemap_proc)(texture_handle*, uint16_t arg0, uint16_t arg1, internal_format arg2, image_format arg3, data_type arg4, const void* const* arg5);
	typedef void(CALL_CONV*create_render_target_proc)(render_target_handle*);
	typedef void(CALL_CONV*create_mesh_proc)(mesh_handle*, size_t arg0, std::pair<unsigned, buffer_handle>* arg1, size_t arg2, buffer_layout* arg3);
	typedef void(CALL_CONV*delete_buffer_proc)(buffer_handle arg0);
	typedef void(CALL_CONV*delete_shader_proc)(shader_handle arg0);
	typedef void(CALL_CONV*delete_texture_proc)(texture_handle arg0);
	typedef void(CALL_CONV*delete_render_target_proc)(render_target_handle arg0);
	typedef void(CALL_CONV*delete_mesh_proc)(mesh_handle arg0);
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
		create_mesh_proc create_mesh_func;
		delete_buffer_proc delete_buffer_func;
		delete_shader_proc delete_shader_func;
		delete_texture_proc delete_texture_func;
		delete_render_target_proc delete_render_target_func;
		delete_mesh_proc delete_mesh_func;
		set_buffer_data_proc set_buffer_data_func;
		swap_buffers_proc swap_buffers_func;
		sync_proc sync_func;
		void(*terminate_func)();
		void* handle;
		allocators::linear_atomic alloc;
		
		state() :
			alloc(1 << 16)
		{ }
	};
	
	buffer_handle backend::create_buffer(size_t arg0, const void* arg1, buffer_usage arg2)
	{
		buffer_handle _retval;
		_state->create_buffer_func(&_retval, arg0, arg1, arg2);
		return _retval;
	}
	shader_handle backend::create_shader(size_t arg0, const std::pair<shader_stage, const char*>* arg1)
	{
		shader_handle _retval;
		_state->create_shader_func(&_retval, arg0, arg1);
		return _retval;
	}
	texture_handle backend::create_texture_2d(uint16_t arg0, uint16_t arg1, internal_format arg2, image_format arg3, data_type arg4, const void* arg5)
	{
		texture_handle _retval;
		_state->create_texture_2d_func(&_retval, arg0, arg1, arg2, arg3, arg4, arg5);
		return _retval;
	}
	texture_handle backend::create_texture_3d(uint16_t arg0, uint16_t arg1, uint16_t arg2, internal_format arg3, image_format arg4, data_type arg5, const void* arg6)
	{
		texture_handle _retval;
		_state->create_texture_3d_func(&_retval, arg0, arg1, arg2, arg3, arg4, arg5, arg6);
		return _retval;
	}
	texture_handle backend::create_texture_cubemap(uint16_t arg0, uint16_t arg1, internal_format arg2, image_format arg3, data_type arg4, const void* const* arg5)
	{
		texture_handle _retval;
		_state->create_texture_cubemap_func(&_retval, arg0, arg1, arg2, arg3, arg4, arg5);
		return _retval;
	}
	render_target_handle backend::create_render_target()
	{
		render_target_handle _retval;
		_state->create_render_target_func(&_retval);
		return _retval;
	}
	mesh_handle backend::create_mesh(size_t arg0, std::pair<unsigned, buffer_handle>* arg1, size_t arg2, buffer_layout* arg3)
	{
		mesh_handle _retval;
		_state->create_mesh_func(&_retval, arg0, arg1, arg2, arg3);
		return _retval;
	}
	void backend::delete_buffer(buffer_handle arg0)
	{
		_state->delete_buffer_func(arg0);
	}
	void backend::delete_shader(shader_handle arg0)
	{
		_state->delete_shader_func(arg0);
	}
	void backend::delete_texture(texture_handle arg0)
	{
		_state->delete_texture_func(arg0);
	}
	void backend::delete_render_target(render_target_handle arg0)
	{
		_state->delete_render_target_func(arg0);
	}
	void backend::delete_mesh(mesh_handle arg0)
	{
		_state->delete_mesh_func(arg0);
	}
	void backend::set_buffer_data(buffer_handle arg0, size_t arg1, const void* arg2)
	{
		_state->set_buffer_data_func(arg0, arg1, arg2);
	}
	void backend::swap_buffers()
	{
		struct cb {
			std::function<void()> f;
			cb(const std::function<void()>& v) : f(v) { }
			~cb() { f(); }
		} c([=](){ this->sync_callback(); });
		_state->swap_buffers_func();
	}
	void backend::sync()
	{
		_state->sync_func();
	}
	
	void backend::init(const std::string& lib)
	{
		_state = new state;
		_state->handle = load_init(lib.c_str());
		_state->create_buffer_func = static_cast<create_buffer_proc>(load_func(_state->handle, "_create_buffer"));
		_state->create_shader_func = static_cast<create_shader_proc>(load_func(_state->handle, "_create_shader"));
		_state->create_texture_2d_func = static_cast<create_texture_2d_proc>(load_func(_state->handle, "_create_texture_2d"));
		_state->create_texture_3d_func = static_cast<create_texture_3d_proc>(load_func(_state->handle, "_create_texture_3d"));
		_state->create_texture_cubemap_func = static_cast<create_texture_cubemap_proc>(load_func(_state->handle, "_create_texture_cubemap"));
		_state->create_render_target_func = static_cast<create_render_target_proc>(load_func(_state->handle, "_create_render_target"));
		_state->create_mesh_func = static_cast<create_mesh_proc>(load_func(_state->handle, "_create_mesh"));
		_state->delete_buffer_func = static_cast<delete_buffer_proc>(load_func(_state->handle, "_delete_buffer"));
		_state->delete_shader_func = static_cast<delete_shader_proc>(load_func(_state->handle, "_delete_shader"));
		_state->delete_texture_func = static_cast<delete_texture_proc>(load_func(_state->handle, "_delete_texture"));
		_state->delete_render_target_func = static_cast<delete_render_target_proc>(load_func(_state->handle, "_delete_render_target"));
		_state->delete_mesh_func = static_cast<delete_mesh_proc>(load_func(_state->handle, "_delete_mesh"));
		_state->set_buffer_data_func = static_cast<set_buffer_data_proc>(load_func(_state->handle, "_set_buffer_data"));
		_state->swap_buffers_func = static_cast<swap_buffers_proc>(load_func(_state->handle, "_swap_buffers"));
		_state->sync_func = static_cast<sync_proc>(load_func(_state->handle, "_sync"));
		_state->terminate_func = static_cast<decltype(_state->terminate_func)>(load_func(_state->handle, "_terminate"));
		void(*init_func)() = static_cast<void(*)()>(load_func(_state->handle, "_init"));
		if (init_func)
		{
			init_func();
		}
		else
		{
			delete _state;
			_state = nullptr;
		}
	}
	void backend::terminate()
	{
		if (_state != nullptr)
		{
			_state->terminate_func();
			load_terminate(_state->handle);
			delete _state;
		}
	}
	backend::backend(const std::string& backend_lib)
	{
		init(backend_lib);
	}
	backend::~backend()
	{
		terminate();
	}
	bool backend::is_valid() const
	{
		return _state != nullptr;
	}
	allocators::linear_atomic* backend::allocator()
	{
		return &_state->alloc;
	}
}
