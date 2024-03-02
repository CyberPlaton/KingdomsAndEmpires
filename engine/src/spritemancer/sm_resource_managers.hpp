#pragma once
#include <core.h>
#include "sm_common.hpp"

namespace sm
{
	constexpr material_t C_DEFAULT_MATERIAL = 0;

	namespace internal
	{
		class ccontext;

		//- Base resource manager. Handles common storage types, like retrieving a handle or unloading.
		//- Note: as of now the implementing resource manager has to respect correct creation behavior:
		//- In m_lookup emplace the name of the resource paired to its handle, whereas the handle is the
		//- index into m_resources where the underlying resource is stored.
		//------------------------------------------------------------------------------------------------------------------------
		template<typename TResourceHandleType, typename TNativeResourceType>
		class cresource_manager
		{
			friend class ccontext;
		public:
			cresource_manager(ccontext& ctx) : m_context(ctx), m_count(0), m_fragmentations(0) {};
			virtual ~cresource_manager();

			TResourceHandleType get(stringview_t name) const;
			void unload(TResourceHandleType handle);

		private:
			struct swrapper
			{
				swrapper(TNativeResourceType resource) : m_resource(std::move(resource)) {}

				TNativeResourceType m_resource;
				bool m_removed = false;
			};

		protected:
			ccontext& m_context;
			vector_t<swrapper> m_resources;
			umap_t<unsigned, TResourceHandleType> m_lookup;

		private:
			unsigned m_count;
			unsigned m_fragmentations;

		protected:
			const TNativeResourceType& native_resource(TResourceHandleType handle) const;
			inline bool fragmented() const { return m_fragmentations > 0; }
			inline TResourceHandleType increment() { auto out = SCAST(TResourceHandleType, m_count++); return out; }
			inline void decrement() { m_fragmentations = (m_fragmentations - 1 < 0) ? 0 m_fragmentations - 1; }
			unsigned fragmentation_slot();
			ccontext& ctx() {return m_context;}

			virtual void on_resource_unload(TNativeResourceType& resource) {};

		};

		//- Manager responsible for sprite atlases, aka texture atlases
		//------------------------------------------------------------------------------------------------------------------------
		class cspriteatlas_manager :
			public cresource_manager<spriteatlas_t, csprite_atlas>
		{
		public:
			cspriteatlas_manager(ccontext& ctx) : cresource_manager<spriteatlas_t, csprite_atlas>(ctx) {}

			spriteatlas_t create(stringview_t spriteatlasname, stringview_t texturepath, const vec2_t& frames);
			core::srect subtexture(spriteatlas_t atlas, stringview_t name) const;

		protected:
			void on_resource_unload(csprite_atlas& resource) override final;
		};

		//- Manager responsible for techniques, aka shaders
		//------------------------------------------------------------------------------------------------------------------------
		class ctechnique_manager :
			public cresource_manager<technique_t, raylib::Shader>
		{
		public:
			ctechnique_manager(ccontext& ctx) : cresource_manager<technique_t, raylib::Shader>(ctx) {}

			technique_t create(stringview_t name, stringview_t vspath, stringview_t pspath);
			technique_t create_embedded(stringview_t name, stringview_t vsdata, stringview_t psdata);

		protected:
			void on_resource_unload(raylib::Shader& resource) override final;
		};

		//- Manager responsible for materials
		//------------------------------------------------------------------------------------------------------------------------
		class cmaterial_manager :
			public cresource_manager<material_t, cmaterial>
		{
		public:
			cmaterial_manager(ccontext& ctx) : cresource_manager<material_t, cmaterial>(ctx) {}

			material_t create(stringview_t name, technique_t technique, blending_mode mode = blending_mode_alpha,
				blending_equation equation = blending_equation_blend_color, blending_factor src = blending_factor_src_color,
				blending_factor dst = blending_factor_one_minus_src_alpha);

			inline material_t default_material() const { return C_DEFAULT_MATERIAL; }

		protected:
			void on_resource_unload(cmaterial& resource) override final;

		private:
			bool create_default(technique_t technique, blending_mode mode = blending_mode_alpha, blending_equation equation = blending_equation_blend_color,
				blending_factor src = blending_factor_src_color, blending_factor dst = blending_factor_one_minus_src_alpha);
		};

		//- Manager responsible for textures
		//------------------------------------------------------------------------------------------------------------------------
		class ctexture_manager :
			public cresource_manager<texture_t, raylib::Texture2D>
		{
		public:
			ctexture_manager(ccontext& ctx) : cresource_manager<texture_t, raylib::Texture2D>(ctx) {}

			texture_t create(stringview_t name, stringview_t path);
			const raylib::Texture2D* native(stringview_t name) const;
			vec2_t texture_size(texture_t texture) const;

		protected:
			void on_resource_unload(raylib::Texture2D& resource) override final;
		};

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TResourceHandleType, typename TNativeResourceType>
		unsigned cresource_manager<TResourceHandleType, TNativeResourceType>::fragmentation_slot()
		{
			if (auto it = algorithm::find_if(m_resources.begin(), m_resources.end(),
				[this](const TNativeResourceType& resource)
				{
					return resource.m_removed;
				}); it != m_resources.end())
			{
				return *it;
			}
			return std::numeric_limits<unsigned>().max();
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TResourceHandleType, typename TNativeResourceType>
		const TNativeResourceType& cresource_manager<TResourceHandleType, TNativeResourceType>::native_resource(TResourceHandleType handle) const
		{
			return m_resources[handle].m_resource;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TResourceHandleType, typename TNativeResourceType>
		void cresource_manager<TResourceHandleType, TNativeResourceType>::unload(TResourceHandleType handle)
		{
			auto& wrapper = m_resources[handle];

			//- handle underlying unload of resource specific to manager
			on_resource_unload(wrapper);

			algorithm::erase_if(m_lookup, [&](auto it)
				{
					return it->second == handle;
				});

			--m_count;
			++m_fragmentations;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TResourceHandleType, typename TNativeResourceType>
		cresource_manager<TResourceHandleType, TNativeResourceType>::~cresource_manager()
		{
			while (!m_resources.empty())
			{
				on_resource_unload(m_resources.back());

				m_resources.pop_back();
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TResourceHandleType, typename TNativeResourceType>
		TResourceHandleType cresource_manager<TResourceHandleType, TNativeResourceType>::get(stringview_t name) const
		{
			if (auto it = algorithm::find_at(m_lookup.begin(), m_lookup.end(), algorithm::hash(name));
				it != m_lookup.end())
			{
				return *it;
			}
			return invalid_handle_t;
		}

	} //- internal

} //- sm