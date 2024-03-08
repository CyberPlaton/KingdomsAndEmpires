#pragma once
#include "../context/editor_context.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class clayer_base : public ccontext_holder
	{
	public:
		clayer_base(ccontext& ctx) : ccontext_holder(ctx) {};
		virtual ~clayer_base() {}

		virtual bool init() {return false;}
		virtual void shutdown() {}

		virtual void on_update(float dt) {}
		virtual void on_world_render() {}
		virtual void on_ui_render() {}
		virtual void on_post_update(float dt) {}

	private:

	};

} //- editor