#pragma once
#include <core.h>
#include <engine.h>

namespace editor
{
	//- Central access to editor for all deeper seated layers. Values inside are modified by the root editor layer.
	//------------------------------------------------------------------------------------------------------------------------
	class ccontext : core::cnon_copyable
	{
	public:
		ccontext();
		~ccontext() = default;

		float window_width();
		float window_height();

	public:
		core::cuuid m_inspected_entity;
		float m_mainmenu_height;
		bool m_docks_enabled = true;
	};

	//- Access to context.
	//------------------------------------------------------------------------------------------------------------------------
	class ccontext_holder
	{
	public:
		ccontext_holder(ccontext& ctx) : m_ctx(ctx) {}
		virtual ~ccontext_holder() {}

	protected:
		inline const ccontext& ctx() const {return m_ctx;}
		inline ccontext& ctx() {return m_ctx;}

	private:
		ccontext& m_ctx;
	};

} //- editor