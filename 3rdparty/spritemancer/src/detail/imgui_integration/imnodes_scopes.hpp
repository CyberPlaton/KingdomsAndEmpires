#pragma once
#include <core.h>
#include <imgui.h>
#include "imgui.h"

namespace ImNodes = ax::NodeEditor;

namespace imnodes
{
	//------------------------------------------------------------------------------------------------------------------------
	class ceditor_scope final :
		private core::cnon_copyable,
		private core::cnon_movable
	{
	public:
		ceditor_scope(ImNodes::EditorContext* ctx);
		~ceditor_scope();

	private:
		ImNodes::EditorContext* m_previous;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cstyle_scope final :
		private core::cnon_copyable,
		private core::cnon_movable
	{
	public:
		cstyle_scope(ImNodes::StyleVar var, const vec2_t& value);
		cstyle_scope(ImNodes::StyleVar var, const float& value);
		cstyle_scope() = default;
		~cstyle_scope();

		template<typename T>
		cstyle_scope& push_back(ImNodes::StyleVar var, const T& value);
		cstyle_scope& push_back(ImNodes::StyleVar var, const vec2_t& value);
		cstyle_scope& push_back(ImNodes::StyleVar var, const vec4_t& value);
		cstyle_scope& push_back(ImNodes::StyleVar var, const float& value);
		cstyle_scope& pop_back();
		cstyle_scope& clear();

	private:
		unsigned m_vars;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	cstyle_scope& cstyle_scope::push_back(ImNodes::StyleVar var, const T& value)
	{
		static_assert(std::is_same_v<vec2_t, T> || std::is_same_v<float, T>, "Invalid operation. Style variable must be float or vec2!");
		return push_back(var, value);
	}

	//------------------------------------------------------------------------------------------------------------------------
	class ccolor_scope final :
		private core::cnon_copyable,
		private core::cnon_movable
	{
	public:
		ccolor_scope(ImNodes::StyleColor var, const core::scolor& value);
		ccolor_scope() = default;
		~ccolor_scope();

		ccolor_scope& push_back(ImNodes::StyleColor var, const core::scolor& value);
		ccolor_scope& pop_back();
		ccolor_scope& clear();

	private:
		unsigned m_vars;
	};

} //- imnodes