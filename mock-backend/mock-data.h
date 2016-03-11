#ifndef MOCK_DATA_H
#define MOCK_DATA_H

#ifdef _MSC_VER
#define MOCK_EXPORT __declspec(dllimport)
#else
#define MOCK_EXPORT
#endif

MOCK_EXPORT extern size_t create_buffer_called;
MOCK_EXPORT extern size_t create_shader_called;
MOCK_EXPORT extern size_t create_texture_2d_called;
MOCK_EXPORT extern size_t create_texture_3d_called;
MOCK_EXPORT extern size_t create_texture_cubemap_called;
MOCK_EXPORT extern size_t create_render_target_called;
MOCK_EXPORT extern size_t create_mesh_called;
MOCK_EXPORT extern size_t delete_buffer_called;
MOCK_EXPORT extern size_t delete_shader_called;
MOCK_EXPORT extern size_t delete_texture_called;
MOCK_EXPORT extern size_t delete_render_target_called;
MOCK_EXPORT extern size_t delete_mesh_called;
MOCK_EXPORT extern size_t set_buffer_data_called;
MOCK_EXPORT extern size_t swap_buffers_called;
MOCK_EXPORT extern size_t sync_called;

#endif
