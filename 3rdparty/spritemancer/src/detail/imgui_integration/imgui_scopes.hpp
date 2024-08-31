#pragma once
#include <core.h>
#include <imgui.h>

namespace imgui
{
	//------------------------------------------------------------------------------------------------------------------------
	class cmenu_scope final :
		private core::cnon_copyable,
		private core::cnon_movable
	{
	public:
		cmenu_scope(const char* name, const bool enabled = true);
		~cmenu_scope();

		operator bool() const noexcept { return m_result; }

	private:
		bool m_result = false;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cmenu_item final :
		private core::cnon_copyable,
		private core::cnon_movable
	{
	public:
		cmenu_item(const char* name, bool* selected = nullptr, const bool enabled = true);
		~cmenu_item() = default;

		operator bool() const noexcept { return m_result; }

	private:
		bool m_result = false;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cwindow_scope final :
		private core::cnon_copyable,
		private core::cnon_movable
	{
	public:
		cwindow_scope(stringview_t name, bool* open, ImGuiWindowFlags flags = ImGuiWindowFlags_None, const vec2_t& position = { 0, 0 }, const vec2_t& size_min = { 0, 0 }, const vec2_t& size_max = { 0, 0 });
		~cwindow_scope();

		operator bool() const noexcept { return m_result; }

	private:
		ImGuiWindowFlags m_flags;
		bool m_result = false;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cid_scope final :
		private core::cnon_copyable,
		private core::cnon_movable
	{
	public:
		cid_scope(const char* name);
		~cid_scope();
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cdisabled_scope final :
		private core::cnon_copyable,
		private core::cnon_movable
	{
	public:
		cdisabled_scope(const bool is_disabled = true);
		~cdisabled_scope();
	};

} //- imgui