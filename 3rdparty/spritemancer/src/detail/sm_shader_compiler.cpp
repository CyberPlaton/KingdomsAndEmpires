#include "sm_shader_compiler.hpp"
#include <../bgfx/tools/shaderc/shaderc.h>

namespace sm::shaderc
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		string_t to_bgfx_platform(stringview_t p)
		{
			if (string_utils::compare(p.data(), "Windows") ||
				string_utils::compare(p.data(), "Xbox One") ||
				string_utils::compare(p.data(), "Xbox Series"))
			{
				return "windows";
			}
			else if (string_utils::compare(p.data(), "Linux"))
			{
				return "linux";
			}
			else if (string_utils::compare(p.data(), "NX"))
			{
				return {};
			}
			else if (string_utils::compare(p.data(), "Android"))
			{
				return "android";
			}
			else if (string_utils::compare(p.data(), "iOS"))
			{
				return "ios";
			}
			else if (string_utils::compare(p.data(), "macOS"))
			{
				return "osx";
			}
			else if (string_utils::compare(p.data(), "PlayStation 4") ||
				string_utils::compare(p.data(), " PlayStation 5"))
			{
				return "orbis";
			}
			return {};
		}

		//------------------------------------------------------------------------------------------------------------------------
		bgfx::Options to_bgfx_options(const soptions& options)
		{
			bgfx::Options out;

			switch (options.m_type)
			{
			default:
			case soptions::shader_type_none: return {};
			case soptions::shader_type_vertex:
			{
				out.shaderType = 'v';
				break;
			}
			case soptions::shader_type_pixel:
			{
				out.shaderType = 'f';
				break;
			}
			case soptions::shader_type_compute:
			{
				out.shaderType = 'c';
				break;
			}
			}

			out.platform = options.m_platform;
			out.profile = options.m_profile;
			out.debugInformation = algorithm::bit_check(options.m_flags, soptions::flag_debug);
			out.avoidFlowControl = algorithm::bit_check(options.m_flags, soptions::flag_avoid_flow_control);
			out.noPreshader = algorithm::bit_check(options.m_flags, soptions::flag_no_preshader);
			out.partialPrecision = algorithm::bit_check(options.m_flags, soptions::flag_partial_precision);
			out.preferFlowControl = algorithm::bit_check(options.m_flags, soptions::flag_prefer_flow_control);
			out.backwardsCompatibility = algorithm::bit_check(options.m_flags, soptions::flag_backward_compatible);
			out.warningsAreErrors = algorithm::bit_check(options.m_flags, soptions::flag_warnings_are_errors);
			out.keepIntermediate = algorithm::bit_check(options.m_flags, soptions::flag_keep_intermediate);
			out.optimize = options.m_optimization != soptions::optimization_level_none;
			out.optimizationLevel = (unsigned)options.m_optimization;

			for (const auto& include : options.m_include_directories)
			{
				out.includeDirs.emplace_back(include.data());
			}
			for (const auto& define : options.m_defines)
			{
				out.defines.emplace_back(define.data());
			}
			for (const auto& dep : options.m_dependencies)
			{
				out.dependencies.emplace_back(dep.data());
			}

			return out;
		}

	} //- unnamed

	//- Note: dynamic and integrated shader compiler for supported platforms and profiles.
	//- Heavily inspired by bgfx shaderc tool.
	//- Also note that to compile a shader for a platform, you have to be on that platform, e.g. you cannot
	//- compile for Windows while running on Linux, as you would miss required libraries and headers etc.
	//------------------------------------------------------------------------------------------------------------------------
	memory_ref_t compile(stringview_t code, const soptions& options)
	{
		CORE_ZONE;

		log_debug(fmt::format("Compiling shader '{}', spritemancer version '{}.{}.{}'",
			options.m_name,
			sm::sinfo::C_VERSION_MAJOR,
			sm::sinfo::C_VERSION_MINOR,
			sm::sinfo::C_VERSION_PATCH));

		auto bgfx_options = to_bgfx_options(options);
		bgfx_options.platform = to_bgfx_platform(options.m_platform);

		log_debug(fmt::format("\tplatform: '{}'", bgfx_options.platform));
		log_debug(fmt::format("\tprofile: '{}'", bgfx_options.profile));

		log_debug(fmt::format("\tinclude dirs: '{}'",
			string_utils::join(bgfx_options.includeDirs.begin(), bgfx_options.includeDirs.end())));

		log_debug(fmt::format("\tdefines: '{}'",
			string_utils::join(bgfx_options.defines.begin(), bgfx_options.defines.end())));

		log_debug(fmt::format("\tdependencies: '{}'",
			string_utils::join(bgfx_options.dependencies.begin(), bgfx_options.dependencies.end())));

		if (string_utils::compare(options.m_platform, "Windows") ||
			string_utils::compare(options.m_platform, "Xbox One") ||
			string_utils::compare(options.m_platform, "Xbox Series"))
		{
		}
		else if (string_utils::compare(options.m_platform, "Linux") ||
			string_utils::compare(options.m_platform, "Android") ||
			string_utils::compare(options.m_platform, "NX"))
		{ 
		}
		else if (string_utils::compare(options.m_platform, "iOS") ||
			string_utils::compare(options.m_platform, "macOS"))
		{
		}
		else if (string_utils::compare(options.m_platform, "PlayStation 4") ||
			string_utils::compare(options.m_platform, " PlayStation 5"))
		{
		}
		else
		{
			log_error(fmt::format("Failed to compile shader due to unsupported platform '{}'",
				options.m_platform));
		}

		//- Save code to a temporary file for compilation as direct compiling from string is not supported
		string_t temp_input_file_path;
		{
			core::cfilesystem::remove(temp_input_file_path.data());

			auto vfs = core::cservice_manager::find<fs::cvirtual_filesystem>()->find_filesystem("/temp/");
			temp_input_file_path = fmt::format("{}/{}.sc", vfs->base_path(), options.m_name);

			if (const auto result = fs::save_text_to_file(temp_input_file_path, core::cmemory::make_ref((byte_t*)code.data(), code.size()));
				result != core::file_io_status_success)
			{
				log_error(fmt::format("Failed creating temporary file for shader compilation '{}'", temp_input_file_path));
				return {};
			}
		}

		entry::clogwriter logwriter;
		entry::cstringwriter stringwriter;
		memory_ref_t memory{};

		bgfx_options.inputFilePath = temp_input_file_path;

		if (bgfx::compileShader(options.m_varying.data(), nullptr, (char*)code.data(), code.size(), false, bgfx_options, &stringwriter, &logwriter))
		{
			string_t shader(stringwriter.take());

			memory = core::cmemory::make_ref((byte_t*)shader.c_str(), shader.size());
		}

		core::cfilesystem::remove(temp_input_file_path.data());

		return memory;
	}

} //- sm::shaderc