#pragma once
#include "../detail/ai_common.hpp"

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

		//- One and only base class for a behavior tree node.
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

			template<typename TNode>
			node_id_t emplace();

			template<typename TNode>
			node_id_t attach_to(node_id_t parent);

			void tick();

			tick_result tick_node(uint32_t i);

			tree_id_t id() const;

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
		template<typename TNode>
		node_id_t cbehavior_tree::attach_to(node_id_t parent)
		{
			auto id = generate_node_id();

			auto& node = m_nodes.emplace_back(TNode(*this, id));

			if (parent > 0 && !attach_node_to(id, parent))
			{
				//- report error attaching a child to parent node
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
		REFLECT_INLINE(ccondition)
		{
			rttr::cregistrator<ccondition, rttr::detail::no_default>("ccondition")
				.ctor<rttr::detail::as_object, cbehavior_tree&, node_id_t>()
				.meth(C_TICK_FUNCTION_NAME.data(), &ccondition::do_tick)
				;
		};

		//------------------------------------------------------------------------------------------------------------------------
		REFLECT_INLINE(caction)
		{
			rttr::cregistrator<caction, rttr::detail::no_default>("caction")
				.ctor<rttr::detail::as_object, cbehavior_tree&, node_id_t>()
				.meth(C_TICK_FUNCTION_NAME.data(), &caction::do_tick)
				;
		};

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

	} //- bt

} //- ai