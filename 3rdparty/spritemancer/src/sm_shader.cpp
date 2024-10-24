#include "sm_shader.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	void cshader::destroy(cshader& shader)
	{
		if (is_valid(shader))
		{
			raylib::UnloadShader(shader.shader());
			shader.m_type = shader_type_none;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader() :
		m_shader({ 0 }), m_type(shader_type_none)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(shader_type type, stringview_t vertex_filepath, stringview_t fragment_filepath)
	{
		load_from_file(type, vertex_filepath, fragment_filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(shader_type type, const char* vs, const char* fs)
	{
		load_from_string(type, vs, fs);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(shader_type type, const uint8_t* vs, unsigned vs_size, const uint8_t* fs, unsigned fs_size)
	{
		load_from_memory(type, vs, vs_size, fs, fs_size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::~cshader()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_file(shader_type type, stringview_t vertex_filepath, stringview_t fragment_filepath)
	{
		memory_ref_t vs = nullptr, fs = nullptr;

		if (!vertex_filepath.empty())
		{
			vs = fs::load_text_from_file(vertex_filepath);
		}

		if (!fragment_filepath.empty())
		{
			fs = fs::load_text_from_file(fragment_filepath);
		}

		return load_from_memory(type, (uint8_t*)vs->data(), (unsigned)vs->size(), (uint8_t*)fs->data(), (unsigned)fs->size());
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_string(shader_type type, const char* vs, const char* fs)
	{
		return load_from_memory(type, (const uint8_t*)vs, strlen(vs), (const uint8_t*)fs, strlen(fs));
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_memory(shader_type type, const uint8_t* vs, unsigned /*vs_size*/, const uint8_t* fs, unsigned /*fs_size*/)
	{
		const char* _vs = nullptr;
		const char* _fs = nullptr;

		switch (type)
		{
		case shader_type_vertex:
		{
			_vs = (const char*)vs;
			break;
		}
		case shader_type_fragment:
		{
			_fs = (const char*)fs;
			break;
		}
		case shader_type_program:
		{
			_vs = (const char*)vs;
			_fs = (const char*)fs;
			break;
		}
		default:
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading shader, unsupported shader type!");
			}
			return opresult_fail;
		}
		}

		if (m_shader = raylib::LoadShaderFromMemory(_vs, _fs); !raylib::IsShaderReady(m_shader))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading shader");
			}

			return opresult_fail;
		}

		m_type = type;

		return opresult_ok;
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
	sm::cshader& cshader::operator=(const cshader& other)
	{
		m_shader = other.m_shader;
		m_type = other.m_type;
		return *this;
	}

} //- sm