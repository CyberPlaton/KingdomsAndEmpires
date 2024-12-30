#include "sm_shader.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	void cshader::destroy(cshader& shader)
	{
		if (is_valid(shader))
		{
			bgfx::destroy(bgfx::ShaderHandle{ shader.shader() });
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader() :
		m_handle(C_INVALID_HANDLE)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(stringview_t path)
	{
		load_from_file(path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(const char* text)
	{
		load_from_string(text);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(const uint8_t* data, unsigned size)
	{
		load_from_memory(data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::~cshader()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_file(stringview_t path)
	{
		memory_ref_t data = nullptr;

		if (!path.empty())
		{
			data = fs::load_text_from_file(path);
		}

		return load_from_memory((uint8_t*)data->data(), (unsigned)data->size());
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_string(const char* text)
	{
		return load_from_memory((const uint8_t*)text, strlen(text));
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_memory(const uint8_t* data, unsigned size)
	{
		const bgfx::Memory* mem = bgfx::makeRef(data, size);

		if (m_handle = bgfx::createShader(mem).idx; !bgfx::isValid(bgfx::ShaderHandle{ m_handle }))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading shader");
			}

			return opresult_fail;
		}

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cshader& cshader::operator=(const cshader& other)
	{
		m_handle = other.m_handle;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::set_uniform_float(stringview_t name, float value)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::set_uniform_int(stringview_t name, int value)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::set_uniform_vec2(stringview_t name, const vec2_t& value)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::set_uniform_vec3(stringview_t name, const vec3_t& value)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::set_uniform_vec4(stringview_t name, const vec4_t& value)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::set_uniform_matrix(stringview_t name, const mat4_t& value)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::set_uniform_texture(stringview_t name, const ctexture& value)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::remove_uniform(stringview_t name)
	{
		//- noop
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader_manager::cshader_manager(unsigned reserve /*= C_SHADER_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader_manager::~cshader_manager()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cshader_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader_manager::on_shutdown()
	{
		clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader_manager::on_update(float)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, stringview_t path)
	{
		return load_of_sync<shader_handle_t>(name, m_data, path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, const char* text)
	{
		return load_of_sync<shader_handle_t>(name, m_data, text);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, const uint8_t* data, unsigned size)
	{
		return load_of_sync<shader_handle_t>(name, m_data, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, stringview_t path)
	{
		return load_of_async<shader_handle_t>(name, m_data, path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, const char* text)
	{
		return load_of_async<shader_handle_t>(name, m_data, text);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, const uint8_t* data, unsigned size)
	{
		return load_of_async<shader_handle_t>(name, m_data, data, size);
	}

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cshader_manager>("cshader_manager")
	.constructor<>()
	(
		rttr::policy::ctor::as_raw_ptr
	)
	;

}