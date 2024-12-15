#include "bgfx_integration.hpp"

namespace sm
{
	namespace graphics
	{
		namespace detail
		{
			//------------------------------------------------------------------------------------------------------------------------
			bool check_available_transient_buffers(unsigned vertex_count, const bgfx::VertexLayout& layout, unsigned index_count)
			{
				return vertex_count == bgfx::getAvailTransientVertexBuffer(vertex_count, layout) &&
					(0 == index_count || index_count == bgfx::getAvailTransientIndexBuffer(index_count));
			}

		} //- detail

		//------------------------------------------------------------------------------------------------------------------------
		void reset_view(const vec2_t& screen_size, const bool vsync)
		{
			const auto flags = vsync ? BGFX_RESET_VSYNC | BGFX_RESET_FLIP_AFTER_RENDER : BGFX_RESET_NONE;

			bgfx::reset(static_cast<uint16_t>(screen_size.x), static_cast<uint16_t>(screen_size.y), flags);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void touch(renderpass_id_t id)
		{
			bgfx::touch(id);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void discard()
		{
			bgfx::discard();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_renderpass_order(const renderpass_order_t& order)
		{
			bgfx::setViewOrder(0, static_cast<uint16_t>(order.size()), order.data());
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_clear_color(renderpass_id_t id, const core::scolor& color)
		{
			bgfx::setViewClear(id, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, color.abgr(), 1.0f, 0);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_clear_discard(renderpass_id_t id)
		{
			bgfx::setViewClear(id, BGFX_CLEAR_DISCARD_DEPTH | BGFX_CLEAR_DISCARD_STENCIL);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_camera(renderpass_id_t id, const vec2_t& position, const vec2_t& scale, const float zoom, const float rotation)
		{
			const auto view = glm::translate(math::C_MAT4_ID, -vec3_t(position, 1.0f))
				* glm::rotate(math::C_MAT4_ID, glm::radians(rotation), vec3_t(0.0f, 0.0f, 1.0f))
				* glm::scale(math::C_MAT4_ID, vec3_t(scale, 1.0f));

			const auto projection = glm::ortho(-zoom, zoom, -zoom, zoom);

			bgfx::setViewTransform(id, glm::value_ptr(view), glm::value_ptr(projection));
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_viewport(renderpass_id_t id, const core::srect& rect)
		{
			bgfx::setViewRect(id, static_cast<uint16_t>(rect.x()), static_cast<uint16_t>(rect.y()),
				static_cast<uint16_t>(rect.w()), static_cast<uint16_t>(rect.h()));
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_shader(const cprogram& shader)
		{
			ctx()->graphics().m_shader = shader.handle();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_rendertarget(renderpass_id_t id, const crendertarget& target)
		{
			bgfx::setViewFrameBuffer(id, target);
		}

	} //- graphics

} //- sm