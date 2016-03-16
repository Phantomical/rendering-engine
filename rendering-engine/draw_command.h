#pragma once

#include "drawstate.h"
#include "interface.h"

#include "glm\glm.hpp"

#include <vector>

namespace re
{
	enum primitive_type : uint8_t
	{
		PRIMITIVE_TRIANGLES,
		PRIMITIVE_TRIANGLE_STRIP,
		PRIMITIVE_POINTS,
		PRIMITIVE_LINES
	};
	enum uniform_type : uint8_t
	{
		UNIFORM_BOOL,
		UNIFORM_BYTE,
		UNIFORM_SHORT,
		UNIFORM_INT,
		UNIFORM_UBYTE,
		UNIFORM_USHORT,
		UNIFORM_UINT,
		UNIFORM_TEXTURE,
		UNIFORM_VEC2I,
		UNIFORM_VEC3I,
		UNIFORM_VEC4I,
		UNIFORM_VEC2UI,
		UNIFORM_VEC3UI,
		UNIFORM_VEC4UI,
		UNIFORM_VEC2F,
		UNIFORM_VEC3F,
		UNIFORM_VEC4F
	};
	enum uniform_matrix_type : uint8_t
	{
		UNIFORM_MAT2X2F,
		UNIFORM_MAT2X3F,
		UNIFORM_MAT2X4F,
		UNIFORM_MAT3X2F,
		UNIFORM_MAT3X3F,
		UNIFORM_MAT3X4F,
		UNIFORM_MAT4X2F,
		UNIFORM_MAT4X3F,
		UNIFORM_MAT4X4F
	};

	namespace draw_commands
	{
		struct draw_arrays
		{
			size_t num_vertices;
			mesh_handle mesh;
			shader_handle shader;
			primitive_type prim_type;
		};
		struct draw_elements
		{
			size_t num_elements;
			mesh_handle mesh;
			shader_handle shader;
			primitive_type prim_type;
		};
		struct draw_arrays_instanced
		{
			size_t num_vertices;
			size_t num_instances;
			mesh_handle mesh;
			shader_handle shader;
			primitive_type prim_type;
		};
		struct draw_elements_instanced
		{
			size_t num_elements;
			size_t num_instances;
			mesh_handle mesh;
			shader_handle shader;
			primitive_type prim_type;
		};
		struct set_shader_uniform
		{
			shader_handle shader;
			int16_t location;
			uniform_type type;
			union
			{
				bool bool_val;
				signed char byte_val;
				short short_val;
				int int_val;
				unsigned char ubyte_val;
				unsigned short ushort_val;
				unsigned int uint_val;
				texture_handle texture_val;
				glm::ivec2 vec2i_val;
				glm::ivec3 vec3i_val;
				glm::ivec4 vec4i_val;
				glm::uvec2 vec2ui_val;
				glm::uvec3 vec3ui_val;
				glm::uvec4 vec4ui_val;
				glm::fvec2 vec2f_val;
				glm::fvec3 vec3f_val;
				glm::fvec4 vec4f_val;
			};

			set_shader_uniform() = default;
			set_shader_uniform(const shader_handle shader) :
				shader(shader),
				location(-1)
			{

			}
			set_shader_uniform(const shader_handle shader, const int16_t location) :
				shader(shader),
				location(location)
			{

			}
			set_shader_uniform(const shader_handle shader, const int16_t location, const uniform_type type) :
				shader(shader),
				location(location),
				type(type)
			{

			}
			~set_shader_uniform() { }

			//Utilty macro since all the definitions are practically identical
#define RE_DEF_CONSTRUCTOR(_eval, _type, _tval) set_shader_uniform(const shader_handle shader, const int16_t location, const _type& val) : \
				shader(shader), location(location), type(uniform_type::UNIFORM_##_eval), _tval(val) { }

			RE_DEF_CONSTRUCTOR(BOOL, bool, bool_val)
			RE_DEF_CONSTRUCTOR(BYTE, signed char, byte_val)
			RE_DEF_CONSTRUCTOR(SHORT, short, short_val)
			RE_DEF_CONSTRUCTOR(INT, int, int_val)
			RE_DEF_CONSTRUCTOR(UBYTE, unsigned char, ubyte_val)
			RE_DEF_CONSTRUCTOR(USHORT, unsigned short, ushort_val)
			RE_DEF_CONSTRUCTOR(UINT, unsigned int, uint_val)
			RE_DEF_CONSTRUCTOR(TEXTURE, texture_handle, texture_val)
			RE_DEF_CONSTRUCTOR(VEC2I, glm::ivec2, vec2i_val)
			RE_DEF_CONSTRUCTOR(VEC3I, glm::ivec3, vec3i_val)
			RE_DEF_CONSTRUCTOR(VEC4I, glm::ivec4, vec4i_val)
			RE_DEF_CONSTRUCTOR(VEC2UI, glm::uvec2, vec2ui_val)
			RE_DEF_CONSTRUCTOR(VEC3UI, glm::uvec3, vec3ui_val)
			RE_DEF_CONSTRUCTOR(VEC4UI, glm::uvec4, vec4ui_val)
			RE_DEF_CONSTRUCTOR(VEC2F, glm::fvec2, vec2f_val)
			RE_DEF_CONSTRUCTOR(VEC3F, glm::fvec3, vec3f_val)
			RE_DEF_CONSTRUCTOR(VEC4F, glm::fvec4, vec4f_val)

#undef RE_DEF_CONSTRUCTOR
		};
		struct set_shader_matrix_uniform
		{
			shader_handle shader;
			int16_t location;
			uniform_matrix_type type;
			union
			{
				glm::mat2x2 mat2x2;
				glm::mat2x3 mat2x3;
				glm::mat2x4 mat2x4;
				glm::mat3x2 mat3x2;
				glm::mat3x3 mat3x3;
				glm::mat3x4 mat3x4;
				glm::mat4x2 mat4x2;
				glm::mat4x3 mat4x3;
				glm::mat4x4 mat4x4;
			};

