#pragma once
#include <core.h>
#include <engine.h>
#include <plugin_logging.h>

namespace ai
{
	namespace bt
	{
		class cbehavior_tree;

		enum tick_result : uint8_t
		{
			tick_result_none = 0,
			tick_result_ok,
			tick_result_fail,
			tick_result_pending,
		};

		//------------------------------------------------------------------------------------------------------------------------
		class ibehavior_tree_context_holder
		{
		public:
			ibehavior_tree_context_holder(cbehavior_tree& context) :
				m_context(context)
			{
			}

		private:
			cbehavior_tree& m_context;

		protected:
			cbehavior_tree& tree() {return m_context;}
			const cbehavior_tree& tree() const { return m_context; }

			tick_result do_tick_child(uint32_t i);
		};
		 
		//------------------------------------------------------------------------------------------------------------------------
		struct sfallback : public ibehavior_tree_context_holder
		{
			sfallback(cbehavior_tree& context) :
				ibehavior_tree_context_holder(context)
			{
			}

			tick_result do_tick()
			{
				logging::log_info("sfallback");

				for (const auto& i : m_children)
				{
					if (do_tick_child(i) == tick_result_ok)
					{
						return tick_result_ok;
					}
				}
				return tick_result_fail;
			}

			vector_t<uint32_t> m_children;

			RTTR_ENABLE();
		};

		REFLECT_INLINE(sfallback)
		{
			rttr::cregistrator<sfallback, rttr::detail::no_default>("sfallback")
				.ctor<rttr::detail::as_object, cbehavior_tree&>()
				.meth("do_tick", &sfallback::do_tick)
				;
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct ssequence : public ibehavior_tree_context_holder
		{
			ssequence(cbehavior_tree& context) :
				ibehavior_tree_context_holder(context)
			{
			}

			tick_result do_tick()
			{
				logging::log_info("ssequence");

				for (const auto& i : m_children)
				{
					if (do_tick_child(i) == tick_result_fail)
					{
						return tick_result_fail;
					}
				}
				return tick_result_ok;
			}

			vector_t<uint32_t> m_children;

			RTTR_ENABLE();
		};

		REFLECT_INLINE(ssequence)
		{
			rttr::cregistrator<ssequence, rttr::detail::no_default>("ssequence")
				.ctor<rttr::detail::as_object, cbehavior_tree&>()
				.meth("do_tick", &ssequence::do_tick)
				;
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct saction : public ibehavior_tree_context_holder
		{
			saction(cbehavior_tree& context) :
				ibehavior_tree_context_holder(context)
			{
			}

			tick_result do_tick()
			{
				logging::log_info("saction");
				return tick_result_ok;
			}

			RTTR_ENABLE();
		};

		REFLECT_INLINE(saction)
		{
			rttr::cregistrator<saction, rttr::detail::no_default>("saction")
				.ctor<rttr::detail::as_object, cbehavior_tree&>()
				.meth("do_tick", &saction::do_tick)
				;
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct scondition : public ibehavior_tree_context_holder
		{
			scondition(cbehavior_tree& context) :
				ibehavior_tree_context_holder(context)
			{
			}

			tick_result do_tick()
			{
				logging::log_info("scondition");
				return tick_result_ok;
			}

			RTTR_ENABLE();
		};

		REFLECT_INLINE(scondition)
		{
			rttr::cregistrator<scondition, rttr::detail::no_default>("scondition")
				.ctor<rttr::detail::as_object, cbehavior_tree&>()
				.meth("do_tick", &scondition::do_tick)
				;
		};

		//------------------------------------------------------------------------------------------------------------------------
		class cbehavior_tree
		{
		public:
			cbehavior_tree(stringview_t name) :
				m_name(name)
			{
			}

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
			void emplace_node(ARGS&&... args)
			{
				//- the new node should be emplaced as child of the root

				m_nodes.emplace_back<TNode>(*this, args...);
			}

			template<typename TNode>
			void emplace_node()
			{
				//- the new node should be emplaced as child of the root

				m_nodes.emplace_back<TNode>(*this);
			}

			void tick()
			{
				const auto& root = m_nodes[0];

				const auto& method = root.get_type().get_method("do_tick");

				method.invoke(root);
			}

			tick_result tick_node(uint32_t i)
			{
				const auto& node = m_nodes[i];

				const auto& method = node.get_type().get_method("do_tick");

				return method.invoke(node).convert<tick_result>();
			}

			std::string m_name;
			vector_t<rttr::instance> m_nodes;
		};

	} //- bt

} //- ai