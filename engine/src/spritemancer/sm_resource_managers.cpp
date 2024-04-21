#include "sm_resource_managers.hpp"

namespace sm
{
	namespace
	{

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	ctexture_manager::ctexture_manager()
	{
		CORE_ASSERT(m_textures.init(C_TEXTURE_RESERVE_COUNT), "Invalid operation. Failed initializing texture manager!");
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture_manager::~ctexture_manager()
	{
		m_textures.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	texture_t ctexture_manager::load(stringview_t name, stringview_t path)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			texture_t handle = invalid_handle_t;

			if (const auto tex = raylib::LoadTexture(path.data()); raylib::IsTextureReady(tex))
			{
				auto* texture = m_textures.create(&handle, tex);

				//- texture should be loaded at this point, so we can store it
				CORE_ASSERT(texture && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Texture!");

				m_handles[name.data()] = handle;
			}
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	texture_t ctexture_manager::at(stringview_t name) const
	{
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctexture_manager::unload(texture_t handle)
	{
		m_textures.destroy(handle);

		algorithm::erase_if(m_handles,
			[&](const auto& pair)
			{
				return pair->second == handle;
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	const ctexture& ctexture_manager::get(texture_t handle) const
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_textures.find(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::ctexture& ctexture_manager::get(texture_t handle)
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_textures.modify(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader_manager::cshader_manager()
	{
		CORE_ASSERT(m_shaders.init(C_TECHNIQUE_RESERVE_COUNT), "Invalid operation. Failed initializing shader manager!");
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader_manager::~cshader_manager()
	{
		m_shaders.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	technique_t cshader_manager::load(stringview_t name, stringview_t vspath, stringview_t fspath)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			technique_t handle = invalid_handle_t;

			if (const auto sha = raylib::LoadShader(vspath.data(), fspath.data()); raylib::IsShaderReady(sha))
			{
				auto* shader = m_shaders.create(&handle, sha);

				//- shader should be loaded at this point, so we can store it
				CORE_ASSERT(shader && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Shader!");

				m_handles[name.data()] = handle;
			}
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	technique_t cshader_manager::load_from_memory(stringview_t name, const char* vs, const char* fs)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			technique_t handle = invalid_handle_t;

			if (const auto sha = raylib::LoadShaderFromMemory(vs, fs); raylib::IsShaderReady(sha))
			{
				auto* shader = m_shaders.create(&handle, sha);

				CORE_ASSERT(shader && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Shader!");

				m_handles[name.data()] = handle;
			}
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader_manager::unload(technique_t handle)
	{
		m_shaders.destroy(handle);

		algorithm::erase_if(m_handles,
			[&](const auto& pair)
			{
				return pair->second == handle;
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	technique_t cshader_manager::at(stringview_t name) const
	{
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	const ctechnique& cshader_manager::get(technique_t handle) const
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_shaders.find(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::ctechnique& cshader_manager::get(technique_t handle)
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_shaders.modify(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_manager::cmaterial_manager()
	{
		CORE_ASSERT(m_materials.init(C_MATERIAL_RESERVE_COUNT), "Invalid operation. Failed initializing material manager!");
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_manager::~cmaterial_manager()
	{
		m_materials.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	material_t cmaterial_manager::load(stringview_t name, technique_t technique, blending_mode mode /*= blending_mode_alpha*/,
		blending_equation equation /*= blending_equation_blend_color*/, blending_factor src /*= blending_factor_src_color*/,
		blending_factor dst /*= blending_factor_one_minus_src_alpha*/)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			material_t handle = invalid_handle_t;

			//- If shader is not valid we crash here
			auto* sm = core::cservice_manager::find<cshader_manager>();
			auto& shader = sm->get(technique);

			auto* mat = m_materials.create(&handle, shader, technique, mode, equation, src, dst);

			CORE_ASSERT(mat && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Material!");

			m_handles[name.data()] = handle;
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	material_t cmaterial_manager::at(stringview_t name) const
	{
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_manager::unload(material_t handle)
	{
		m_materials.destroy(handle);

		algorithm::erase_if(m_handles,
			[&](const auto& pair)
			{
				return pair->second == handle;
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	const sm::cmaterial& cmaterial_manager::get(material_t handle) const
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_materials.find(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cspriteatlas_manager::cspriteatlas_manager()
	{
		CORE_ASSERT(m_spritesheets.init(C_SPRITEATLAS_RESERVE_COUNT), "Invalid operation. Failed initializing spriteatlas manager!");
	}

	//------------------------------------------------------------------------------------------------------------------------
	cspriteatlas_manager::~cspriteatlas_manager()
	{
		m_spritesheets.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	spriteatlas_t cspriteatlas_manager::load(stringview_t name, texture_t texture, const vec2_t& frames)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			spriteatlas_t handle = invalid_handle_t;

			auto* sheet = m_spritesheets.create(&handle, texture);

			CORE_ASSERT(sheet && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Spriteatlas!");

			m_handles[name.data()] = handle;

			//- initialize sprite atlas. We will crash here if the texture does not exist
			auto* tm = core::cservice_manager::find<ctexture_manager>();
			const auto& tex = tm->get(texture).texture();

			const auto texture_size = vec2_t(tex.width, tex.height);
			const auto frame_size = vec2_t(texture_size.x / frames.x, texture_size.y / frames.y);

			sheet->begin(texture);

			for (auto y = 0u; y < frames.y; ++y)
			{
				for (auto x = 0u; x < frames.x; ++x)
				{
					const auto px = x * frame_size.x / texture_size.x;
					const auto py = y * frame_size.y / texture_size.y;
					const auto w = frame_size.x / texture_size.x;
					const auto h = frame_size.y / texture_size.y;

					sheet->subtexture({ px, py, w, h });
				}
			}

			sheet->end();
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cspriteatlas_manager::unload(material_t handle)
	{
		m_spritesheets.destroy(handle);

		algorithm::erase_if(m_handles,
			[&](const auto& pair)
			{
				return pair->second == handle;
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	const sm::cspriteatlas& cspriteatlas_manager::get(spriteatlas_t handle) const
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_spritesheets.find(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget_manager::crendertarget_manager()
	{
		CORE_ASSERT(m_rendertargets.init(C_RENDERTARGET_RESERVE_COUNT), "Invalid operation. Failed initializing rendertarget manager!");
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget_manager::~crendertarget_manager()
	{
		m_rendertargets.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	rendertarget_t crendertarget_manager::load(stringview_t name, unsigned width, unsigned height)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			rendertarget_t handle = invalid_handle_t;

			auto* rt = m_rendertargets.create(&handle, width, height);

			CORE_ASSERT(rt && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Rendertarget!");

			m_handles[name.data()] = handle;
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crendertarget_manager::unload(rendertarget_t handle)
	{
		m_rendertargets.destroy(handle);

		algorithm::erase_if(m_handles,
			[&](const auto& pair)
			{
				return pair->second == handle;
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	const sm::crendertarget& crendertarget_manager::get(rendertarget_t handle) const
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_rendertargets.find(handle);
	}

} //- sm