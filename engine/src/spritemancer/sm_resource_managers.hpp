#pragma once
#include <core.h>
#include "sm_common.hpp"

namespace sm
{
	namespace internal
	{
		//- Manager responsible for sprite atlases, aka texture atlases
		//------------------------------------------------------------------------------------------------------------------------
		class cspriteatlas_manager :
			public core::cresource_manager<spriteatlas_t, csprite_atlas>
		{
		public:
			cspriteatlas_manager() {}

			spriteatlas_t create(stringview_t spriteatlasname, stringview_t texturepath, const vec2_t& frames);
			core::srect subtexture(spriteatlas_t atlas, stringview_t name) const;

		protected:
			void on_resource_unload(csprite_atlas& resource) override final;
			bool init() override final;

			RTTR_ENABLE(core::cresource_manager<spriteatlas_t, csprite_atlas>);
		};

		//- Manager responsible for techniques, aka shaders
		//------------------------------------------------------------------------------------------------------------------------
		class ctechnique_manager :
			public core::cresource_manager<technique_t, raylib::Shader>
		{
		public:
			ctechnique_manager() {}

			technique_t create(stringview_t name, stringview_t vspath, stringview_t pspath);
			technique_t create_embedded(stringview_t name, stringview_t vsdata, stringview_t psdata);

		protected:
			void on_resource_unload(raylib::Shader& resource) override final;
			bool init() override final;

			RTTR_ENABLE(core::cresource_manager<technique_t, raylib::Shader>);
		};

		//- Manager responsible for materials
		//------------------------------------------------------------------------------------------------------------------------
		class cmaterial_manager :
			public core::cresource_manager<material_t, cmaterial>
		{
			friend class ctechnique_manager;
		public:
			cmaterial_manager() {}

			material_t create(stringview_t name, technique_t technique, blending_mode mode = blending_mode_alpha,
				blending_equation equation = blending_equation_blend_color, blending_factor src = blending_factor_src_color,
				blending_factor dst = blending_factor_one_minus_src_alpha);

			inline material_t default_material() const { return C_DEFAULT_MATERIAL; }

		protected:
			void on_resource_unload(cmaterial& resource) override final;
			bool init() override final;

		private:
			bool create_default(technique_t technique, blending_mode mode = blending_mode_alpha, blending_equation equation = blending_equation_blend_color,
				blending_factor src = blending_factor_src_color, blending_factor dst = blending_factor_one_minus_src_alpha);

			RTTR_ENABLE(core::cresource_manager<material_t, cmaterial>);
		};

		//- Manager responsible for textures
		//------------------------------------------------------------------------------------------------------------------------
		class ctexture_manager :
			public core::cresource_manager<texture_t, raylib::Texture2D>
		{
		public:
			ctexture_manager() {}

			texture_t create(stringview_t name, stringview_t path);
			const raylib::Texture2D* native(stringview_t name) const;
			vec2_t texture_size(texture_t texture) const;

		protected:
			void on_resource_unload(raylib::Texture2D& resource) override final;
			bool init() override final;

			RTTR_ENABLE(core::cresource_manager<texture_t, raylib::Texture2D>);
		};

	} //- internal

} //- sm