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
		static auto S_PLACEHOLDER_TEXTURE = 0;

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
		//- copy construct component for editing
		auto* transform = e.get_mut<stransform>();

		ImGuiID sid = 10000, scid = 50000;

		if (ImGui::TreeNode("Translation"))
		{
			ImGui::DragFloat("##translation_x", &transform->m_position.x, 1.0f);
			ImGui::SameLine();
			ImGui::DragFloat("##translation_y", &transform->m_position.y, 1.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Scale"))
		{
			ImGui::DragFloat("##scale_x", &transform->m_scale.x, 0.5f, 0.001f);
			ImGui::SameLine();
			ImGui::DragFloat("##scale_y", &transform->m_scale.y, 0.5f, 0.001f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Shear"))
		{
			ImGui::DragFloat("##shear_x", &transform->m_shear.x, 0.1f);
			ImGui::SameLine();
			ImGui::DragFloat("##shear_y", &transform->m_shear.y, 0.1f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Rotation"))
		{
			ImGui::DragFloat("##rotation", &transform->m_rotation, 0.01f, 0.0f, math::C_PI2);
			ImGui::TreePop();
		}
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
	void smesh::show_ui(flecs::entity e)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void smaterial::show_ui(flecs::entity e)
	{
	}

} //- ecs