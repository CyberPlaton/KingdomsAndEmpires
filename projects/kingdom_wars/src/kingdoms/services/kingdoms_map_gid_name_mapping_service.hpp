#pragma once
#include <core.h>

namespace kingdoms
{
	//- Responsible for mapping global identifiers of a map to specific texture or subtexture names.
	//------------------------------------------------------------------------------------------------------------------------
	class cmap_gid_name_mapping_service final : public core::cservice
	{
	public:
		cmap_gid_name_mapping_service() = default;
		~cmap_gid_name_mapping_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float dt) override final;

		cmap_gid_name_mapping_service& map(stringview_t mapname, int gid, stringview_t texturename);

		const umap_t<int, stringview_t>& map_names(stringview_t mapname) const;
		stringview_t texture_name(stringview_t mapname, int gid) const;

	private:
		umap_t<stringview_t, umap_t<int, stringview_t>> m_mappings;

		RTTR_ENABLE(core::cservice);
	};

} //- kingdoms
