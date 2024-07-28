#pragma once
#include "../editor_dock_base.hpp"

namespace editor
{
	//- Responsible for working with an entity, i.e. adding/removing components or altering their data
	//------------------------------------------------------------------------------------------------------------------------
	class centity_inspector final : public clayer_base
	{
	public:
		centity_inspector(scontext& ctx) : clayer_base(ImGui::GetID("##entity_inspector"), ctx) {};
		~centity_inspector() = default;

		bool init() override;
		void shutdown() override;
		void on_ui_render() override;

	private:

	};

} //- editor