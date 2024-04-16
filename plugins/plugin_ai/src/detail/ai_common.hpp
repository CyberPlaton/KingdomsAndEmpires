#pragma once
#include <core.h>
#include <engine.h>
#include <plugin_logging.h>

#ifndef BT_NO_DEBUG
#define BT_EMIT_TICK_EVENT(tree_id, node_id) \
engine::cengine::service<engine::cevent_service>()->emit_event<debug::snode_tick_event>(tree_id, node_id)

#define BT_EMIT_ATTACH_NODE_EVENT(tree_id, node_id, parent_id, node_type) \
engine::cengine::service<engine::cevent_service>()->emit_event<debug::snode_attach_event>(tree_id, node_id, node_type, parent_id)

#define BT_EMIT_ATTACH_DECORATOR_EVENT(tree_id, node_id, decorator_type) \
engine::cengine::service<engine::cevent_service>()->emit_event<debug::sdecorator_attach_event>(tree_id, node_id, decorator_type)

#else
#define BT_EMIT_TICK_EVENT(tree_id, node_id)
#define BT_EMIT_ATTACH_NODE_EVENT(tree_id, node_id, parent_id, node_type)
#define BT_EMIT_ATTACH_DECORATOR_EVENT(tree_id, node_id, decorator_type)
#endif

namespace ai
{
	namespace bt
	{
		using node_id_t = handle_type_t;
		using tree_id_t = handle_type_t;

		//------------------------------------------------------------------------------------------------------------------------
		enum tick_result : uint8_t
		{
			tick_result_none = 0,
			tick_result_ok,
			tick_result_fail,
			tick_result_pending,
		};

		//- Simple blackboard for storing data of any type.
		//------------------------------------------------------------------------------------------------------------------------
		class cblackboard
		{
		public:
			cblackboard() = default;
			~cblackboard() = default;

			//- Get reference to value stored in blackboard. Crashes when element not present or type does not match.
			template<typename TType>
			const TType& find(stringview_t name) const;

			//- Get reference to value stored in blackborad. Emplaces new one if not existent, crashes when type does not match.
			template<typename TType>
			TType& get_or_emplace(stringview_t name);

		private:
			umap_t<stringview_t, core::cany> m_storage;
		};

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TType>
		TType& cblackboard::get_or_emplace(stringview_t name)
		{
			return m_storage[name.data()].cast_ref<TType>();
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TType>
		const TType& cblackboard::find(stringview_t name) const
		{
			return m_storage.at(name.data()).cast_ref<TType>();
		}

		namespace debug
		{
			//- Event describing a Tree node tick. We assume that the node to be ticked was previously registered.
			//------------------------------------------------------------------------------------------------------------------------
			struct snode_tick_event
			{
				snode_tick_event(tree_id_t tree, node_id_t node) :
					m_tree(tree), m_node(node)
				{}

				tree_id_t m_tree = invalid_handle_t;
				node_id_t m_node = invalid_handle_t;
			};

			//- Event describing a new node being attached to an existing one.
			//- When parent is 0, then attaching is done to root of the Tree.
			//------------------------------------------------------------------------------------------------------------------------
			struct snode_attach_event
			{
				snode_attach_event(tree_id_t tree, node_id_t node, rttr::type node_type, node_id_t parent = 0) :
					m_tree(tree), m_node(node), m_parent(parent), m_node_type(node_type)
				{}

				tree_id_t m_tree = invalid_handle_t;
				node_id_t m_node = invalid_handle_t;
				node_id_t m_parent = invalid_handle_t;
				rttr::type m_node_type;
			};

			//- Event describing a new decorator being attached to a Tree node.
			//------------------------------------------------------------------------------------------------------------------------
			struct sdecorator_attach_event
			{
				sdecorator_attach_event(tree_id_t tree, node_id_t node, rttr::type decorator_type) :
					m_tree(tree), m_node(node), m_decorator_type(decorator_type)
				{}

				tree_id_t m_tree = invalid_handle_t;
				node_id_t m_node = invalid_handle_t;
				rttr::type m_decorator_type;
			};

		} //- debug

	} //- bt

} //- ai