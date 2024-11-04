#pragma once
#include "../context/editor_context.hpp"

namespace editor
{
	//- Base class for any UI entity, such as asset browser or entity inspector etc. For windows and elements that can be
	//- closed, use the 'active' function to indicate whether to close or not, thats important as other systems may depend on it.
	//-
	//- TODO: rename. Layer is a reserved name from engine and can lead to misunderstandings.
	//------------------------------------------------------------------------------------------------------------------------
	class clayer_base : public ccontext_holder
	{
	public:
		clayer_base(const char* id, scontext& ctx) : m_id(id), ccontext_holder(ctx) {};
		virtual ~clayer_base() {}

		virtual bool init()						{ CORE_ASSERT(false, "Invalid operation. Base class function must be implemented!"); return false; }
		virtual void shutdown()					{}
		virtual void on_update(float dt)		{}
		virtual void on_world_render()			{}
		virtual void on_ui_render()				{}
		virtual void on_post_update(float dt)	{}

		inline const char* id() const { return m_id; }
		inline bool& active() { return m_active; }

	private:
		const char* m_id = nullptr;
		bool m_active = true;
	};

	using layer_ref_t = ref_t<clayer_base>;

	namespace detail
	{
		template<typename T, typename... ARGS>
		layer_ref_t create_ui_element(ARGS&&... args) { return std::make_shared<T>(std::forward<ARGS>(args)...); }

	} //- detail

} //- editor