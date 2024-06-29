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
			core::fs::cfileinfo m_compiled_output_path;
		};

		casset_service() = default;
		~casset_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		bool compile(stringview_t source_filepath);

		stringview_t compiled_output_path(stringview_t source_filepath) const;

		asset_ref_t open_asset(stringview_t source_filepath);

		asset_ref_t create_asset(const core::fs::cfileinfo& filepath);

	private:
		umap_t<stringview_t, asset_ref_t> m_assets;

		RTTR_ENABLE(core::cservice);
	};

} //- engine

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(casset_service)
	{
		rttr::cregistrator<casset_service, rttr::detail::as_raw_pointer>("casset_service")
			;
	};

} //- engine