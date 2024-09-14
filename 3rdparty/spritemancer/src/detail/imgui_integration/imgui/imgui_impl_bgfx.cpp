#include "imgui_impl_bgfx.h"
#include "../../bgfx_integration/bgfx.hpp"
#include "droidsans.ttf.h"
#include "roboto_regular.ttf.h"
#include "robotomono_regular.ttf.h"
#include "icons_kenney.ttf.h"
#include "icons_kenney.h"
#include "IconsFontAwesome6.h"
#include "FA6FreeSolidFontData.h"

#include "vs_ocornut_imgui.bin.h"
#include "fs_ocornut_imgui.bin.h"
#include "vs_imgui_image.bin.h"
#include "fs_imgui_image.bin.h"

namespace
{
	//------------------------------------------------------------------------------------------------------------------------
	static const bgfx::EmbeddedShader s_embeddedShaders[] =
	{
		BGFX_EMBEDDED_SHADER(vs_ocornut_imgui),
		BGFX_EMBEDDED_SHADER(fs_ocornut_imgui),
		BGFX_EMBEDDED_SHADER(vs_imgui_image),
		BGFX_EMBEDDED_SHADER(fs_imgui_image),

		BGFX_EMBEDDED_SHADER_END()
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct FontRangeMerge
	{
		const void* data;
		size_t      size;
		ImWchar     ranges[3];
	};

	//------------------------------------------------------------------------------------------------------------------------
	static FontRangeMerge s_fontRangeMerge[] =
	{
		{ s_iconsKenneyTtf,				sizeof(s_iconsKenneyTtf),				{ ICON_MIN_KI, ICON_MAX_KI, 0 } },
	};

	//------------------------------------------------------------------------------------------------------------------------
	static void* memAlloc(size_t _size, void* _userData);

	//------------------------------------------------------------------------------------------------------------------------
	static void memFree(void* _ptr, void* _userData);

	//------------------------------------------------------------------------------------------------------------------------
	struct simgui_context
	{
		void render(ImDrawData* _drawData);
		void create(float _fontSize, bx::AllocatorI* _allocator);
		void destroy();
		void setupStyle(const bool _dark);
		void endFrame();

		ImGuiContext* m_imgui;
		bx::AllocatorI* m_allocator;
		bgfx::VertexLayout  m_layout;
		bgfx::ProgramHandle m_program;
		bgfx::ProgramHandle m_imageProgram;
		bgfx::TextureHandle m_texture;
		bgfx::UniformHandle s_tex;
		bgfx::UniformHandle u_imageLodEnabled;
		ImFont* m_font[ImGui::Font::Count];
		int64_t m_last;
		int m_lastScroll;
		bgfx::ViewId m_viewId;
	};

	//------------------------------------------------------------------------------------------------------------------------
	void simgui_context::render(ImDrawData* _drawData)
	{
		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		int32_t dispWidth = _drawData->DisplaySize.x * _drawData->FramebufferScale.x;
		int32_t dispHeight = _drawData->DisplaySize.y * _drawData->FramebufferScale.y;
		if (dispWidth <= 0
			|| dispHeight <= 0)
		{
			return;
		}

		bgfx::setViewName(m_viewId, "ImGui");
		bgfx::setViewMode(m_viewId, bgfx::ViewMode::Sequential);

		const bgfx::Caps* caps = bgfx::getCaps();
		{
			float ortho[16];
			float x = _drawData->DisplayPos.x;
			float y = _drawData->DisplayPos.y;
			float width = _drawData->DisplaySize.x;
			float height = _drawData->DisplaySize.y;

			bx::mtxOrtho(ortho, x, x + width, y + height, y, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
			bgfx::setViewTransform(m_viewId, NULL, ortho);
			bgfx::setViewRect(m_viewId, 0, 0, uint16_t(width), uint16_t(height));
		}

		const ImVec2 clipPos = _drawData->DisplayPos;       // (0,0) unless using multi-viewports
		const ImVec2 clipScale = _drawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

		// Render command lists
		for (int32_t ii = 0, num = _drawData->CmdListsCount; ii < num; ++ii)
		{
			bgfx::TransientVertexBuffer tvb;
			bgfx::TransientIndexBuffer tib;

			const ImDrawList* drawList = _drawData->CmdLists[ii];
			uint32_t numVertices = (uint32_t)drawList->VtxBuffer.size();
			uint32_t numIndices = (uint32_t)drawList->IdxBuffer.size();

			if (!bgfx::checkAvailTransientBuffers(numVertices, m_layout, numIndices))
			{
				// not enough space in transient buffer just quit drawing the rest...
				break;
			}

			bgfx::allocTransientVertexBuffer(&tvb, numVertices, m_layout);
			bgfx::allocTransientIndexBuffer(&tib, numIndices, sizeof(ImDrawIdx) == 4);

			ImDrawVert* verts = (ImDrawVert*)tvb.data;
			bx::memCopy(verts, drawList->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert));

			ImDrawIdx* indices = (ImDrawIdx*)tib.data;
			bx::memCopy(indices, drawList->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx));

			bgfx::Encoder* encoder = bgfx::begin();

			for (const ImDrawCmd* cmd = drawList->CmdBuffer.begin(), *cmdEnd = drawList->CmdBuffer.end(); cmd != cmdEnd; ++cmd)
			{
				if (cmd->UserCallback)
				{
					cmd->UserCallback(drawList, cmd);
				}
				else if (0 != cmd->ElemCount)
				{
					uint64_t state = 0
						| BGFX_STATE_WRITE_RGB
						| BGFX_STATE_WRITE_A
						| BGFX_STATE_MSAA
						;

					bgfx::TextureHandle th = m_texture;
					bgfx::ProgramHandle program = m_program;

					if (NULL != cmd->TextureId)
					{
						union { ImTextureID ptr; struct { bgfx::TextureHandle handle; uint8_t flags; uint8_t mip; } s; } texture = { cmd->TextureId };

						state |= 0 != (IMGUI_FLAGS_ALPHA_BLEND & texture.s.flags)
							? BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
							: BGFX_STATE_NONE
							;
						th = texture.s.handle;

						if (0 != texture.s.mip)
						{
							const float lodEnabled[4] = { float(texture.s.mip), 1.0f, 0.0f, 0.0f };
							bgfx::setUniform(u_imageLodEnabled, lodEnabled);
							program = m_imageProgram;
						}
					}
					else
					{
						state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
					}

					// Project scissor/clipping rectangles into framebuffer space
					ImVec4 clipRect;
					clipRect.x = (cmd->ClipRect.x - clipPos.x) * clipScale.x;
					clipRect.y = (cmd->ClipRect.y - clipPos.y) * clipScale.y;
					clipRect.z = (cmd->ClipRect.z - clipPos.x) * clipScale.x;
					clipRect.w = (cmd->ClipRect.w - clipPos.y) * clipScale.y;

					if (clipRect.x < dispWidth
						&& clipRect.y < dispHeight
						&& clipRect.z >= 0.0f
						&& clipRect.w >= 0.0f)
					{
						const uint16_t xx = uint16_t(bx::max(clipRect.x, 0.0f));
						const uint16_t yy = uint16_t(bx::max(clipRect.y, 0.0f));
						encoder->setScissor(xx, yy
							, uint16_t(bx::min(clipRect.z, 65535.0f) - xx)
							, uint16_t(bx::min(clipRect.w, 65535.0f) - yy)
						);

						encoder->setState(state);
						encoder->setTexture(0, s_tex, th);
						encoder->setVertexBuffer(0, &tvb, cmd->VtxOffset, numVertices);
						encoder->setIndexBuffer(&tib, cmd->IdxOffset, cmd->ElemCount);
						encoder->submit(m_viewId, program);
					}
				}
			}

			bgfx::end(encoder);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void simgui_context::create(float _fontSize, bx::AllocatorI* _allocator)
	{
		IMGUI_CHECKVERSION();

		m_allocator = _allocator;

		if (NULL == _allocator)
		{
			static bx::DefaultAllocator allocator;
			m_allocator = &allocator;
		}

		m_viewId = 255;
		m_lastScroll = 0;
		m_last = bx::getHPCounter();

		ImGui::SetAllocatorFunctions(memAlloc, memFree, NULL);

		m_imgui = ImGui::GetCurrentContext();

		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2(1280.0f, 720.0f);
		io.DeltaTime = 1.0f / 60.0f;
		io.IniFilename = NULL;

		setupStyle(true);

		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

		bgfx::RendererType::Enum type = bgfx::getRendererType();
		m_program = bgfx::createProgram(
			bgfx::createEmbeddedShader(s_embeddedShaders, type, "vs_ocornut_imgui")
			, bgfx::createEmbeddedShader(s_embeddedShaders, type, "fs_ocornut_imgui")
			, true
		);

		u_imageLodEnabled = bgfx::createUniform("u_imageLodEnabled", bgfx::UniformType::Vec4);
		m_imageProgram = bgfx::createProgram(
			bgfx::createEmbeddedShader(s_embeddedShaders, type, "vs_imgui_image")
			, bgfx::createEmbeddedShader(s_embeddedShaders, type, "fs_imgui_image")
			, true
		);

		m_layout
			.begin()
			.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.end();

		s_tex = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);

		uint8_t* data;
		int32_t width;
		int32_t height;
		{
			ImFontConfig config;
			config.FontDataOwnedByAtlas = false;
			config.MergeMode = false;

			const ImWchar* ranges = io.Fonts->GetGlyphRangesCyrillic();
			m_font[ImGui::Font::Regular] = io.Fonts->AddFontFromMemoryTTF((void*)s_robotoRegularTtf, sizeof(s_robotoRegularTtf), _fontSize, &config, ranges);
			m_font[ImGui::Font::Mono] = io.Fonts->AddFontFromMemoryTTF((void*)s_robotoMonoRegularTtf, sizeof(s_robotoMonoRegularTtf), _fontSize - 3.0f, &config, ranges);

			config.MergeMode = true;
			config.DstFont = m_font[ImGui::Font::Regular];

			for (uint32_t ii = 0; ii < BX_COUNTOF(s_fontRangeMerge); ++ii)
			{
				const FontRangeMerge& frm = s_fontRangeMerge[ii];

				io.Fonts->AddFontFromMemoryTTF((void*)frm.data
					, (int)frm.size
					, _fontSize - 3.0f
					, &config
					, frm.ranges
				);
			}

			//- Adding icons in compressed form
			static const ImWchar compressed_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
			io.Fonts->AddFontFromMemoryCompressedTTF((void*)fa_solid_900_compressed_data
				, fa_solid_900_compressed_size
				, _fontSize - 3.0f
				, &config
				, compressed_ranges);
		}

		io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

		m_texture = bgfx::createTexture2D(
			(uint16_t)width
			, (uint16_t)height
			, false
			, 1
			, bgfx::TextureFormat::BGRA8
			, 0
			, bgfx::copy(data, width * height * 4)
		);

		ImGui::SetCurrentFont(m_font[ImGui::Font::Regular]);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void simgui_context::destroy()
	{
		m_imgui = nullptr;

		bgfx::destroy(s_tex);
		bgfx::destroy(m_texture);

		bgfx::destroy(u_imageLodEnabled);
		bgfx::destroy(m_imageProgram);
		bgfx::destroy(m_program);

		m_allocator = NULL;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void simgui_context::setupStyle(const bool _dark)
	{
		// Doug Binks' darl color scheme
		// https://gist.github.com/dougbinks/8089b4bbaccaaf6fa204236978d165a9
		ImGuiStyle& style = ImGui::GetStyle();
		if (_dark)
		{
			ImGui::StyleColorsDark(&style);
		}
		else
		{
			ImGui::StyleColorsLight(&style);
		}

		style.FrameRounding = 4.0f;
		style.WindowBorderSize = 0.0f;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void simgui_context::endFrame()
	{
		ImGui::Render();
		render(ImGui::GetDrawData());
	}

	static simgui_context S_CTX;

	//------------------------------------------------------------------------------------------------------------------------
	static void* memAlloc(size_t _size, void* /*_userData*/)
	{
		return bx::alloc(S_CTX.m_allocator, _size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	static void memFree(void* _ptr, void* /*_userData*/)
	{
		bx::free(S_CTX.m_allocator, _ptr);
	}

} //- unnamed

namespace ImGui
{
	void PushFont(Font::Enum _font)
	{
		PushFont(S_CTX.m_font[_font]);
	}

} //- ImGui

namespace imgui
{
	//------------------------------------------------------------------------------------------------------------------------
	void imguiCreate(float _fontSize /*= 18.0f*/, bx::AllocatorI* _allocator /*= nullptr*/)
	{
		S_CTX.create(_fontSize, _allocator);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void imguiDestroy()
	{
		S_CTX.destroy();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void imguiEndFrame()
	{
		S_CTX.endFrame();
	}

} //- imgui