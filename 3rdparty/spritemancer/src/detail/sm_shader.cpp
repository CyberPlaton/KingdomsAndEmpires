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

	namespace resource
	{
		//------------------------------------------------------------------------------------------------------------------------
		const core::resource::iresource* cshader_manager::load(stringview_t name, const fs::cfileinfo& path)
		{
			if (path.exists())
			{
				//- We have to discern whether we are loading a compiled shader or a raw shader text file
				const auto ext = path.extension().generic_u8string();

				if (string_utils::compare(ext, ".ccs") ||
					string_utils::compare(ext, ".cps") ||
					string_utils::compare(ext, ".cvs"))
				{
					unsigned size = 0;
					byte_t* data = nullptr;

					if (data = fs::load_binary_file_data(path.path(), &size); data && size > 0)
					{
						const auto* mem = bgfx::makeRef(data, size);

						if (const auto handle = bgfx::createShader(mem); bgfx::isValid(handle))
						{
							cshader shader;
							shader.m_resource.m_name = name;
							shader.m_resource.m_blob = core::cmemory::make_ref(data, size);
							shader.m_resource.m_handle = handle;
							shader.m_resource.m_type = string_utils::compare(ext, ".ccs") ? shader_type_compute :
								(string_utils::compare(ext, ".cps") ? shader_type_pixel : shader_type_vertex);

							if (serror_reporter::instance().m_callback)
							{
								serror_reporter::instance().m_callback(core::logging_verbosity_trace,
									fmt::format("Successfully loaded shader '{}' at '{}'", name, path.path()));
							}

							return emplace_object(name, std::move(shader));
						}
					}
				}
				else
				{
					if (const auto* text = fs::load_text_file_data(path.path()); text)
					{
						const auto shader_type = string_utils::compare(ext, ".cs") ? shader_type_compute :
							(string_utils::compare(ext, ".ps") ? shader_type_pixel : shader_type_vertex);

						if (const auto blob = compile_shader_string(name, shader_type, text); blob && !blob->empty())
						{
							const auto* mem = bgfx::makeRef(blob->data(), blob->size());

							if (const auto handle = bgfx::createShader(mem); bgfx::isValid(handle))
							{
								cshader shader;
								shader.m_resource.m_name = name;
								shader.m_resource.m_blob = std::move(blob);
								shader.m_resource.m_handle = handle;
								shader.m_resource.m_type = shader_type;

								if (serror_reporter::instance().m_callback)
								{
									serror_reporter::instance().m_callback(core::logging_verbosity_trace,
										fmt::format("Successfully loaded shader '{}' at '{}'", name, path.path()));
								}

								return emplace_object(name, std::move(shader));
							}
						}
					}
				}
			}

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					fmt::format("Failed loading shader '{}' at '{}'", name, path.path()));
			}

			return nullptr;
		}

	} //- resource

} //- sm

RTTR_REGISTRATION
{
	using namespace sm::resource;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cshader_manager>("cshader_manager")
	.constructor<>()
	(
		rttr::policy::ctor::as_raw_ptr
	)
	;

}