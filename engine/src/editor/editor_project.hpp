#pragma once
#include <core.h>

namespace editor
{
	//- Class defining a project. It defines the base path for resources and other settings special for this project.
	//- It is also very important on startup, where we have a startup project the engine actually launches.
	//------------------------------------------------------------------------------------------------------------------------
	class cproject final
	{
	public:
		struct sconfig
		{
			string_t m_basepath;
			string_t m_project_name;

			RTTR_ENABLE();
		};

		cproject(stringview_t filepath);
		cproject(sconfig&& cfg);
		~cproject();

		inline const sconfig& config() const { return m_cfg; }

	private:
		sconfig m_cfg;

		RTTR_ENABLE();
		RTTR_REGISTRATION_FRIEND;
	};

} //- editor