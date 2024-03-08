#include "editor_dock_right_panel.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	bool cright_panel::init()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cright_panel::shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cright_panel::on_ui_render()
	{
		const auto& entities = ecs::cworld_manager::instance().active().em().entities();
		auto e = entities[0];

		if (ImGui::Begin("Componenet UI test"))
		{
			vector_t<std::string> names;

			e.each([&](flecs::id c)
				{
					//- retrieve usable name of component without namespaces etc
					std::string name = c.str().c_str();
					core::string_utils::split(name, '.', names);

					if (ImGui::CollapsingHeader(names[names.size() - 1].c_str()))
					{
						auto type = rttr::type::get_by_name(names[names.size() - 1]);

						if (auto m = type.get_method(ecs::C_COMPONENT_SHOW_UI_FUNC_NAME); m.is_valid())
						{
							m.invoke({}, e);
						}
					}
				});


			ImGui::End();
		}
	}

} //- editor