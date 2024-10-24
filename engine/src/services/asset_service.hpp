#pragma once
#include "../assets/asset.hpp"

namespace engine
{
	using asset_ref_t = ref_t<casset>;

	//- Service responsible for kicking off compilation of a source resource into game/engine-ready format, creating,
	//- deleting and moving asset files, computing final destination location for any given source file,
	//- managing an asset/resource database along with cross-references and dependencies.
	//------------------------------------------------------------------------------------------------------------------------
	class casset_service final : public core::cservice
	{
	public:
		struct sconfig
		{
			fs::cfileinfo m_compiled_output_path;
		};

		casset_service() = default;
		~casset_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		asset_ref_t open_asset(const fs::cfileinfo& source_filepath);

		asset_ref_t create_asset(const fs::cfileinfo& filepath, rttr::type resource_type);
		asset_ref_t create_asset(const fs::cfileinfo& filepath, rttr::type resource_type, casset::asset_meta_t meta, rttr::variant options);

	private:
		//- mapping source resource paths to asset files
		umap_t<stringview_t, asset_ref_t> m_assets;

		RTTR_ENABLE(core::cservice);
	};

} //- engine