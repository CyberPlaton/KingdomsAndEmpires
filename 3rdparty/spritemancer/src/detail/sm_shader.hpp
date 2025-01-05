#pragma once
#include "sm_config.hpp"
#include "sm_shader_compiler.hpp"

namespace sm
{
	//- A shader, a single vertex or fragment, or compute etc. shader.
	//- TODO: As of now, we can only load embedded shader types compiled externally. Each function to load a shader expects
	//- that exactly format.
	//------------------------------------------------------------------------------------------------------------------------
	class cshader final : public core::cresource
	{
	public:
		explicit cshader(stringview_t filepath);
		explicit cshader(const char* string);
		explicit cshader(const uint8_t* data, unsigned size);
		cshader();
		~cshader();

		opresult load_from_file(stringview_t filepath);
		opresult load_from_string(const char* string);
		opresult load_from_memory(const uint8_t* data, unsigned size);

		inline shader_handle_t shader() const { return m_handle; }

		void set_uniform_float(stringview_t name, float value);
		void set_uniform_int(stringview_t name, int value);
		void set_uniform_vec2(stringview_t name, const vec2_t& value);
		void set_uniform_vec3(stringview_t name, const vec3_t& value);
		void set_uniform_vec4(stringview_t name, const vec4_t& value);
		void set_uniform_matrix(stringview_t name, const mat4_t& value);
		void set_uniform_texture(stringview_t name, const ctexture& value);
		void remove_uniform(stringview_t name);

		cshader& operator=(const cshader& other);

		operator bgfx::ShaderHandle() const noexcept { return bgfx::ShaderHandle{ m_handle }; }

	private:
		shader_handle_t m_handle;

		RTTR_ENABLE(core::cresource);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cshader_manager final :
		public core::cservice,
		public core::cresource_manager<cshader>
	{
	public:
		cshader_manager(unsigned reserve = C_SHADER_RESOURCE_MANAGER_RESERVE_COUNT);
		~cshader_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		shader_handle_t load_sync(stringview_t name, stringview_t path);
		shader_handle_t load_sync(stringview_t name, const char* text);
		shader_handle_t load_sync(stringview_t name, const uint8_t* data, unsigned size);

		core::cfuture_type<shader_handle_t> load_async(stringview_t name, stringview_t path);
		core::cfuture_type<shader_handle_t> load_async(stringview_t name, const char* text);
		core::cfuture_type<shader_handle_t> load_async(stringview_t name, const uint8_t* data, unsigned size);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cshader>);
	};

} //- sm