#include "sm_program.hpp"
#include "sm_shader.hpp"

namespace sm
{
	namespace resource
	{
		//------------------------------------------------------------------------------------------------------------------------
		const core::resource::iresource* cprogram_manager::load(stringview_t name, const fs::cfileinfo& path)
		{
			if (const auto* text = fs::load_text_file_data(path.path()); text)
			{
				if (const auto var = core::io::from_json_string(rttr::type::get<detail::sprogram>(), text); var.is_valid())
				{
					cprogram program;

					program.m_resource = var.get_value<detail::sprogram>();

					//- Proceed loading shaders
					auto* sm = core::cservice_manager::find<cshader_manager>();

					//- Either create from vertex/pixel shaders or from computpe shader
					if (!program.m_resource.m_compute_shader_path.empty())
					{
						const cshader* compute = nullptr;
						if (fs::cfileinfo info(program.m_resource.m_compute_shader_path); info.exists())
						{
							if (compute = sm->load_sync(info.stem(), info); compute)
							{
								if (program.m_resource.m_handle = bgfx::createProgram(compute->m_resource.m_handle); bgfx::isValid(program.m_resource.m_handle))
								{
									if (serror_reporter::instance().m_callback)
									{
										serror_reporter::instance().m_callback(core::logging_verbosity_trace,
											fmt::format("Successfully loaded compute program '{}' at '{}'", name, path.path()));
									}

									return emplace_object(name, std::move(program));
								}
							}
						}
					}
					else
					{
						const cshader* vertex = nullptr;
						const cshader* pixel = nullptr;

						fs::cfileinfo vertex_info(program.m_resource.m_vertex_shader_path);
						fs::cfileinfo pixel_info(program.m_resource.m_pixel_shader_path);

						if (vertex_info.exists() && pixel_info.exists())
						{
							vertex = sm->load_sync(vertex_info.stem(), vertex_info);
							pixel = sm->load_sync(pixel_info.stem(), pixel_info);

							if (program.m_resource.m_handle = bgfx::createProgram(vertex->m_resource.m_handle, pixel->m_resource.m_handle);
								bgfx::isValid(program.m_resource.m_handle))
							{
								if (serror_reporter::instance().m_callback)
								{
									serror_reporter::instance().m_callback(core::logging_verbosity_trace,
										fmt::format("Successfully loaded program '{}' at '{}'", name, path.path()));
								}

								return emplace_object(name, std::move(program));
							}
						}
					}
				}
			}

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					fmt::format("Failed loading program '{}' at '{}'", name, path.path()));
			}

			return nullptr;
		}

	} //- resource

} //- sm

RTTR_REGISTRATION
{
	using namespace sm::resource;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cprogram_manager>("cprogram_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}