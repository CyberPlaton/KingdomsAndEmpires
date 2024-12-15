#pragma once
#include "../sm_config.hpp"

namespace sm
{
	namespace graphics
	{
		namespace detail
		{
			bool check_available_transient_buffers(unsigned vertex_count, const bgfx::VertexLayout& layout, unsigned index_count);

		} //- detail

		void reset_view(const vec2_t& screen_size, const bool vsync);
		void touch(renderpass_id_t id);
		void discard();
		void set_renderpass_order(const renderpass_order_t& order);

		void set_clear_color(renderpass_id_t id, const core::scolor& color);
		void set_clear_discard(renderpass_id_t id);

		void set_camera(renderpass_id_t id, const vec2_t& position, const vec2_t& scale, const float zoom, const float rotation);
		void set_viewport(renderpass_id_t id, const core::srect& rect);

		void set_shader(const cprogram& shader);
		void set_rendertarget(renderpass_id_t id, const crendertarget& target);

	} //- graphics

} //- sm
