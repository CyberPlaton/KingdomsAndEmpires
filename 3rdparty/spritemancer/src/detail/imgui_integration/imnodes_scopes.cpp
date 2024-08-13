#include "imnodes_scopes.hpp"

namespace imnodes
{
	//------------------------------------------------------------------------------------------------------------------------
	ceditor_scope::ceditor_scope(ImNodes::EditorContext* ctx) :
		m_previous(ImNodes::GetCurrentEditor())
	{
		ImNodes::SetCurrentEditor(ctx);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ceditor_scope::~ceditor_scope()
	{
		ImNodes::SetCurrentEditor(m_previous);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstyle_scope::cstyle_scope(ImNodes::StyleVar var, const vec2_t& value)
	{
		push_back(var, value);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstyle_scope::cstyle_scope(ImNodes::StyleVar var, const float& value)
	{
		push_back(var, value);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstyle_scope::~cstyle_scope()
	{
		clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstyle_scope& cstyle_scope::push_back(ImNodes::StyleVar var, const vec2_t& value)
	{
		ImNodes::PushStyleVar(var, { value.x, value.y });
		++m_vars;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstyle_scope& cstyle_scope::push_back(ImNodes::StyleVar var, const float& value)
	{
		ImNodes::PushStyleVar(var, value);
		++m_vars;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstyle_scope& cstyle_scope::push_back(ImNodes::StyleVar var, const vec4_t& value)
	{
		ImNodes::PushStyleVar(var, { value.x, value.y, value.z, value.w });
		++m_vars;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstyle_scope& cstyle_scope::pop_back()
	{
		if (m_vars > 0)
		{
			ImNodes::PopStyleVar();
			--m_vars;
		}
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstyle_scope& cstyle_scope::clear()
	{
		ImNodes::PopStyleVar(SCAST(int, m_vars));
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccolor_scope::ccolor_scope(ImNodes::StyleColor var, const core::scolor& value)
	{
		push_back(var, value);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccolor_scope::~ccolor_scope()
	{
		clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccolor_scope& ccolor_scope::push_back(ImNodes::StyleColor var, const core::scolor& value)
	{
		const auto v = value.normalize();
		ImNodes::PushStyleColor(var, { v.r, v.g, v.b, v.a });
		++m_vars;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccolor_scope& ccolor_scope::pop_back()
	{
		if (m_vars > 0)
		{
			ImNodes::PopStyleColor();
			--m_vars;
		}
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccolor_scope& ccolor_scope::clear()
	{
		ImNodes::PopStyleColor(SCAST(int, m_vars));
		return *this;
	}

} //- imnodes