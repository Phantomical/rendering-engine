//Copyright (c) 2016 Sean Lynch
/*
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

#ifndef RE_DRAWSTATE_H
#define RE_DRAWSTATE_H

#include <cstdint>

namespace re
{
	struct drawstate
	{
		union
		{
			struct
			{
				bool blend : 1; //Enables alpha blending
				bool cull_face : 1;
				bool debug_output : 1; //Enables GL_DEBUG_OUTPUT_SYNCHRONOUS
				bool depth_clamp : 1;
				bool depth_test : 1;
				bool dither : 1;
				bool line_smooth : 1;
				bool multisample : 1;
				bool polygon_offset_fill : 1;
				bool polygon_offset_line : 1;
				bool polygon_offset_point : 1;
				bool polygon_smooth : 1;
				bool primitive_restart : 1; //Enables GL_PRIMITIVE_RESTART_FIXED_INDEX
				bool rasterizer_discard : 1; //For transform feedback
				bool sample_alpha_to_coverage : 1;
				bool sample_alpha_to_one : 1;
				bool sample_coverage : 1;
				bool sample_shading : 1;
				bool sample_mask : 1;
				bool scissor_test : 1; // Enable the stencil test
				bool texture_cube_map_seamless : 1; //Enables smoother interpolation in cubemaps
				bool transform_feedback : 1; //Enables transform feedback
				bool discard_output : 1; //Prevents output primitives from being rendered (Useful with transform feedback)
			};
			uint32_t _key_val; //This will work as long as there are less than 32 flags
		};

		drawstate() :
			_key_val(0)
		{
			
		}
		drawstate(uint32_t bitfield) :
			_key_val(bitfield)
		{

		}
	};
}
#endif
