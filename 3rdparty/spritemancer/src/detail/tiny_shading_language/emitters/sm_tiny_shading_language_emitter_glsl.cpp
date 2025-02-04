#include "sm_tiny_shading_language_emitter_glsl.hpp"

namespace sm
{
	namespace tsl
	{
		namespace
		{
		} //- unnamed

		//------------------------------------------------------------------------------------------------------------------------
		string_t ctiny_shader_emitter_glsl::emit_function_name(const string_t& func)
		{
			return func;
		}

		//------------------------------------------------------------------------------------------------------------------------
		string_t ctiny_shader_emitter_glsl::emit_data_type(token_type token)
		{
			switch (token)
			{
			default: return {};
			case token_type_void: return "void";
			case token_type_bool: return "bool";
			case token_type_int: return "int";
			case token_type_uint: return "uint";
			case token_type_float: return "float";
			case token_type_double: return "double";
			case token_type_bvec2: return "bvec2";
			case token_type_bvec3: return "bvec3";
			case token_type_bvec4: return "bvec4";
			case token_type_ivec2: return "ivec2";
			case token_type_ivec3: return "ivec3";
			case token_type_ivec4: return "ivec4";
			case token_type_uvec2: return "uvec2";
			case token_type_uvec3: return "uvec3";
			case token_type_uvec4: return "uvec4";
			case token_type_dvec2: return "dvec2";
			case token_type_dvec3: return "dvec3";
			case token_type_dvec4: return "dvec4";
			case token_type_vec2: return "vec2";
			case token_type_vec3: return "vec3";
			case token_type_vec4: return "vec4";
			case token_type_mat2: return "mat2";
			case token_type_mat3: return "mat3";
			case token_type_mat4: return "mat4";
			case token_type_sampler1D: return "sampler1D";
			case token_type_sampler2D: return "sampler2D";
			}
		}

	} //- tsl

} //- sm