			set_shader_matrix_uniform() = default;
			set_shader_matrix_uniform(const shader_handle shader) :
				shader(shader),
				location(-1)
			{

			}
			set_shader_matrix_uniform(const shader_handle shader, const int16_t location) :
				shader(shader),
				location(location)
			{

			}
			set_shader_matrix_uniform(const shader_handle shader, const int16_t location, const uniform_matrix_type type) :
				shader(shader),
				location(location),
				type(type)
			{

			}
			~set_shader_matrix_uniform() { }

#define RE_DEF_CONSTRUCTOR(_eval, _type) set_shader_matrix_uniform(const shader_handle shader, const int16_t location, const glm::_type& val) : \
				shader(shader), location(location), type(uniform_matrix_type::UNIFORM_##_eval), _type(val) { }
		
			RE_DEF_CONSTRUCTOR(MAT2X2F, mat2x2)
			RE_DEF_CONSTRUCTOR(MAT2X3F, mat2x3)
			RE_DEF_CONSTRUCTOR(MAT2X4F, mat2x4)
			RE_DEF_CONSTRUCTOR(MAT3X2F, mat3x2)
			RE_DEF_CONSTRUCTOR(MAT3X3F, mat3x3)
			RE_DEF_CONSTRUCTOR(MAT3X4F, mat3x4)
			RE_DEF_CONSTRUCTOR(MAT4X2F, mat4x2)
			RE_DEF_CONSTRUCTOR(MAT4X3F, mat4x3)
			RE_DEF_CONSTRUCTOR(MAT4X4F, mat4x4)

#undef RE_DEF_CONSTRUCTOR			
		};
		struct transform_feedback_arrays
		{
			draw_arrays draw_command;
			buffer_handle output_buffer;
			//This can be null to indicate that the client doesn't want to know
			unsigned* primitives_generated;
		};
		struct transform_feedback_elements
		{
			draw_elements draw_command;
			buffer_handle output_buffer;
			//This can be null to indicate that the client doesn't want to know
			unsigned* primitives_generated;
		};
	}
	
	struct draw_command
	{
		enum type : uint8_t
		{
			DRAW_ARRAYS,
			DRAW_ELEMENTS,
			DRAW_ARRAYS_INSTANCED,
			DRAW_ELEMENTS_INSTANCED,
			SET_SHADER_UNIFORM,
			SET_SHADER_MATRIX_UNIFORM,
			TRANSFORM_FEEDBACK_ARRAYS,
			TRANSFORM_FEEDBACK_ELEMENTS
		} _type;
		union
		{
			draw_commands::draw_arrays draw_arrays;
			draw_commands::draw_elements draw_elements;
			draw_commands::draw_arrays_instanced draw_arrays_instanced;
			draw_commands::draw_elements_instanced draw_elements_instanced;
			draw_commands::set_shader_uniform set_shader_uniform;
			draw_commands::set_shader_matrix_uniform set_shader_matrix_uniform;
			draw_commands::transform_feedback_arrays transform_feedback_arrays;
			draw_commands::transform_feedback_elements transform_feedback_elements;
		};

		draw_command() = default;
		draw_command(const type t) :
			_type(t)
		{

		}
		~draw_command() { }

		draw_command& operator=(const draw_command& o) = default;

#define RE_DEF_CONSTRUCTOR(vty) draw_command(const type t, const draw_commands::vty& val) : _type(t), vty(val) { }

		RE_DEF_CONSTRUCTOR(draw_arrays)
		RE_DEF_CONSTRUCTOR(draw_elements)
		RE_DEF_CONSTRUCTOR(draw_arrays_instanced)
		RE_DEF_CONSTRUCTOR(draw_elements_instanced)
		RE_DEF_CONSTRUCTOR(set_shader_uniform)
		RE_DEF_CONSTRUCTOR(set_shader_matrix_uniform)
		RE_DEF_CONSTRUCTOR(transform_feedback_arrays)
		RE_DEF_CONSTRUCTOR(transform_feedback_elements)

#undef RE_DEF_CONSTRUCTOR
	};
}
