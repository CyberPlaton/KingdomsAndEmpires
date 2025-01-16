#include "sm_program.hpp"
#include "sm_shader.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	cprogram::cprogram(const cshader& vertex, const cshader& fragment) :
		m_vertex(vertex), m_fragment(fragment)
	{
		load_from_handles(m_vertex.shader(), m_fragment.shader());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram::cprogram() :
		m_handle(C_INVALID_HANDLE)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram::cprogram(shader_handle_t shader)
	{
		load_from_shader(shader);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram::cprogram(shader_handle_t vertex, shader_handle_t fragment)
	{
		load_from_handles(vertex, fragment);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram::~cprogram()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cprogram::load_from_shader(shader_handle_t shader)
	{
		bgfx::ProgramHandle handle{ bgfx::kInvalidHandle };

		if (handle = bgfx::createProgram(bgfx::ShaderHandle{ SCAST(uint16_t, shader) }, false); !bgfx::isValid(handle))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading program");
			}
		}
		return bgfx::isValid(handle) ? opresult_ok : opresult_fail;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cprogram::load_from_shaders(const cshader& vertex, const cshader& fragment)
	{
		m_vertex = vertex;
		m_fragment = fragment;
		return load_from_handles(m_vertex.shader(), m_fragment.shader());
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cprogram::load_from_handles(shader_handle_t vertex, shader_handle_t fragment)
	{
		if (m_handle = bgfx::createProgram(bgfx::ShaderHandle{ SCAST(uint16_t, vertex) }, bgfx::ShaderHandle{ SCAST(uint16_t, fragment) }).idx;
			!bgfx::isValid(bgfx::ProgramHandle{ SCAST(uint16_t, m_handle) }))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading program");
			}

			return opresult_fail;
		}
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram_manager::cprogram_manager(unsigned reserve /*= C_PROGRAM_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram_manager::~cprogram_manager()
	{
		clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cprogram_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprogram_manager::on_shutdown()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprogram_manager::on_update(float)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::program_handle_t cprogram_manager::load_sync(stringview_t name, shader_handle_t vs, shader_handle_t fs)
	{
		auto* sm = core::cservice_manager::find<cshader_manager>();

		return load_of_sync<program_handle_t>(name.data(), sm->at(vs).shader(), sm->at(fs).shader());
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::program_handle_t cprogram_manager::load_sync(stringview_t name, const cshader& vs, const cshader& fs)
	{
		return load_of_sync<program_handle_t>(name.data(), vs.shader(), fs.shader());
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::program_handle_t cprogram_manager::load_sync(stringview_t name, shader_handle_t shader)
	{
		auto* sm = core::cservice_manager::find<cshader_manager>();

		return load_of_sync<program_handle_t>(name.data(), sm->at(shader).shader());
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::program_handle_t> cprogram_manager::load_async(stringview_t name, shader_handle_t vs, shader_handle_t fs)
	{
		auto* sm = core::cservice_manager::find<cshader_manager>();

		return load_of_async<program_handle_t>(name.data(), sm->at(vs).shader(), sm->at(fs).shader());
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::program_handle_t> cprogram_manager::load_async(stringview_t name, const cshader& vs, const cshader& fs)
	{
		return load_of_async<program_handle_t>(name.data(), vs.shader(), fs.shader());
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::program_handle_t> cprogram_manager::load_async(stringview_t name, shader_handle_t shader)
	{
		auto* sm = core::cservice_manager::find<cshader_manager>();

		return load_of_async<program_handle_t>(name.data(), sm->at(shader).shader());
	}

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cprogram_manager>("cprogram_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}