/*
    This file was generated by mock-gen.py as part of the project build.

	This file is part of rendering-engine.
	
	rendering-engine is free software : you can redistribute it and / or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	rendering-engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with rendering-engine. If not, see <http://www.gnu.org/licenses/>.
*/

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
