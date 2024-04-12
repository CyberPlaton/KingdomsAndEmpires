#pragma once
#include "../detail/ai_common.hpp"

//- Utility macro for defining a action or condition node, where only a tick function is required without a custom constructor.
//------------------------------------------------------------------------------------------------------------------------
#define DECLARE_LEAF_NODE(c) \
REFLECT_INLINE(c) \
{ \
	rttr::cregistrator<c, rttr::detail::no_default>(STRING(c)) \
		.ctor<rttr::detail::as_object, cbehavior_tree&, node_id_t>() \
		.meth(C_TICK_FUNCTION_NAME.data(), &c::do_tick) \
		; \
};

//- Basically the same as above but with custom constructor parameters.
//- When declaring the constructor provide only the parameters required after the default node ones.
//------------------------------------------------------------------------------------------------------------------------
#define DECLARE_LEAF_NODE_EX(c, ...) \
REFLECT_INLINE(c) \
{ \
	rttr::cregistrator<c, rttr::detail::no_default>(STRING(c)) \
		.ctor<rttr::detail::as_object, cbehavior_tree&, node_id_t, __VA_ARGS__>() \
		.meth(C_TICK_FUNCTION_NAME.data(), &c::do_tick) \
		; \
};

namespace ai
{
	namespace bt
	{
		namespace
		{
			constexpr std::string_view C_TICK_FUNCTION_NAME			= "do_tick";
			constexpr std::string_view C_EMPLACE_CHILD_FUNCTION_NAME= "emplace_child";
			constexpr std::string_view C_ID_FUNCTION_NAME			= "id";

		} //- unnamed


		class cbehavior_tree;

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

		//- The one and only base class for a behavior tree node.
		//------------------------------------------------------------------------------------------------------------------------
		class ibehavior_tree_context_holder
		{
		public:
			ibehavior_tree_context_holder(cbehavior_tree& context, node_id_t id) :
				m_context(context), m_id(id)
			{
			}

			virtual node_id_t id() const;

		private:
			cbehavior_tree& m_context;
			const node_id_t m_id;

		protected:
			cbehavior_tree& tree();
			const cbehavior_tree& tree() const;

			tick_result do_tick_child(uint32_t i);

			RTTR_ENABLE();
		};

		//- Composite node. Executes children until the first returns ok.
		//------------------------------------------------------------------------------------------------------------------------
		class cfallback : public ibehavior_tree_context_holder
		{
		public:
			cfallback(cbehavior_tree& context, node_id_t id) :
				ibehavior_tree_context_holder(context, id)
			{
			}

			tick_result do_tick();

			void emplace_child(node_id_t id);

		private:
			vector_t<uint32_t> m_children;

			RTTR_ENABLE(ibehavior_tree_context_holder);
			RTTR_REFLECTABLE();
		};

		//- Composite node. Executes children until the first returns fail.
		//------------------------------------------------------------------------------------------------------------------------
		class csequence : public ibehavior_tree_context_holder
		{
		public:
			csequence(cbehavior_tree& context, node_id_t id) :
				ibehavior_tree_context_holder(context, id)
			{
			}

			tick_result do_tick();

			void emplace_child(node_id_t id);

		private:
			vector_t<uint32_t> m_children;

			RTTR_ENABLE(ibehavior_tree_context_holder);
			RTTR_REFLECTABLE();
		};

		//- Leaf node. Executes its function and returns result.
		//- Note: when implementing new kinds of actions, make sure it looks like this class.
		//------------------------------------------------------------------------------------------------------------------------
		class caction : public ibehavior_tree_context_holder
		{
		public:
			caction(cbehavior_tree& context, node_id_t id) :
				ibehavior_tree_context_holder(context, id)
			{
			}

			tick_result do_tick();

		private:
			RTTR_ENABLE(ibehavior_tree_context_holder);
		};

		//- Leaf node. Executes its function and returns result.
		//- Note: when implementing new kinds of actions, make sure it looks like this class.
		//- In this context tick_result_ok is true and tick_result_fail is false.
		//------------------------------------------------------------------------------------------------------------------------
		class ccondition : public ibehavior_tree_context_holder
		{
		public:
			ccondition(cbehavior_tree& context, node_id_t id) :
				ibehavior_tree_context_holder(context, id)
			{
			}

			tick_result do_tick();

		private:
			RTTR_ENABLE(ibehavior_tree_context_holder);
		};

		//- Only relevant internal to behavior tree class.
		//------------------------------------------------------------------------------------------------------------------------
		class croot : public ibehavior_tree_context_holder
		{
		public:
			croot(cbehavior_tree& context, node_id_t id) :
				ibehavior_tree_context_holder(context, id)
			{
			}

			void do_tick();

			void emplace_child(node_id_t id);

		private:
			vector_t<uint32_t> m_children;

			RTTR_ENABLE(ibehavior_tree_context_holder);
			RTTR_REFLECTABLE();
		};

		//- Action node emitting a debug message.
		//------------------------------------------------------------------------------------------------------------------------
		class caction_logg : public ibehavior_tree_context_holder
		{
		public:
			caction_logg(cbehavior_tree& context, node_id_t id, stringview_t message) :
				ibehavior_tree_context_holder(context, id), m_message(message)
			{
			}

			tick_result do_tick();

		private:
			const std::string m_message;
			RTTR_ENABLE(ibehavior_tree_context_holder);
		};

