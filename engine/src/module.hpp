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
		RTTR_REFLECTABLE();
	};

} //- io

namespace io
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cmodule::sdefinition)
	{
		rttr::cregistrator<cmodule::sdefinition>("cmodule::sdefinition")
			.prop("m_directory", &cmodule::sdefinition::m_directory)
			.prop("m_resource_extension", &cmodule::sdefinition::m_resource_extension)
			.prop("m_asset_group", &cmodule::sdefinition::m_asset_group)
			.prop("m_recursive", &cmodule::sdefinition::m_recursive)
			.prop("m_dependencies", &cmodule::sdefinition::m_dependencies)
			;
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cmodule)
	{
		rttr::cregistrator<cmodule>("cmodule")
			.prop("m_name", &cmodule::m_name)
			.prop("m_path_alias", &cmodule::m_path_alias)
			.prop("m_definitions", &cmodule::m_definitions)
			.meth(core::cresource::C_DESTROY_FUNCTION_NAME.data(), &cmodule::destroy)
			.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS, vector_t<string_t>{".module"})
			;
	};

} //- io