#pragma once
#include "sm_config.hpp"

namespace sm
{
	//- GPU resident image representation.
	//- TODO: information does not need to be held by each texture. Store it somewhere else and in texture only the handle.
	//------------------------------------------------------------------------------------------------------------------------
	class ctexture final : public core::cresource
	{
	public:
		explicit ctexture(const cimage& image);
		explicit ctexture(stringview_t filepath);
		explicit ctexture(void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
			bool mips, unsigned layers, texture_format format, uint64_t flags);
		explicit ctexture(texture_handle_t handle, const bgfx::TextureInfo& info);
		ctexture();
		~ctexture();


		opresult load_from_image(const cimage& image);
		opresult load_from_file(stringview_t filepath);
		opresult load_from_memory(void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
			bool mips, unsigned layers, texture_format format, uint64_t flags);

		inline unsigned w() const { return SCAST(unsigned, m_info.width); }
		inline unsigned h() const { return SCAST(unsigned, m_info.height); }
		inline texture_handle_t texture() const { return m_texture; }

		operator bgfx::TextureHandle() const noexcept { return bgfx::TextureHandle{ SCAST(uint16_t, m_texture) }; }

	private:
		texture_handle_t m_texture;
		bgfx::TextureInfo m_info;

		RTTR_ENABLE(core::cresource);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class ctexture_manager final :
		public core::cservice,
		public core::cresource_manager<ctexture>
	{
	public:
		ctexture_manager(unsigned reserve = C_TEXTURE_RESOURCE_MANAGER_RESERVE_COUNT);
		~ctexture_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		texture_handle_t load_sync(stringview_t name, const cimage& image);
		texture_handle_t load_sync(stringview_t name, stringview_t filepath);
		texture_handle_t load_sync(stringview_t name, void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
			bool mips, unsigned layers, texture_format format, uint64_t flags);

		core::cfuture_type<texture_handle_t> load_async(stringview_t name, const cimage& image);
		core::cfuture_type<texture_handle_t> load_async(stringview_t name, stringview_t filepath);
		core::cfuture_type<texture_handle_t> load_async(stringview_t name, void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
			bool mips, unsigned layers, texture_format format, uint64_t flags);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<ctexture>);
	};

} //- sm