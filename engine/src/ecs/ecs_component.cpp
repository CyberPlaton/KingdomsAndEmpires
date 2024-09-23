#include "ecs_component.hpp"

namespace ecs
{
	namespace
	{
		constexpr auto C_MIN = MIN(float);
		constexpr auto C_MAX = MAX(float);

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void sidentifier::show_ui(flecs::entity e)
	{
		auto* identifier = e.get_mut<sidentifier>();

		ImGui::Text(identifier->m_name.data());
		ImGui::Text(identifier->m_uuid.string().data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void stransform::show_ui(flecs::entity e)
	{
		auto* transform = e.get_mut<stransform>();

		vec2_t translation = { 0.0f, 0.0f }, scale = { 0.0f, 0.0f }, shear = { 0.0f, 0.0f };
		float rotation = 0.0f;
		ImGuiID sid = 10000, scid = 50000;

		translation = math::extract_translation(transform->m_matrix);
		scale = math::extract_scale(transform->m_matrix);
		shear = math::extract_shear(transform->m_matrix);
		rotation = math::extract_rotation(transform->m_matrix);

		imgui::cui::edit_field_vec2(translation, C_MIN, C_MAX, "Translation", "Translation", sid++, scid++, ImGuiSliderFlags_Logarithmic, 0.1f);
		imgui::cui::edit_field_vec2(scale, C_MIN, C_MAX, "Scale", "Scale", sid++, scid++, ImGuiSliderFlags_Logarithmic, 0.1f);
		imgui::cui::edit_field_vec2(shear, C_MIN, C_MAX, "Shear", "Shear", sid++, scid++, ImGuiSliderFlags_Logarithmic, 0.1f);
		imgui::cui::edit_field_float(rotation, C_MIN, C_MAX, "Rotation", "Rotation", sid++, scid++, ImGuiSliderFlags_Logarithmic, 0.1f);

		transform->m_matrix = math::transform(translation, scale, shear, rotation);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void shierarchy::show_ui(flecs::entity e)
	{
		auto* hierarchy = e.get_mut<shierarchy>();

		ImGui::Text(hierarchy->m_parent.string().data());


		for (const auto& uuid : hierarchy->m_children)
		{
			ImGui::Text(uuid.string().data());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void sanimation::show_ui(flecs::entity e)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void scamera::show_ui(flecs::entity e)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void smaterial::show_ui(flecs::entity e)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void ssprite_renderer::show_ui(flecs::entity e)
	{

	}

} //- ecs