		//- Inheritance free (mostly) and virtual functions free behavior tree implementation.
		//- TODO:
		//- detaching children from nodes
		//- attaching a tree as a node to another tree (along with detaching)
		//- decorator nodes
		//- blackboards (one global for a Tree and optional local for nodes)
		//------------------------------------------------------------------------------------------------------------------------
		class cbehavior_tree
		{
		public:
			cbehavior_tree(stringview_t name);

// 			void tick()
// 			{
// 				engine::cengine::service<engine::cthread_service>()->push_background_job(fmt::format("{}_tick", m_name),
// 					[this]()
// 					{
// 						const auto& root = m_nodes[0];
// 
// 						const auto& method = root.get_type().get_method("do_tick");
// 
// 						method.invoke(root);
// 					});
// 			}

			template<typename TNode, typename... ARGS>
			node_id_t emplace(ARGS&&... args);

			template<typename TNode, typename... ARGS>
			node_id_t attach_to(node_id_t parent, ARGS&&... args);

			template<typename TNode>
			node_id_t emplace();

			template<typename TNode>
			node_id_t attach_to(node_id_t parent);

			void tick();

			tick_result tick_node(uint32_t i);

			tree_id_t id() const;
			stringview_t name() const;

		private:
			const std::string m_name;
			vector_t<rttr::variant> m_nodes;
			const tree_id_t m_id;
			node_id_t m_next_id; //- Note: 0 is reserved for root node

		private:
			node_id_t generate_node_id();

			bool attach_node_to(node_id_t id, node_id_t parent);

		};

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TNode, typename... ARGS>
		node_id_t cbehavior_tree::emplace(ARGS&&... args)
		{
			return attach_to<TNode, ARGS...>((node_id_t)0, args...);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TNode, typename... ARGS>
		node_id_t cbehavior_tree::attach_to(node_id_t parent, ARGS&&... args)
		{
			auto id = generate_node_id();

			auto& node = m_nodes.emplace_back(TNode(*this, id, args...));

			if (id > 0 && !attach_node_to(id, parent))
			{
				logging::log_error(fmt::format("[Behavior Tree '{} (#{})'] attaching node '{}' to '{}' failed",
					m_name, m_id, id, parent));
			}
			else if (id == 0 && parent > 0)
			{
				logging::log_warn(fmt::format("[Behavior Tree '{} (#{})'] attaching root node to '{}' is not allowed",
					m_name, m_id, parent));
			}

			return id;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TNode>
		node_id_t cbehavior_tree::attach_to(node_id_t parent)
		{
			auto id = generate_node_id();

			auto& node = m_nodes.emplace_back(TNode(*this, id));

			if (id > 0 && !attach_node_to(id, parent))
			{
				logging::log_error(fmt::format("[Behavior Tree '{} (#{})'] attaching node '{}' to '{}' failed",
					m_name, m_id, id, parent));
			}
			else if (id == 0 && parent > 0)
			{
				logging::log_warn(fmt::format("[Behavior Tree '{} (#{})'] attaching root node to '{}' is not allowed",
					m_name, m_id, parent));
			}

			return id;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TNode>
		node_id_t cbehavior_tree::emplace()
		{
			return attach_to<TNode>((node_id_t)0);
		}

	} //- bt

} //- ai

namespace ai
{
	namespace bt
	{
		//------------------------------------------------------------------------------------------------------------------------
		REFLECT_INLINE(ibehavior_tree_context_holder)
		{
			rttr::cregistrator<ibehavior_tree_context_holder, rttr::detail::no_default>("ibehavior_tree_context_holder")
				.ctor<rttr::detail::as_object, cbehavior_tree&, node_id_t>()
				.meth(C_ID_FUNCTION_NAME.data(), &ibehavior_tree_context_holder::id)
				;
		};

		//------------------------------------------------------------------------------------------------------------------------
		REFLECT_INLINE(croot)
		{
			rttr::cregistrator<croot, rttr::detail::no_default>("croot")
				.ctor<rttr::detail::as_object, cbehavior_tree&, node_id_t>()
				.meth(C_TICK_FUNCTION_NAME.data(), &croot::do_tick)
				.meth(C_EMPLACE_CHILD_FUNCTION_NAME.data(), &croot::emplace_child)
				.prop("m_children", &croot::m_children)
				;
		};

		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_LEAF_NODE(ccondition);

		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_LEAF_NODE(caction);

		//------------------------------------------------------------------------------------------------------------------------
		REFLECT_INLINE(csequence)
		{
			rttr::cregistrator<csequence, rttr::detail::no_default>("csequence")
				.ctor<rttr::detail::as_object, cbehavior_tree&, node_id_t>()
				.meth(C_TICK_FUNCTION_NAME.data(), &csequence::do_tick)
				.meth(C_EMPLACE_CHILD_FUNCTION_NAME.data(), &csequence::emplace_child)
				.prop("m_children", &csequence::m_children)
				;
		};

		//------------------------------------------------------------------------------------------------------------------------
		REFLECT_INLINE(cfallback)
		{
			rttr::cregistrator<cfallback, rttr::detail::no_default>("cfallback")
				.ctor<rttr::detail::as_object, cbehavior_tree&, node_id_t>()
				.meth(C_TICK_FUNCTION_NAME.data(), &cfallback::do_tick)
				.meth(C_EMPLACE_CHILD_FUNCTION_NAME.data(), &cfallback::emplace_child)
				.prop("m_children", &cfallback::m_children)
				;
		};

		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_LEAF_NODE_EX(caction_logg, stringview_t);

	} //- bt

} //- ai