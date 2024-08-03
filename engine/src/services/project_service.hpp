#pragma once
#include "../editor/editor_project.hpp"

namespace engine
{
	//- Service responsible for loading, unloading and managing active project.
	//------------------------------------------------------------------------------------------------------------------------
	class cproject_service final : public core::cservice
	{
	public:
		cproject_service() = default;
		~cproject_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float dt) override final;

		ref_t<editor::cproject> create(stringview_t name);
		void set(const ref_t<editor::cproject>& project);
		const editor::cproject& current() const;
		editor::cproject& current();
		bool load(stringview_t filepath);
		bool has() const;

	private:
		umap_t<unsigned, ref_t<editor::cproject>> m_projects;
		core::cmutex m_mutex;
		ref_t<editor::cproject> m_current;

		RTTR_ENABLE(core::cservice);
	};

} //- engine