#include "sm_resource_manager.hpp"

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		template<typename TResource, typename THandle, typename... ARGS>
		core::cfuture_type<THandle> load_of_async(stringview_t name, umap_t<unsigned, TResource>& data, ARGS&&... args)
		{
			unsigned hash = algorithm::hash(name);

			core::cfuture_type<THandle> result = core::casync::launch_async([h=hash, d=data]() -> THandle
				{
					d.emplace(h, std::forward<ARGS>(args)...);

					return h;
				});

			return result;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TResource, typename THandle, typename... ARGS>
		THandle load_of_sync(stringview_t name, umap_t<unsigned, TResource>& data, ARGS&&... args)
		{
			unsigned hash = algorithm::hash(name);

			data.emplace(hash, std::forward<ARGS>(args)...);

			return THandle(hash);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TResource>
		void destroy_all(umap_t<unsigned, TResource>& data)
		{
			for (auto& pair : data)
			{
				TResource::destroy(pair.second);
			}
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cimage_manager::cimage_manager(unsigned reserve /*= 0*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage_manager::~cimage_manager()
	{
		destroy_all(m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::image_handle_t cimage_manager::load_sync(stringview_t name, stringview_t filepath)
	{
		return load_of_sync<cimage, image_handle_t>(name, m_data, filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::image_handle_t cimage_manager::load_sync(stringview_t name, image_type type, void* data, unsigned size)
	{
		return load_of_sync<cimage, image_handle_t>(name, m_data, type, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::image_handle_t> cimage_manager::load_async(stringview_t name, stringview_t filepath)
	{
		return load_of_async<cimage, image_handle_t>(name, m_data, filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::image_handle_t> cimage_manager::load_async(stringview_t name, image_type type, void* data, unsigned size)
	{
		return load_of_async<cimage, image_handle_t>(name, m_data, type, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture_manager::ctexture_manager(unsigned reserve /*= 0*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture_manager::~ctexture_manager()
	{
		destroy_all(m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::texture_handle_t ctexture_manager::load_sync(stringview_t name, const cimage& image)
	{
		return load_of_sync<ctexture, texture_handle_t>(name, m_data, image);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::texture_handle_t ctexture_manager::load_sync(stringview_t name, stringview_t filepath)
	{
		return load_of_sync<ctexture, texture_handle_t>(name, m_data, filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::texture_handle_t ctexture_manager::load_sync(stringview_t name, image_type type, void* data, unsigned size)
	{
		return load_of_sync<ctexture, texture_handle_t>(name, m_data, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::texture_handle_t> ctexture_manager::load_async(stringview_t name, const cimage& image)
	{
		return load_of_async<ctexture, texture_handle_t>(name, m_data, image);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::texture_handle_t> ctexture_manager::load_async(stringview_t name, stringview_t filepath)
	{
		return load_of_async<ctexture, texture_handle_t>(name, m_data, filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::texture_handle_t> ctexture_manager::load_async(stringview_t name, image_type type, void* data, unsigned size)
	{
		return load_of_async<ctexture, texture_handle_t>(name, m_data, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader_manager::cshader_manager(unsigned reserve /*= 0*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader_manager::~cshader_manager()
	{
		destroy_all(m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, shader_type type,
		stringview_t vertex_filepath, stringview_t fragment_filepath)
	{
		return load_of_sync<cshader, shader_handle_t>(name, m_data, type, vertex_filepath, fragment_filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, shader_type type, const char* vs, const char* fs)
	{
		return load_of_sync<cshader, shader_handle_t>(name, m_data, type, vs, fs);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, shader_type type,
		const uint8_t* vs, unsigned vs_size, const uint8_t* fs, unsigned fs_size)
	{
		return load_of_sync<cshader, shader_handle_t>(name, m_data, type, vs, vs_size, fs, fs_size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, shader_type type,
		stringview_t vertex_filepath, stringview_t fragment_filepath)
	{
		return load_of_async<cshader, shader_handle_t>(name, m_data, type, vertex_filepath, fragment_filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, shader_type type,
		const char* vs, const char* fs)
	{
		return load_of_async<cshader, shader_handle_t>(name, m_data, type, vs, fs);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, shader_type type,
		const uint8_t* vs, unsigned vs_size, const uint8_t* fs, unsigned fs_size)
	{
		return load_of_async<cshader, shader_handle_t>(name, m_data, type, vs, vs_size, fs, fs_size);
	}

} //- sm