#pragma once
#include <core.h>

namespace editor
{
	//- Class defining a project. It defines the base path for resources and other settings special for this project.
	//- It is also very important on startup, where we have a startup project the engine actually launches.
	//------------------------------------------------------------------------------------------------------------------------
	struct sproject final
	{
		struct sconfig
		{
			//- Plugins etc. specific to the project
			vector_t<string_t> m_project_plugins;
			string_t m_basepath;
			string_t m_project_name;
			string_t m_startup_world;

			RTTR_ENABLE()
		};

		sconfig m_cfg;

		RTTR_ENABLE()
	};

} //- editor

using project_ref_t = ref_t<editor::sproject>;