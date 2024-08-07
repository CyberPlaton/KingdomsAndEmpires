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

		project_ref_t create_project(stringview_t basepath, stringview_t name);
		bool open_project(const core::fs::cfileinfo& filepath);
		void remove_project(stringview_t name);
		void delete_project(const core::fs::cfileinfo& filepath);

		bool set_current(const core::fs::cfileinfo& filepath);
		bool set_current(stringview_t name);

		project_ref_t current() const;
		bool has() const;

	private:
		umap_t<unsigned, project_ref_t> m_projects;
		core::cmutex m_mutex;
		project_ref_t m_current;

		RTTR_ENABLE(core::cservice);
	};

} //- engine