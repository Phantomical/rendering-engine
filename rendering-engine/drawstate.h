#pragma once

#include <cstdint>

namespace gldr
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
			};
			uint32_t _key_val; //This will work as long as there are less than 32 flags
		};

		drawstate() :
			_key_val(0)
		{
			
		}
	};
}
