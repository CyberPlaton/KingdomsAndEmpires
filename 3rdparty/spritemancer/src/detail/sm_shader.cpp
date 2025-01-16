#include "sm_shader.hpp"

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		memory_ref_t compile_shader_string(stringview_t name, shader_type type, const char* text)
		{
			const auto& vfs_shaders = core::cservice_manager::find<fs::cvirtual_filesystem>()->find_filesystem("/shaders");

			sm::shaderc::soptions options;
			options.m_name = name;

			switch (type)
			{
			case shader_type_vertex:
				options.m_type = sm::shaderc::soptions::shader_type_vertex;
				break;
			case shader_type_pixel:
				options.m_type = sm::shaderc::soptions::shader_type_pixel;
				break;
			case shader_type_compute:
				options.m_type = sm::shaderc::soptions::shader_type_compute;
				break;
			default:
				return {};
			}

			options.m_include_directories.emplace_back(vfs_shaders->base_path());

			return sm::shaderc::compile(text, options);
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader() :
		m_handle(C_INVALID_HANDLE)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(stringview_t name, shader_type type, stringview_t path) :
		m_shader_name(name)
	{
		load_from_file(m_shader_name, type, path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(stringview_t name, shader_type type, const char* text) :
		m_shader_name(name)
	{
		load_from_string(m_shader_name, type, text);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(stringview_t name, shader_type type, const uint8_t* data, unsigned size) :
		m_shader_name(name)
	{
		load_from_memory(m_shader_name, type, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::~cshader()
	{
		if (is_valid(*this))
		{
			bgfx::destroy(bgfx::ShaderHandle{ SCAST(uint16_t, shader()) });
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_file(stringview_t name, shader_type type, stringview_t path)
	{
		memory_ref_t data = nullptr;

		if (!path.empty())
		{
			data = fs::load_text_from_file(path);
		}

		return load_from_string(name, type, (const char*)data->data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_string(stringview_t name, shader_type type, const char* text)
	{
		if (const auto mem = compile_shader_string(name, type, text); mem && !mem->empty())
		{
			return load_from_memory(name, type, (uint8_t*)mem->data(), mem->size());
		}

		if (serror_reporter::instance().m_callback)
		{
			serror_reporter::instance().m_callback(core::logging_verbosity_error,
				fmt::format("Failed compiling and loading shader '{}'", name));
		}
		return opresult_fail;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_memory(stringview_t name, shader_type type, const uint8_t* data, unsigned size)
	{
		//- Create own copy of the blob to ensure it won´t go out of scope.
		m_shader_blob = core::cmemory::make_ref((byte_t*)data, size);
		m_type = type;

		const bgfx::Memory* mem = bgfx::makeRef(m_shader_blob->data(), m_shader_blob->size());

		if (m_handle = bgfx::createShader(mem).idx; !bgfx::isValid(bgfx::ShaderHandle{ SCAST(uint16_t, m_handle) }))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					fmt::format("Failed loading shader '{}' from memory data ", name));
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
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, shader_type type, stringview_t path)
	{
		return load_of_sync<shader_handle_t>(name.data(), name, type, path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, shader_type type, const char* text)
	{
		return load_of_sync<shader_handle_t>(name.data(), name, type, text);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, shader_type type, const uint8_t* data, unsigned size)
	{
		return load_of_sync<shader_handle_t>(name.data(), name, type, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, shader_type type, stringview_t path)
	{
		return load_of_async<shader_handle_t>(name.data(), name, type, path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, shader_type type, const char* text)
	{
		return load_of_async<shader_handle_t>(name.data(), name, type, text);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, shader_type type, const uint8_t* data, unsigned size)
	{
		return load_of_async<shader_handle_t>(name.data(), name, type, data, size);
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