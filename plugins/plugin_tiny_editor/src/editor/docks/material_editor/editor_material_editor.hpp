#pragma once
#include "../editor_dock_base.hpp"

namespace editor::nodes
{
	//- TODO:
	//- 1. imnodes node groups support
	//- 2. link coloration, drawing icons at a specific position, disabled nodes support
	//- 3. imnodes node label drawing support, different logic and behavior for node types
	//- 4. 

	ImNodesStyle& get_global_style();

	//- Immediate mode imnodes link defining a links visuals and settings that are used in imnodes internally
	//------------------------------------------------------------------------------------------------------------------------
	class clink final
	{
	public:
		clink(int id);
		~clink();

		clink& color(ImNodesCol variable, const core::scolor& value);
		clink& style(ImNodesStyleVar variable, const vec2_t& value);
		clink& style(ImNodesStyleVar variable, float value);
		clink& thickness(float link_thickness);
		clink& icon(stringview_t value, link_icon_position where);

		void begin(int from_pin_id, int to_pin_id);
		void end();

	private:
		ImLinkData& m_data;
		vector_t<cscoped_color_variable> m_colors;
		vector_t<cscoped_style_variable> m_styles;
		bool m_enabled;
		bool m_scope;
		link_icon_position m_icon_position;

	private:
		ImLinkData& internal_data(int id);
	};

	//- Immediate mode imnodes pin defining a pins visual and settings that are used in imnodes internally
	//------------------------------------------------------------------------------------------------------------------------
	class cpin final
	{
	public:
		cpin(int id);
		~cpin();

		cpin& label(stringview_t value);
		cpin& type(pin_type value);
		cpin& shape(pin_shape value);
		cpin& color(ImNodesCol variable, const core::scolor& value);
		cpin& style(ImNodesStyleVar variable, const vec2_t& value);
		cpin& style(ImNodesStyleVar variable, float value);
		cpin& enabled(bool value);
		cpin& filled(bool value);

		void begin();
		void end();

	private:
		ImPinData& m_data;
		vector_t<cscoped_color_variable> m_colors;
		vector_t<cscoped_style_variable> m_styles;
		bool m_scope;
		bool m_filled;
		pin_shape m_shape;
		pin_type m_type;

	private:
		ImPinData& internal_data(int id);
	};

} //- editor::nodes

namespace editor
{
	//- Responsible for creating materials in a visual way.
	//------------------------------------------------------------------------------------------------------------------------
	class cmaterial_editor final : public clayer_base
	{
	public:
		cmaterial_editor(scontext& ctx);
		~cmaterial_editor();

		bool init() override final;
		void shutdown() override final;
		void on_update(float dt) override final;
		void on_ui_render() override final;

	private:
		ImNodesEditorContext* m_ctx;
	};

} //- editor