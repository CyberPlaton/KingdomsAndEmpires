#pragma once
#include "../editor/editor_project.hpp"

namespace engine
{
	//- Class responsible for project management. It can create and delete, open and remove projects and is the only access
	//- to current active project in applicaiton.
	//------------------------------------------------------------------------------------------------------------------------
	class cproject_service final : public core::cservice
	{
	public:
		cproject_service() = default;
		~cproject_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float dt) override final;

		//- Create new project file at given directory with given name
		project_ref_t create_project(stringview_t basepath, stringview_t name);

		//- Load project from file to project service
		bool open_project(const core::fs::cfileinfo& filepath);

		//- Unload project from project service
		void remove_project(stringview_t name);

		//- Delete project file and associated directory
		void delete_project(const core::fs::cfileinfo& filepath);

		//- Currently, we basically set ref to new project without any loading of
		//- resources or plugins etc.
		bool set_current(const core::fs::cfileinfo& filepath);
		bool set_current(const project_ref_t& project);
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