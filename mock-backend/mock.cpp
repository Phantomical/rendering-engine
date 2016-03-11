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

extern "C" void EXPORT CALL_CONV _create_buffer(buffer_handle* _retval)
{
	create_buffer_called++;
}
extern "C" void EXPORT CALL_CONV _create_shader(shader_handle* _retval)
{
	create_shader_called++;
}
extern "C" void EXPORT CALL_CONV _create_texture_2d(texture_handle* _retval)
{
	create_texture_2d_called++;
}
extern "C" void EXPORT CALL_CONV _create_texture_3d(texture_handle* _retval)
{
	create_texture_3d_called++;
}
extern "C" void EXPORT CALL_CONV _create_texture_cubemap(texture_handle* _retval)
{
	create_texture_cubemap_called++;
}
extern "C" void EXPORT CALL_CONV _create_render_target(render_target_handle* _retval)
{
	create_render_target_called++;
}
extern "C" void EXPORT CALL_CONV _delete_buffer(buffer_handle arg0)
{
	delete_buffer_called++;
}
extern "C" void EXPORT CALL_CONV _delete_shader(shader_handle arg0)
{
	delete_shader_called++;
}
extern "C" void EXPORT CALL_CONV _delete_texture(texture_handle arg0)
{
	delete_texture_called++;
}
extern "C" void EXPORT CALL_CONV _delete_render_target(render_target_handle arg0)
{
	delete_render_target_called++;
}
extern "C" void EXPORT CALL_CONV _set_buffer_data(buffer_handle arg0, size_t arg1, const void* arg2)
{
	set_buffer_data_called++;
}
extern "C" void EXPORT CALL_CONV _swap_buffers()
{
	swap_buffers_called++;
}
extern "C" void EXPORT CALL_CONV _sync()
{
	sync_called++;
}

extern "C" void EXPORT CALL_CONV _init() { }
extern "C" void EXPORT CALL_CONV _terminate() { }
