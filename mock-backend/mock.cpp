#include "interface.h"

#define CALL_CONV GLDR_BACKEND_CALL_CONV
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

using namespace gldr;

__declspec(dllexport) size_t create_buffer_called = 0;
__declspec(dllexport) size_t create_shader_called = 0;
__declspec(dllexport) size_t create_texture_2d_called = 0;
__declspec(dllexport) size_t create_texture_3d_called = 0;
__declspec(dllexport) size_t create_texture_cubemap_called = 0;
__declspec(dllexport) size_t create_render_target_called = 0;
__declspec(dllexport) size_t delete_buffer_called = 0;
__declspec(dllexport) size_t delete_shader_called = 0;
__declspec(dllexport) size_t delete_texture_called = 0;
__declspec(dllexport) size_t delete_render_target_called = 0;
__declspec(dllexport) size_t set_buffer_data_called = 0;
__declspec(dllexport) size_t swap_buffers_called = 0;
__declspec(dllexport) size_t sync_called = 0;

extern "C" buffer_handle EXPORT CALL_CONV _create_buffer(size_t arg0, const void* arg1, buffer_usage arg2)
{
	create_buffer_called++;
	return buffer_handle();
}
extern "C" shader_handle EXPORT CALL_CONV _create_shader(size_t arg0, std::pair<shader_stage, const char*>* arg1)
{
	create_shader_called++;
	return shader_handle();
}
extern "C" texture_handle EXPORT CALL_CONV _create_texture_2d(size_t arg0, size_t arg1, internal_format arg2, image_format arg3, data_type arg4, const void* arg5)
{
	create_texture_2d_called++;
	return texture_handle();
}
extern "C" texture_handle EXPORT CALL_CONV _create_texture_3d(size_t arg0, size_t arg1, size_t arg2, internal_format arg3, image_format arg4, data_type arg5, const void* arg6)
{
	create_texture_3d_called++;
	return texture_handle();
}
extern "C" texture_handle EXPORT CALL_CONV _create_texture_cubemap(size_t arg0, size_t arg1, internal_format arg2, image_format arg3, data_type arg4, const void* const* arg5)
{
	create_texture_cubemap_called++;
	return texture_handle();
}
extern "C" render_target_handle EXPORT CALL_CONV _create_render_target()
{
	create_render_target_called++;
	return render_target_handle();
}
extern "C" void EXPORT CALL_CONV _delete_buffer(buffer_handle arg0)
{
	delete_buffer_called++;
	return void();
}
extern "C" void EXPORT CALL_CONV _delete_shader(shader_handle arg0)
{
	delete_shader_called++;
	return void();
}
extern "C" void EXPORT CALL_CONV _delete_texture(texture_handle arg0)
{
	delete_texture_called++;
	return void();
}
extern "C" void EXPORT CALL_CONV _delete_render_target(render_target_handle arg0)
{
	delete_render_target_called++;
	return void();
}
extern "C" void EXPORT CALL_CONV _set_buffer_data(buffer_handle arg0, size_t arg1, const void* arg2)
{
	set_buffer_data_called++;
	return void();
}
extern "C" void EXPORT CALL_CONV _swap_buffers()
{
	swap_buffers_called++;
	return void();
}
extern "C" void EXPORT CALL_CONV _sync()
{
	sync_called++;
	return void();
}
extern "C" void EXPORT CALL_CONV _init() { }
extern "C" void EXPORT CALL_CONV _terminate() { }

