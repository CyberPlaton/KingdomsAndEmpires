#pragma once
#include "../sm_config.hpp"

namespace sm
{
	namespace shaderc
	{
		//- Mostly integrating bgfx shaderc tool to be used by spritemancer at runtime
		//------------------------------------------------------------------------------------------------------------------------
		struct soptions
		{
			char m_shader_type;
			string_t m_platform;
			string_t m_profile;

			string_t m_input_filepath;
			string_t m_output_filepath;

			vector_t<string_t> m_include_dirs;
			vector_t<string_t> m_defines;
			vector_t<string_t> m_deps;

			bool m_disasm;
			bool m_raw;
			bool m_preprocess_only;
			bool m_depends;

			bool m_debug_information;

			bool m_avoid_flow_control;
			bool m_no_preshader;
			bool m_partial_precision;
			bool m_prefer_flow_control;
			bool m_backwards_compatibility;
			bool m_warnings_are_errors;
			bool m_keep_intermediate;

			bool m_optimize;
			uint32_t m_optimization_level;
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct suniform
		{
			string_t m_name;
			bgfx::UniformType::Enum m_type;
			uint16_t m_reg_index;
			uint16_t m_reg_count;
			uint16_t m_tex_format;
			uint8_t m_tex_component;
			uint8_t m_tex_dimension;
			uint8_t m_num;
		};

		using uniform_array_t = vector_t<suniform>;

		bool compile_glsl_shader(const soptions& options, unsigned version, const string_t& code, bx::WriterI* writer, );
		bool compile_hlsl_shader();
		bool compile_metal_shader();
		bool compile_pssl_shader();
		bool compile_spirv_shader();

	} //- shaderc

} //- sm