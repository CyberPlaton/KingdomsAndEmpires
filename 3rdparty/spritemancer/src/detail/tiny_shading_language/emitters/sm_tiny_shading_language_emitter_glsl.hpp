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
			string_t emit_function_name(const string_t&) override final { return {}; };
			string_t emit_data_type(token_type) override final { return {}; };
		};

	} //- tsl

} //- sm