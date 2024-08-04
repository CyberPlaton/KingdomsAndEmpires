#include "imgui_scopes.hpp"

namespace imgui
{
	//------------------------------------------------------------------------------------------------------------------------
	cmenu_item::cmenu_item(const char* name, bool* selected /*= nullptr*/, const bool enabled /*= true*/)
	{
		m_result = ImGui::MenuItem(name, nullptr, selected, enabled);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmenu_scope::cmenu_scope(const char* name, const bool enabled /*= true*/)
	{
		m_result = ImGui::BeginMenu(name, enabled);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmenu_scope::~cmenu_scope()
	{
		if (m_result) { ImGui::EndMenu(); }
	}

	//------------------------------------------------------------------------------------------------------------------------
	cwindow_scope::cwindow_scope(stringview_t name, bool* open, ImGuiWindowFlags flags /*= ImGuiWindowFlags_None*/,
		const vec2_t& position /*= { 0, 0 }*/, const vec2_t& size_min /*= { 0, 0 }*/, const vec2_t& size_max /*= { 0, 0 }*/) :
		m_flags(flags)
	{
		CORE_ASSERT(open, "Invalid operation");

		ImGui::SetNextWindowPos({ position.x, position.y }, ImGuiCond_Appearing);

		if (size_min.x > 0 && size_min.y > 0 && size_max.x > 0 && size_max.y > 0)
		{
			ImGui::SetNextWindowSizeConstraints({ size_min.x, size_min.y }, { size_max.x, size_max.y });
		}

		m_result = ImGui::Begin(name.data(), open, flags);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cwindow_scope::~cwindow_scope()
	{
		if (algorithm::bit_check(m_flags, ImGuiWindowFlags_ChildWindow))
		{
			ImGui::EndChild();
		}
		else
		{
			ImGui::End();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cid_scope::cid_scope(const char* name)
	{
		ImGui::PushID(name);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cid_scope::~cid_scope()
	{
		ImGui::PopID();
	}

} //- imgui