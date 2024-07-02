#include "module.hpp"

namespace io
{
	//------------------------------------------------------------------------------------------------------------------------
	io::cmodule& cmodule::name(stringview_t s)
	{
		m_name.assign(s.data());

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t cmodule::name() const
	{
		return m_name;
	}

	//------------------------------------------------------------------------------------------------------------------------
	io::cmodule& cmodule::path_alias(stringview_t alias)
	{
		m_path_alias.assign(alias.data());

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t cmodule::path_alias() const
	{
		return m_path_alias;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const vector_t<io::cmodule::sdefinition>& cmodule::definitions() const
	{
		return m_definitions;
	}

	//------------------------------------------------------------------------------------------------------------------------
	io::cmodule& cmodule::definition(const sdefinition& def)
	{
		m_definitions.emplace_back(def);

		return *this;
	}

} //- io