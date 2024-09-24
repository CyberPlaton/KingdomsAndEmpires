#include "ecs_component.hpp"

namespace ecs
{
	namespace
	{
		constexpr auto C_TRANSLATION_MIN= MIN(float);
		constexpr auto C_TRANSLATION_MAX= MAX(float);
		constexpr auto C_SCALE_MIN		= math::C_EPSILON;
		constexpr auto C_SCALE_MAX		= MAX(float);
		constexpr auto C_SHEAR_MIN		= MIN(float);
		constexpr auto C_SHEAR_MAX		= MAX(float);
		constexpr auto C_ROTATION_MIN	= 0.0f;
		constexpr auto C_ROTATION_MAX	= math::C_PI2;
		constexpr auto C_FORMAT_SCALE	= "%.5f";
		constexpr auto C_FORMAT_TRANS	= "%.3f";
		constexpr auto C_FORMAT_SHEAR	= "%.4f";
		constexpr auto C_FORMAT_ROT		= "%.5f";
		constexpr auto C_FLAGS			= ImGuiSliderFlags_Logarithmic;
		constexpr auto C_CHANGE_TRANS	= 1.0f;
		constexpr auto C_CHANGE_SCALE	= 1.0f;
		constexpr auto C_CHANGE_ROT		= 0.01f;
		constexpr auto C_CHANGE_SHEAR	= 0.1f;

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

		if (ImGui::TreeNode("Translation"))
		{
			ImGui::DragFloat("##x", &translation.x, C_CHANGE_TRANS, C_TRANSLATION_MIN, C_TRANSLATION_MAX, C_FORMAT_TRANS, C_FLAGS);
			ImGui::SameLine();
			ImGui::DragFloat("##y", &translation.y, C_CHANGE_TRANS, C_TRANSLATION_MIN, C_TRANSLATION_MAX, C_FORMAT_TRANS, C_FLAGS);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Scale"))
		{
			ImGui::DragFloat("##x", &scale.x, C_CHANGE_SCALE, C_SCALE_MIN, C_SCALE_MAX, C_FORMAT_SCALE, C_FLAGS);
			ImGui::SameLine();
			ImGui::DragFloat("##y", &scale.y, C_CHANGE_SCALE, C_SCALE_MIN, C_SCALE_MAX, C_FORMAT_SCALE, C_FLAGS);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Shear"))
		{
			ImGui::DragFloat("##x", &shear.x, C_CHANGE_SHEAR, C_SHEAR_MIN, C_SHEAR_MAX, C_FORMAT_SHEAR, C_FLAGS);
			ImGui::SameLine();
			ImGui::DragFloat("##y", &shear.y, C_CHANGE_SHEAR, C_SHEAR_MIN, C_SHEAR_MAX, C_FORMAT_SHEAR, C_FLAGS);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Rotation"))
		{
			ImGui::DragFloat("##r", &rotation, C_CHANGE_ROT, C_ROTATION_MIN, C_ROTATION_MAX, C_FORMAT_ROT, C_FLAGS);
			ImGui::TreePop();
		}

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