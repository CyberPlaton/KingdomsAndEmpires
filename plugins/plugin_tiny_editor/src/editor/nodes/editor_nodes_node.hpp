#pragma once
#include <core.h>
#include <engine.h>

//-- Use this macro to declare a new node type
//------------------------------------------------------------------------------------------------------------------------
#define DECLARE_NODE(_class) \
private: \
friend class cnode<_class>::cscope;

namespace editor::nodes
{
	//- Helper class to instantiate a node with a special type. Does not hold any data.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	class cnode : public IImNode
	{
	public:
		class cscope final
		{
		public:
			template<typename... ARGS>
			cscope(int id, ARGS&&... args);
			~cscope();

			TType& settings();

		private:
			TType* m_node = nullptr;
		};

		virtual ~cnode() = default;

		TType& group(int id);
		TType& header(stringview_t title, stringview_t icon, stringview_t description, stringview_t tooltip);
		TType& color(ImNodesCol variable, const core::scolor& value);
		TType& style(ImNodesStyleVar variable, const vec2_t& value);
		TType& style(ImNodesStyleVar variable, float value);

	private:
		void begin();
		void end();
		TType& self() { return *static_cast<TType*>(this); }
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	template<typename... ARGS>
	cnode<TType>::cscope::cscope(int id, ARGS&&... args)
	{
		auto& data = IImNode::internal_data(id);

		if (!data.Class)
		{
			data.Class = new TType(id, std::forward<ARGS>(args)...);
		}

		m_node = reinterpret_cast<TType*>(data.Class);
		m_node->m_data = &data;

		m_node->begin();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	cnode<TType>::cscope::~cscope()
	{
		m_node->end();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	TType& cnode<TType>::cscope::settings()
	{
		return *m_node;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	TType& cnode<TType>::header(stringview_t title, stringview_t icon, stringview_t description, stringview_t tooltip)
	{
		CORE_ASSERT(m_data, "Invalid operation. Internal data is not ready!");

		ImNodes::BeginNodeTitleBar();

		ImGui::Text(fmt::format("{} {}\n{}", title.data(), icon.data(), description.data()).data());

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * tooltip.size());
			ImGui::TextUnformatted(tooltip.data());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		ImNodes::EndNodeTitleBar();

		return self();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	TType& cnode<TType>::group(int id)
	{
		CORE_ASSERT(m_data, "Invalid operation. Internal data is not ready!");

		m_data->Group = id;

		return self();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	TType& cnode<TType>::style(ImNodesStyleVar variable, float value)
	{
		m_styles.push(variable, value);

		return self();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	TType& cnode<TType>::style(ImNodesStyleVar variable, const vec2_t& value)
	{
		m_styles.push(variable, value);

		return self();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	TType& cnode<TType>::color(ImNodesCol variable, const core::scolor& value)
	{
		CORE_ASSERT(variable >= ImNodesCol_NodeBackground && variable <= ImNodesCol_TitleBarSelected,
			"Invalid operation. Incorrect color variable selected!");

		m_colors.push(variable, value.abgr());

		return self();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	void cnode<TType>::end()
	{
		ImNodes::EndNode();
		m_scope = false;
		m_colors.reset();
		m_styles.reset();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	void cnode<TType>::begin()
	{
		CORE_ASSERT(m_data, "Invalid operation. Internal data is not ready!");

		ImNodes::BeginNode(m_data->Id);
		m_scope = true;
	}

} //- editor::nodes