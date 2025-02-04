#pragma once
#include "../sm_tiny_shading_language_backend.hpp"

namespace sm
{
	namespace tsl
	{
		//------------------------------------------------------------------------------------------------------------------------
		class ctiny_shader_emitter_glsl final : public itiny_shader_emitter
		{
		public:
			stringview_t emitter_name() const override final { static constexpr auto C_NAME = "glsl"; return C_NAME; }

			string_t emit_function_name(const string_t& func) override final;;
			string_t emit_data_type(token_type token) override final;;
		};

	} //- tsl

} //- sm