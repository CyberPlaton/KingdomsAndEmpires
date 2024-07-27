#pragma once
#include <core.h>

namespace io
{
	//- A module defines directories of assets to be loaded when this module is loaded. Further assets can be grouped
	//- inside the module.
	//------------------------------------------------------------------------------------------------------------------------
	class cmodule final : public core::cresource
	{
	public:
		static void destroy(cmodule& module) {}

		struct sdefinition
		{
			vector_t<string_t> m_dependencies; //- Modules this depends on
			string_t m_directory;
			string_t m_resource_extension;
			string_t m_asset_group;
			bool m_recursive = false;

			RTTR_ENABLE();
		};

		cmodule() = default;
		~cmodule() = default;

		cmodule& name(stringview_t s);
		cmodule& path_alias(stringview_t alias);
		cmodule& definition(const sdefinition& def);

		stringview_t name() const;
		stringview_t path_alias() const;
		const vector_t<sdefinition>& definitions() const;

	private:
		vector_t<sdefinition> m_definitions;
		string_t m_name;
		string_t m_path_alias;

		RTTR_ENABLE(core::cresource);
		RTTR_REGISTRATION_FRIEND;
	};

} //- io