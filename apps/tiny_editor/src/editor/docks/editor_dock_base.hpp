#pragma once
#include "../context/editor_context.hpp"

namespace editor
{
	//- Base class for any UI entity, such as asset browser or entity inspector etc.
	//------------------------------------------------------------------------------------------------------------------------
	class clayer_base : public ccontext_holder
	{
	public:
		clayer_base(scontext& ctx) : ccontext_holder(ctx) {};
		virtual ~clayer_base() {}

		virtual bool init()						{ CORE_ASSERT(false, "Invalid operation. Base class function must be implemented!"); return false; }
		virtual void shutdown()					{}
		virtual void on_update(float dt)		{}
		virtual void on_world_render()			{}
		virtual void on_ui_render()				{}
		virtual void on_post_update(float dt)	{}

	private:
	};

	using layer_ref_t = ref_t<clayer_base>;

	namespace detail
	{
		template<typename T, typename... ARGS>
		layer_ref_t create_ui_element(ARGS&&... args) { return std::make_shared<T>(std::forward<ARGS>(args)...); }

	} //- detail

} //- editor