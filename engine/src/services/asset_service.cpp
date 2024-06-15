#include "asset_service.hpp"

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
		if (const auto compiler_type = rttr::type::get_by_name(asset.compiler_type().data()); compiler_type.is_valid())
		{
			const auto compiler = compiler_type.create();

			if (const auto [data, size] = core::cfile::load_binary(source_filepath.data()); data)
			{
				auto ref = core::cmemory::make_ref((void*)data, size, core::cfile::unload_binary);

				//- try to compile source data, if there is a registered function
				if (const auto method = compiler_type.get_method("compile"); method.is_valid())
				{
					if (const auto [result, compiled_data] = method.invoke(compiler, ref, asset.compiling_options()).convert<iresource_compiler::compile_result_t>(); result)
					{
						//- finally export resulting data to destination path along with the asset file
						auto destination_path = compiled_output_path(source_filepath);

						auto status_compiled = core::cfile::save_binary(destination_path.data(), (uint8_t*)compiled_data->data(), compiled_data->size());
						auto status_asset = core::cfile::save_text("TODO", core::io::to_json_string(asset));

						return status_compiled == core::file_io_status_success && status_asset == core::file_io_status_success;
					}
				}
			}
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t casset_service::compiled_output_path(stringview_t source_filepath) const
	{
		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	engine::casset casset_service::load_asset(stringview_t source_filepath)
	{
		return {};
	}

} //- engine