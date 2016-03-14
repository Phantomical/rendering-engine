#pragma once

#include "drawstate.h"

#include "glm\glm.hpp"

#include <vector>

namespace gldr
{
	enum class primitive_type : uint8_t
	{
		TRIANGLES,
		TRIANGLE_STRIP,
		POINTS,
		LINES
	};
	enum class uniform_type : uint8_t
	{
		BOOL,
		BYTE,
		SHORT,
		INT,
		UBYTE,
		USHORT,
		UINT,
		TEXTURE,
		VEC2I,
		VEC3I,
		VEC4I,
		VEC2UI,
		VEC3UI,
		VEC4UI,
		VEC2F,
		VEC3F,
		VEC4F
	};
	enum class uniform_matrix_type : uint8_t
	{
		MAT2X2F,
		MAT2X3F,
		MAT2X4F,
		MAT3X2F,
		MAT3X3F,
		MAT3X4F,
		MAT4X2F,
		MAT4X3F,
		MAT4X4F
	};

	namespace _1
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
				shader(shader), location(location), type(uniform_type::_eval), _tval(val) { }

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
				shader(shader), location(location), type(uniform_matrix_type::_eval), _type(val) { }
		
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
	}
}
