#pragma once
#include "sm_config.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class cimage_manager final :
		public core::cservice,
		public core::cresource_manager<cimage>
	{
	public:
		cimage_manager(unsigned reserve = C_IMAGE_RESOURCE_MANAGER_RESERVE_COUNT);
		~cimage_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		image_handle_t load_sync(stringview_t name, stringview_t filepath);
		image_handle_t load_sync(stringview_t name, void* data, unsigned size);

		core::cfuture_type<image_handle_t> load_async(stringview_t name, stringview_t filepath);
		core::cfuture_type<image_handle_t> load_async(stringview_t name, void* data, unsigned size);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cimage>);
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

	//------------------------------------------------------------------------------------------------------------------------
	class cshader_manager final :
		public core::cservice,
		public core::cresource_manager<cshader>
	{
	public:
		cshader_manager(unsigned reserve = C_SHADER_RESOURCE_MANAGER_RESERVE_COUNT);
		~cshader_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		shader_handle_t load_sync(stringview_t name, stringview_t path);
		shader_handle_t load_sync(stringview_t name, const char* text);
		shader_handle_t load_sync(stringview_t name, const uint8_t* data, unsigned size);

		core::cfuture_type<shader_handle_t> load_async(stringview_t name, stringview_t path);
		core::cfuture_type<shader_handle_t> load_async(stringview_t name, const char* text);
		core::cfuture_type<shader_handle_t> load_async(stringview_t name, const uint8_t* data, unsigned size);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cshader>);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cprogram_manager final :
		public core::cservice,
		public core::cresource_manager<cprogram>
	{
	public:
		cprogram_manager(unsigned reserve = C_PROGRAM_RESOURCE_MANAGER_RESERVE_COUNT);
		~cprogram_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		program_handle_t load_sync(stringview_t name, const bgfx::ShaderHandle vs, const bgfx::ShaderHandle fs);
		program_handle_t load_sync(stringview_t name, const cshader& vs, const cshader& fs);

		core::cfuture_type<program_handle_t> load_async(stringview_t name, const bgfx::ShaderHandle vs, const bgfx::ShaderHandle fs);
		core::cfuture_type<program_handle_t> load_async(stringview_t name, const cshader& vs, const cshader& fs);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cprogram>);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cspriteatlas_manager final :
		public core::cservice,
		public core::cresource_manager<cspriteatlas>
	{
	public:
		cspriteatlas_manager(unsigned reserve = C_SPRITEATLAS_RESOURCE_MANAGER_RESERVE_COUNT);
		~cspriteatlas_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		spriteatlas_handle_t load_sync(stringview_t name, unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames);

		core::cfuture_type<spriteatlas_handle_t> load_async(stringview_t name, unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cspriteatlas>);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class crendertarget_manager final :
		public core::cservice,
		public core::cresource_manager<crendertarget>
	{
	public:
		crendertarget_manager(unsigned reserve = C_RENDERTARGET_RESOURCE_MANAGER_RESERVE_COUNT);
		~crendertarget_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		rendertarget_handle_t load_sync(stringview_t name, unsigned w, unsigned h);

		core::cfuture_type<rendertarget_handle_t> load_async(stringview_t name, unsigned w, unsigned h);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<crendertarget>);
	};

} //- sm