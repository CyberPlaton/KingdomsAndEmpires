#pragma once
#include "sm_config.hpp"
#include "sm_shader_compiler.hpp"

namespace sm
{
	//- A shader, a single vertex or fragment, or compute etc. shader.
	//------------------------------------------------------------------------------------------------------------------------
	class cshader final : public core::cresource
	{
	public:
		explicit cshader(stringview_t name, shader_type type, stringview_t filepath);
		explicit cshader(stringview_t name, shader_type type, const char* string);
		explicit cshader(stringview_t name, shader_type type, const uint8_t* data, unsigned size);
		cshader();
		~cshader();

		opresult load_from_file(stringview_t name, shader_type type, stringview_t filepath);
		opresult load_from_string(stringview_t name, shader_type type, const char* string);

		//- Note: we expect a compiled shader. Otherwise it will not be loaded.
		opresult load_from_memory(stringview_t name, shader_type type, const uint8_t* data, unsigned size);

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

		operator bgfx::ShaderHandle() const noexcept { return bgfx::ShaderHandle{ SCAST(uint16_t, m_handle) }; }

	private:
		stringview_t m_shader_name;
		memory_ref_t m_shader_blob;
		shader_handle_t m_handle;
		shader_type m_type;

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

		shader_handle_t load_sync(stringview_t name, shader_type type, stringview_t path);
		shader_handle_t load_sync(stringview_t name, shader_type type, const char* text);
		shader_handle_t load_sync(stringview_t name, shader_type type, const uint8_t* data, unsigned size);

		core::cfuture_type<shader_handle_t> load_async(stringview_t name, shader_type type, stringview_t path);
		core::cfuture_type<shader_handle_t> load_async(stringview_t name, shader_type type, const char* text);
		core::cfuture_type<shader_handle_t> load_async(stringview_t name, shader_type type, const uint8_t* data, unsigned size);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cshader>);
	};

} //- sm