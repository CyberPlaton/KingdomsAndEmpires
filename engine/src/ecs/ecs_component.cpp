#include "ecs_component.hpp"
#include "../spritemancer/sm_ui.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	void sidentifier::show_ui(flecs::entity e)
	{
		auto* identifier = e.get_mut<sidentifier>();

		ImGui::TextUnformatted(fmt::format("uuid:\t{}", identifier->m_uuid.string()).c_str());
		ImGui::TextUnformatted(fmt::format("id:\t{}", identifier->m_self.id()).c_str());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void stransform::show_ui(flecs::entity e)
	{
		auto* transform = e.get_mut<stransform>();

		ImGui::TextUnformatted(fmt::format("X:\t{}", transform->m_x).c_str());
		ImGui::TextUnformatted(fmt::format("Y:\t{}", transform->m_y).c_str());
		ImGui::TextUnformatted(fmt::format("W:\t{}", transform->m_w).c_str());
		ImGui::TextUnformatted(fmt::format("H:\t{}", transform->m_h).c_str());
		ImGui::TextUnformatted(fmt::format("R:\t{}", transform->m_rotation).c_str());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void shierarchy::show_ui(flecs::entity e)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void sanimation::show_ui(flecs::entity e)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void ssprite::show_ui(flecs::entity e)
	{

	}

} //- ecs