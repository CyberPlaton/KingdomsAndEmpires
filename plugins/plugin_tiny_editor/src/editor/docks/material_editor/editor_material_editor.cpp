#include "editor_material_editor.hpp"

namespace editor
{
	namespace
	{
	} //- unnamed

	namespace nodes
	{
		//------------------------------------------------------------------------------------------------------------------------
		cnode::cnode(int id) : m_data(internal_data(id)), m_scope(false)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		cnode::~cnode()
		{
			if (m_scope)
			{
				end();
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		cnode& cnode::group(int id)
		{
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cnode& cnode::type(node_type value)
		{
			m_data.Type = (ImNodesNodeType)value;
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cnode& cnode::label(stringview_t value, bool titlebar /*= true*/)
		{
			m_data.Label = value.data();
			if (titlebar)
			{
				algorithm::bit_set(m_data.Flags, ImNodesFlags_Titlebar);
			}
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cnode::begin()
		{
			ImNodes::BeginNode(m_data.Id);
			m_scope = true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cnode::end()
		{
			ImNodes::EndNode();
			m_scope = false;

			//- reset colors and style variables
			for (auto& variable : m_colors) { variable.end(); }
			for (auto& variable : m_styles) { variable.end(); }
		}

		//------------------------------------------------------------------------------------------------------------------------
		ImNodeData& cnode::internal_data(int id)
		{
			auto& context = ImNodes::EditorContextGet();
			const int node_idx = ImNodes::ObjectPoolFindOrCreateIndex(context.Nodes, id);
			return context.Nodes.Pool[node_idx];
		}

		//------------------------------------------------------------------------------------------------------------------------
		clink::clink(int id) : m_data(internal_data(id)), m_enabled(true), m_scope(false), m_icon_position(link_icon_position_none)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		clink::~clink()
		{
			end();
		}

		//------------------------------------------------------------------------------------------------------------------------
		clink& clink::thickness(float link_thickness)
		{
			m_data.Thickness = link_thickness;
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		clink& clink::icon(stringview_t value, link_icon_position where)
		{
			m_data.Label = value.data();
			m_data.LabelPosition = (ImNodesLinkLabelPosition)where;
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void clink::begin(int from_pin_id, int to_pin_id)
		{
			ImNodes::Link(m_data.Id, from_pin_id, to_pin_id);
			m_scope = true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void clink::end()
		{
			m_scope = false;

			//- reset colors and style variables
			for (auto& variable : m_colors) { variable.end(); }
			for (auto& variable : m_styles) { variable.end(); }
		}

		//------------------------------------------------------------------------------------------------------------------------
		ImLinkData& clink::internal_data(int id)
		{
			auto& context = ImNodes::EditorContextGet();
			const int node_idx = ImNodes::ObjectPoolFindOrCreateIndex(context.Links, id);
			return context.Links.Pool[node_idx];
		}

		//------------------------------------------------------------------------------------------------------------------------
		cpin::cpin(int id) : m_data(internal_data(id)), m_shape(pin_shape_none), m_type(pin_type_none), m_scope(false), m_filled(false)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		cpin::~cpin()
		{
			if (m_scope)
			{
				end();
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		ImPinData& cpin::internal_data(int id)
		{
			auto& context = ImNodes::EditorContextGet();
			const int node_idx = ImNodes::ObjectPoolFindOrCreateIndex(context.Pins, id);

			auto& data = context.Pins.Pool[node_idx];
			data.Flags = GImNodes->CurrentAttributeFlags;

			return data;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cpin::begin()
		{
			ImNodesPinShape shape;

			switch (m_shape)
			{
			default:
			case pin_shape_none:
			case pin_shape_circle:
			{
				shape = m_filled ? ImNodesPinShape_CircleFilled : ImNodesPinShape_Circle;
				break;
			}
			case pin_shape_quad:
			{
				shape = m_filled ? ImNodesPinShape_QuadFilled : ImNodesPinShape_Quad;
				break;
			}
			case pin_shape_triangle:
			{
				shape = m_filled ? ImNodesPinShape_TriangleFilled : ImNodesPinShape_Triangle;
				break;
			}
			}

			switch (m_type)
			{
			case pin_type_input:
			{
				ImNodes::BeginInputAttribute(m_data.Id, shape);
				break;
			}
			case pin_type_output:
			{
				ImNodes::BeginOutputAttribute(m_data.Id, shape);
				break;
			}
			case pin_type_static:
			{
				ImNodes::BeginStaticAttribute(m_data.Id);
				break;
			}
			}

			m_scope = true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cpin::end()
		{
			switch (m_type)
			{
			case pin_type_input:
			{
				ImNodes::EndInputAttribute();
				break;
			}
			case pin_type_output:
			{
				ImNodes::EndOutputAttribute();
				break;
			}
			case pin_type_static:
			{
				ImNodes::EndStaticAttribute();
				break;
			}
			}
			m_scope = false;

			//- reset colors and style variables
			for (auto& variable : m_colors) { variable.end(); }
			for (auto& variable : m_styles) { variable.end(); }
		}

		//------------------------------------------------------------------------------------------------------------------------
		cpin& cpin::label(stringview_t value)
		{
			m_data.Label = value.data();
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cpin& cpin::type(pin_type value)
		{
			m_type = value;
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cpin& cpin::shape(pin_shape value)
		{
			m_shape = value;
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cpin& cpin::enabled(bool value)
		{
			m_data.Enabled = value;
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cpin& cpin::filled(bool value)
		{
			m_filled = value;
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cscoped_color_variable::cscoped_color_variable(ImNodesCol var, const core::scolor& color, bool manual_scope /*= false*/) :
			m_manual_scope(manual_scope)
		{
			ImNodes::PushColorStyle(var, color.abgr());
		}

		//------------------------------------------------------------------------------------------------------------------------
		cscoped_color_variable::~cscoped_color_variable()
		{
			if (!m_manual_scope)
			{
				ImNodes::PopColorStyle();
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		cscoped_style_variable::cscoped_style_variable(ImNodesStyleVar var, const vec2_t& value, bool manual_scope /*= false*/) :
			m_manual_scope(manual_scope)
		{
			ImNodes::PushStyleVar(var, {value.x, value.y});
		}

		//------------------------------------------------------------------------------------------------------------------------
		cscoped_style_variable::cscoped_style_variable(ImNodesStyleVar var, float value, bool manual_scope /*= false*/) :
			m_manual_scope(manual_scope)
		{
			ImNodes::PushStyleVar(var, value);
		}

		//------------------------------------------------------------------------------------------------------------------------
		cscoped_style_variable::~cscoped_style_variable()
		{
			if (!m_manual_scope)
			{
				ImNodes::PopStyleVar();
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cscoped_color_variable::end()
		{
			if (m_manual_scope)
			{
				ImNodes::PopColorStyle();
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cscoped_style_variable::end()
		{
			if (m_manual_scope)
			{
				ImNodes::PopStyleVar();
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		cnode& cnode::color(ImNodesCol variable, const core::scolor& value)
		{
			CORE_ASSERT(variable >= ImNodesCol_NodeBackground && variable <= ImNodesCol_TitleBarSelected,
				"Invalid operation. Incorrect color variable selected!");

			m_colors.emplace_back(variable, value, true);
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cnode& cnode::style(ImNodesStyleVar variable, const vec2_t& value)
		{
			m_styles.emplace_back(variable, value, true);
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cnode& cnode::style(ImNodesStyleVar variable, float value)
		{
			m_styles.emplace_back(variable, value, true);
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cpin& cpin::color(ImNodesCol variable, const core::scolor& value)
		{
			CORE_ASSERT(variable >= ImNodesCol_Pin && variable <= ImNodesCol_PinHovered,
				"Invalid operation. Incorrect color variable selected!");

			m_colors.emplace_back(variable, value, true);
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cpin& cpin::style(ImNodesStyleVar variable, const vec2_t& value)
		{
			m_styles.emplace_back(variable, value, true);
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cpin& cpin::style(ImNodesStyleVar variable, float value)
		{
			m_styles.emplace_back(variable, value, true);
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		clink& clink::color(ImNodesCol variable, const core::scolor& value)
		{
			CORE_ASSERT(variable >= ImNodesCol_Link && variable <= ImNodesCol_LinkSelected,
				"Invalid operation. Incorrect color variable selected!");

			m_colors.emplace_back(variable, value, true);
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		clink& clink::style(ImNodesStyleVar variable, const vec2_t& value)
		{
			m_styles.emplace_back(variable, value, true);
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		clink& clink::style(ImNodesStyleVar variable, float value)
		{
			m_styles.emplace_back(variable, value, true);
			return *this;
		}

	} //- nodes

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_editor::cmaterial_editor(scontext& ctx) :
		clayer_base(ctx), m_ctx(ImNodes::EditorContextCreate())
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_editor::~cmaterial_editor()
	{
		ImNodes::EditorContextFree(m_ctx);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmaterial_editor::init()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_editor::shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_editor::on_update(float dt)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_editor::on_ui_render()
	{
		if (ImGui::Begin("##MaterialEditor"))
		{
			ImNodes::BeginNodeEditor();

			ImNodes::EditorContextSet(m_ctx);

			for (auto i = 1; i < 6; ++i)
			{
				nodes::cnode node(i);

				if (i % 2 == 0)
				{
					node.color(ImNodesCol_NodeBackground, { 0, 255, 0, 255 });
				}
				else
				{
					node.color(ImNodesCol_NodeBackground, { 255, 0, 0, 255 });
				}
				node.label(fmt::format("Title {}", ICON_FA_ARROWS_DOWN_TO_PEOPLE), true)
					.begin();

				if (ImNodes::IsNodeVisible(i))
				{
					ImGui::Text(fmt::format("Node '{}'", i).data());
				}

				nodes::cpin pin(i);

				pin.type(nodes::pin_type_output)
					.shape(nodes::pin_shape_quad)
					;

				if (i % 2 == 0)
				{
					pin.filled(true)
						.color(ImNodesCol_Pin, { 200, 150, 150, 255 });
				}
				else
				{
					pin.filled(false)
						.color(ImNodesCol_Pin, { 100, 250, 250, 255 });
				}

				pin.begin();

				ImGui::Text(fmt::format("Pin '{}'", i).data());

				pin.end();

				node.end();
			}

			ImNodes::EndNodeEditor();
		}
		ImGui::End();
	}

} //- editor