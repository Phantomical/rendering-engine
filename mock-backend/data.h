#pragma once

__declspec(dllexport) extern size_t create_buffer_called;
__declspec(dllexport) extern size_t create_shader_called;
__declspec(dllexport) extern size_t create_texture_2d_called;
__declspec(dllexport) extern size_t create_texture_3d_called;
__declspec(dllexport) extern size_t create_texture_cubemap_called;
__declspec(dllexport) extern size_t create_render_target_called;
__declspec(dllexport) extern size_t delete_buffer_called;
__declspec(dllexport) extern size_t delete_shader_called;
__declspec(dllexport) extern size_t delete_texture_called;
__declspec(dllexport) extern size_t delete_render_target_called;
__declspec(dllexport) extern size_t set_buffer_data_called;
__declspec(dllexport) extern size_t swap_buffers_called;
__declspec(dllexport) extern size_t sync_called;
