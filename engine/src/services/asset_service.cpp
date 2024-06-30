#include "asset_service.hpp"
#include "../io/io_native_filesystem.hpp"
#include <plugin_logging.h>

namespace engine
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		string_t asset_filepath(stringview_t source_filepath)
		{
			return fmt::format("{}{}", source_filepath.data(), detail::C_ASSET_EXTENSION.data());
		}

	} //- unnamed

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
	asset_ref_t casset_service::open_asset(const core::fs::cfileinfo& source_filepath)
	{
		const auto path = source_filepath.path();

		if (m_assets.find(path) == m_assets.end())
		{
			return nullptr;
		}

		return m_assets.at(path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	asset_ref_t casset_service::create_asset(const core::fs::cfileinfo& filepath, rttr::type resource_type)
	{
		return create_asset(filepath, resource_type, {}, {});
	}

	//------------------------------------------------------------------------------------------------------------------------
	asset_ref_t casset_service::create_asset(const core::fs::cfileinfo& filepath, rttr::type resource_type,
		casset::asset_meta_t meta, rttr::variant options)
	{
		logging::log_debug(fmt::format("Creating asset file at '{}'", filepath.path()));

		auto* vfs = core::cservice_manager::find<core::fs::cvirtual_filesystem>();

		if (vfs->find_filesystem("/")->create_file(filepath))
		{
			if (auto file = vfs->open(filepath, core::file_mode_write | core::file_mode_truncate); file)
			{
				auto asset = std::make_shared<casset>(filepath.path(), resource_type, meta, options);

				const auto text = core::io::to_json_string(*asset, true);

				file->write((const byte_t*)text.data(), SCAST(unsigned, text.length()));

				logging::log_info(fmt::format("Successfully created asset file at '{}'", filepath.path()));

				return asset;
			}
		}

		logging::log_warn(fmt::format("Failed creating asset file at '{}'", filepath.path()));

		return nullptr;
	}

} //- engine