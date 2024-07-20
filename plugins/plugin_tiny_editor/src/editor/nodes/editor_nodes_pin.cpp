#include "editor_nodes_pin.hpp"

namespace editor::nodes
{
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
	cpin& cpin::color(ImNodesCol variable, const core::scolor& value)
	{
		CORE_ASSERT(variable >= ImNodesCol_Pin && variable <= ImNodesCol_PinHovered,
			"Invalid operation. Incorrect color variable selected!");

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpin& cpin::style(ImNodesStyleVar variable, const vec2_t& value)
	{
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpin& cpin::style(ImNodesStyleVar variable, float value)
	{
		return *this;
	}

} //- editor::nodes