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
		const ImVec2& position /*= { 0, 0 }*/, const ImVec2& size_min /*= { 0, 0 }*/, const ImVec2& size_max /*= { 0, 0 }*/) :
		m_flags(flags)
	{
		CORE_ASSERT(open, "Invalid operation");

		ImVec2 p = position;

		if (p.x == 0 && p.y == 0)
		{
			//p.x = SCAST(float, raylib::GetScreenWidth()) * 0.5f - glm::max(size_max.x, size_min.x) * 0.5f;
			//p.y = SCAST(float, raylib::GetScreenHeight()) * 0.5f - glm::max(size_max.y, size_min.y) * 0.5f;
		}

		ImGui::SetNextWindowPos(p, ImGuiCond_Appearing);

		if (size_min.x > 0 && size_min.y > 0 && size_max.x > 0 && size_max.y > 0)
		{
			ImGui::SetNextWindowSizeConstraints(size_min, size_max);
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

	//------------------------------------------------------------------------------------------------------------------------
	cdisabled_scope::cdisabled_scope(const bool is_disabled /*= true*/)
	{
		ImGui::BeginDisabled(is_disabled);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cdisabled_scope::~cdisabled_scope()
	{
		ImGui::EndDisabled();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cchild_scope::cchild_scope(stringview_t name, const ImVec2& size /*= {0.0f, 0.0f}*/,
		ImGuiChildFlags child_flags /*= ImGuiChildFlags_None*/, ImGuiWindowFlags window_flags /*= ImGuiWindowFlags_None*/)
	{
		m_result = ImGui::BeginChild(name.data(), size, child_flags, window_flags);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cchild_scope::~cchild_scope()
	{
		ImGui::EndChild();
	}

} //- imgui