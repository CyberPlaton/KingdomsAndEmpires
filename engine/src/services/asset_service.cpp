#include "asset_service.hpp"
#include "../io/io_native_filesystem.hpp"

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	casset_service::~casset_service()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool casset_service::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void casset_service::on_shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void casset_service::on_update(float)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool casset_service::compile(stringview_t source_filepath)
	{
		//- load the asset of source file, fail if it does not exist, meaning source file was not imported
		auto asset = load_asset(source_filepath);

		//- create the compiler class instance for compiling, if there is a valid one registered
// 		if (const auto compiler_type = rttr::type::get_by_name(asset.compiler_type().data()); compiler_type.is_valid())
// 		{
// 			const auto compiler = compiler_type.create();
// 
// 			if (const auto [data, size] = core::cfile::load_binary(source_filepath.data()); data)
// 			{
// 				auto ref = core::cmemory::make_ref((void*)data, size, core::cfile::unload_binary);
// 
// 				//- try to compile source data, if there is a registered function
// 				if (const auto method = compiler_type.get_method("compile"); method.is_valid())
// 				{
// 					if (const auto [result, compiled_data] = method.invoke(compiler, ref, asset.compiling_options()).convert<iresource_compiler::compile_result_t>(); result)
// 					{
// 						//- finally export resulting data to destination path along with the asset file
// 						auto destination_path = compiled_output_path(source_filepath);
// 
// 						auto status_compiled = core::cfile::save_binary(destination_path.data(), (uint8_t*)compiled_data->data(), compiled_data->size());
// 						auto status_asset = core::cfile::save_text("TODO", core::io::to_json_string(asset));
// 
// 						return status_compiled == core::file_io_status_success && status_asset == core::file_io_status_success;
// 					}
// 				}
// 			}
// 		}

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t casset_service::compiled_output_path(stringview_t source_filepath) const
	{
		//- find asset file for source, this is required only to get the final extension of the compiled resource
		if (core::fs::cfileinfo asset_info(fmt::format("{}.asset", source_filepath.data())); asset_info.exists())
		{
			auto* vfs = core::cservice_manager::find<core::fs::cvirtual_filesystem>();

			if (const auto asset_file = vfs->open(asset_info, core::file_mode_read); asset_file)
			{
				const auto file_size = asset_file->size();
				const auto blob = core::cmemory::make_ref(file_size);

				if (asset_file->read(blob->data(), file_size) == file_size)
				{
					const auto asset = core::io::from_json_blob<casset>(blob->data(), blob->size());
				}
			}
		}


		core::fs::cfileinfo info(source_filepath);

		const auto guid = info.name();

	}

	//------------------------------------------------------------------------------------------------------------------------
	asset_ref_t casset_service::open_asset(stringview_t source_filepath)
	{
		const auto asset_path = fmt::format("{}{}", source_filepath.data(), detail::C_ASSET_EXTENSION.data());

		if (m_assets.find(source_filepath) == m_assets.end())
		{
			//- check if asset file does not exist and we have to create it first
			core::fs::cfileinfo info(asset_path);

			//- create asset file if it does not exist
			const auto ext = info.extension().c_str();

			//- asset file with default compiler settings registered for given extension

			if (!info.exists() && !create_asset(info))
			{
				return nullptr;
			}
		}

		return m_assets.at(source_filepath);


		if (core::fs::cfileinfo info(asset_path); info.exists())
		{
			auto* vfs = core::cservice_manager::find<core::fs::cvirtual_filesystem>();

			if (const auto asset_file = vfs->open(info, core::file_mode_read); asset_file)
			{
				const auto file_size = asset_file->size();
				const auto blob = core::cmemory::make_ref(file_size);

				if (asset_file->read(blob->data(), file_size) == file_size)
				{
					const auto asset = core::io::from_json_blob<casset>(blob->data(), blob->size());
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	asset_ref_t casset_service::create_asset(const core::fs::cfileinfo& filepath)
	{
		auto* vfs = core::cservice_manager::find<core::fs::cvirtual_filesystem>();

		if (vfs->find_filesystem("/")->create_file(filepath))
		{
			if (auto file = vfs->open(filepath, core::file_mode_write | core::file_mode_truncate); file)
			{
				casset asset;

				file->write<casset>(asset);
			}
		}

		return nullptr;
	}

} //- engine