#pragma once
#include "../editor_dock_base.hpp"

namespace editor::nodes
{
	//- TODO:
	//- 1. imnodes node groups support
	//- 2. link coloration, drawing icons at a specific position, disabled nodes support
	//- 3. imnodes node label drawing support, different logic and behavior for node types
	//- 4. 

	//------------------------------------------------------------------------------------------------------------------------
	enum node_type : uint8_t
	{
		node_type_none		= 0,
		node_type_dynamic	= BIT(1),	//- a default type of node with input links on left and output links on right side
		node_type_static	= BIT(2),	//- a special type of node where links are created to node instead of individual pins
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum link_icon_position : uint8_t
	{
		link_icon_position_none		= 0,
		link_icon_position_start	= BIT(1),	//- icon at start of the link, just outside of the pin
		link_icon_position_end		= BIT(2),	//- icon at end of the link, just outside of the pin
		link_icon_position_center	= BIT(3)	//- icon in middle of the link
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum pin_shape : uint8_t
	{
		pin_shape_none = 0,
		pin_shape_circle,
		pin_shape_triangle,
		pin_shape_quad,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum pin_type : uint8_t
	{
		pin_type_none = 0,
		pin_type_input,
		pin_type_output,
		pin_type_static,
	};

	ImNodesStyle& get_global_style();

	//------------------------------------------------------------------------------------------------------------------------
	class cscoped_color_variable final
	{
	public:
		cscoped_color_variable(ImNodesCol var, const core::scolor& color, bool manual_scope = false);
		~cscoped_color_variable();

		void end();

	private:
		bool m_manual_scope;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cscoped_style_variable final
	{
	public:
		cscoped_style_variable(ImNodesStyleVar var, const vec2_t& value, bool manual_scope = false);
		cscoped_style_variable(ImNodesStyleVar var, float value, bool manual_scope = false);
		~cscoped_style_variable();

		void end();

	private:
		bool m_manual_scope;
	};

	//- Immediate mode imnodes node defining a nodes visuals and settings that are used in imnodes internally
	//------------------------------------------------------------------------------------------------------------------------
	class cnode final
	{
	public:
		cnode(int id);
		~cnode();

		cnode& group(int id);
		cnode& type(node_type value);
		cnode& label(stringview_t value, bool titlebar = true);
		cnode& color(ImNodesCol variable, const core::scolor& value);
		cnode& style(ImNodesStyleVar variable, const vec2_t& value);
		cnode& style(ImNodesStyleVar variable, float value);

		void begin();
		void end();

	private:
		ImNodeData& m_data;
		vector_t<cscoped_color_variable> m_colors;
		vector_t<cscoped_style_variable> m_styles;
		bool m_scope;
		node_type m_type;

	private:
		ImNodeData& internal_data(int id);
	};

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