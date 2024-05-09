#include "sm_renderer_dx11.hpp"

#include <dwmapi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <atlbase.h>
#include <dxgi1_2.h>

#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		struct svertex
		{
			vec3_t m_position;
			vec2_t m_texture;
			ivec4_t m_tint;
		};

		//------------------------------------------------------------------------------------------------------------------------
		ID3D11InputLayout* dx_input_layout						= nullptr;
		ID3D11Device* dx_device									= nullptr;
		ID3D11DeviceContext* dx_device_context					= nullptr;
		IDXGISwapChain1* dx_swapchain							= nullptr;
		DXGI_SWAP_CHAIN_DESC1 dx_swapchain_desc;
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC dx_swapchain_desc_fullscreen;
		D3D11_VIEWPORT dx_viewport;
		IDXGIFactory2* dx_factory								= nullptr;
		IDXGIAdapter* dx_adapter								= nullptr;
		IDXGIDevice* dx_gi_device								= nullptr;
		ID3D11BlendState* dx_blend_state						= nullptr;
		ID3D11BlendState* dx_blend_state_default				= nullptr;
		ID3D11RenderTargetView* dx_rendertarget_view			= nullptr;
		ID3D11Texture2D* dx_depthstencil_buffer					= nullptr;
		ID3D11DepthStencilView* dx_depthstencil_view			= nullptr;
		ID3D11DepthStencilState* dx_depthstencil_state_default	= nullptr;
		ID3D11RasterizerState* dx_rasterizer_state_f			= nullptr;
		ID3D11RasterizerState* dx_rasterizer_state_w			= nullptr;
		ID3D11PixelShader* dx_pixel_shader						= nullptr;
		ID3D11VertexShader* dx_vertex_shader					= nullptr;
		ID3D11SamplerState* dx_sampler_state					= nullptr;
		ID3D11UnorderedAccessView* dx_uav						= nullptr;
		ID3D11UnorderedAccessView* dx_uav_layer					= nullptr;
		uint32_t dx_quad_shader									= 0;
		ID3D11Buffer* dx_vertex_buffer_quad						= nullptr;
		ID3D11Buffer* dx_vertex_buffer_layer					= nullptr;
		ID3D11Buffer* dx_vertex_indices_quad_layer				= nullptr;
		ID3D11Buffer* dx_vertex_indices_quad					= nullptr;
		constexpr auto C_VERTICES_COUNT_MAX						= 128;
		svertex vertices[C_VERTICES_COUNT_MAX];
		crenderable blank_quad;
		ivec4_t viewport										= { 0, 0, 0, 0 };
		vec4_t blend_state										= { 1.0f, 1.0f, 1.0f, 1.0f };
		blending_mode blend_mode								= blending_mode_normal;
		auto fullscreen											= false;
		auto enable_vsync										= true;
		auto width												= 720;
		auto height												= 648;

		vector_t<ID3D11ShaderResourceView*> dx_texture_srvs;
		vector_t<ID3D11UnorderedAccessView*> dx_texture_uavs;
		vector_t<ID3D11SamplerState*> dx_texture_samplers;

		//------------------------------------------------------------------------------------------------------------------------
		constexpr stringview_t C_SHADER_LATEST_PROFILE		= "latest";
		constexpr stringview_t C_SHADER_FEATURE_LEVEL_11_0	= "5_0";
		constexpr stringview_t C_SHADER_FEATURE_LEVEL_10_1	= "4_1";
		constexpr stringview_t C_SHADER_FEATURE_LEVEL_10_0	= "4_0";
		constexpr stringview_t C_SHADER_FEATURE_LEVEL_9_3	= "4_0_level_9_3";
		constexpr stringview_t C_SHADER_FEATURE_LEVEL_9_1	= "4_0_level_9_1";
		constexpr stringview_t C_VS_SHADER_EXT = "vs_";
		constexpr stringview_t C_PS_SHADER_EXT = "ps_";
		constexpr stringview_t C_CS_SHADER_EXT = "cs_";
		constexpr stringview_t C_GS_SHADER_EXT = "gs_";
		constexpr stringview_t C_HS_SHADER_EXT = "hs_";
		constexpr stringview_t C_DS_SHADER_EXT = "ds_";

		//------------------------------------------------------------------------------------------------------------------------
		constexpr stringview_t C_PS_DEFAULT_ENTRY_POINT = "SimplePS";
		constexpr stringview_t C_PS_DEFAULT =
			"Texture2D shaderTexture : register(t0);							\n"
			"SamplerState SampleType : register(s0);							\n"
			"struct PixelShaderInput											\n"
			"{																	\n"
			"float4 position : SV_POSITION;										\n"
			"float4 color: COLOR;												\n"
			"float2 tex : TEXCOORD0;											\n"
			"float4 PositionWS : TEXCOORD1;										\n"
			"};																	\n"
			"float4 SimplePS(PixelShaderInput IN) : SV_TARGET					\n"
			"{																	\n"
			"float4 textureColor = shaderTexture.Sample(SampleType, IN.tex);	\n"
			"textureColor.r *= IN.color.r/255;									\n"
			"textureColor.g *= IN.color.g/255;									\n"
			"textureColor.b *= IN.color.b/255;									\n"
			"textureColor.w *= IN.color.w/255;									\n"
			"return textureColor;												\n"
			"};																	\n"
			;

		//------------------------------------------------------------------------------------------------------------------------
		constexpr stringview_t C_VS_DEFAULT_ENTRY_POINT = "SimpleVS";
		constexpr stringview_t C_VS_DEFAULT =
			"cbuffer PerApplication : register(b0)							\n"
			"{																\n"
			"matrix projectionMatrix;										\n"
			"};																\n"
			"cbuffer PerFrame : register(b1)								\n"
			"{																\n"
			"matrix viewMatrix; matrix viewMatrixInv;						\n"
			"};																\n"
			"cbuffer PerObject : register(b2)								\n"
			"{																\n"
			"matrix worldMatrix;											\n"
			"};																\n"
			"struct AppData													\n"
			"{																\n"
			"float3 position : POSITION;									\n"
			"float4 color: COLOR;											\n"
			"float2 tex : TEXCOORD;											\n"
			"};																\n"
			"struct VertexShaderOutput										\n"
			"{																\n"
			"float4 position : SV_POSITION;									\n"
			"float4 color: COLOR;											\n"
			"float2 tex : TEXCOORD0;										\n"
			"float4 PositionWS : TEXCOORD1;									\n"
			"};																\n"
			"VertexShaderOutput SimpleVS(AppData IN)						\n"
			"{																\n"
			"VertexShaderOutput OUT;										\n"
			"OUT.position = float4(IN.position, 1);							\n"
			"OUT.PositionWS = mul(worldMatrix, float4(IN.position, 1.0f));	\n"
			"OUT.tex = IN.tex;												\n"
			"OUT.color = IN.color;											\n"
			"return OUT;													\n"
			"};																\n"
			;

		//------------------------------------------------------------------------------------------------------------------------
		enum shader_type : uint8_t
		{
			shader_type_none = 0,
			shader_type_vertex,
			shader_type_pixel,
			shader_type_compute,
			shader_type_geometry,
			shader_type_hull,
			shader_type_domain,
		};

		//------------------------------------------------------------------------------------------------------------------------
		constexpr D3D11_INPUT_ELEMENT_DESC vertex_desc[] =
		{
			{ "POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",		0,	DXGI_FORMAT_R32G32B32A32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		//------------------------------------------------------------------------------------------------------------------------
		constexpr D3D11_DEPTH_STENCIL_VIEW_DESC dx_depthstencil_desc { DXGI_FORMAT_D32_FLOAT, D3D11_DSV_DIMENSION_TEXTURE2D };

		//------------------------------------------------------------------------------------------------------------------------
		constexpr D3D_FEATURE_LEVEL feature_levels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		//------------------------------------------------------------------------------------------------------------------------
		template<typename T>
		inline static void safe_release(T& p)
		{
			if (p)
			{
				p->Release();
				p = nullptr;
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static bool is_windows_10()
		{
#if BX_PLATFORM_WINDOWS >= 0x0A00
			return true;
#else
			return false;
#endif
		}

		//------------------------------------------------------------------------------------------------------------------------
		string_t latest_profile(shader_type type)
		{
			stringview_t s;

			switch (type)
			{
			case shader_type_vertex:
			{
				s = C_VS_SHADER_EXT;
				break;
			}
			case shader_type_pixel:
			{
				s = C_PS_SHADER_EXT;
				break;
			}
			case shader_type_compute:
			{
				s = C_CS_SHADER_EXT;
				break;
			}
			case shader_type_geometry:
			{
				s = C_GS_SHADER_EXT;
				break;
			}
			case shader_type_hull:
			{
				s = C_HS_SHADER_EXT;
				break;
			}
			case shader_type_domain:
			{
				s = C_DS_SHADER_EXT;
				break;
			}
			default:
				break;
			}

			switch (dx_device->GetFeatureLevel())
			{
			case D3D_FEATURE_LEVEL_11_1:
			case D3D_FEATURE_LEVEL_11_0:
			{
				return fmt::format("{}{}", s, C_SHADER_FEATURE_LEVEL_11_0);
			}
			case D3D_FEATURE_LEVEL_10_1:
			{
				return fmt::format("{}{}", s, C_SHADER_FEATURE_LEVEL_10_1);
			}
			case D3D_FEATURE_LEVEL_10_0:
			{
				return fmt::format("{}{}", s, C_SHADER_FEATURE_LEVEL_10_0);
			}
			case D3D_FEATURE_LEVEL_9_3:
			{
				return fmt::format("{}{}", s, C_SHADER_FEATURE_LEVEL_9_3);
			}
			case D3D_FEATURE_LEVEL_9_2:
			case D3D_FEATURE_LEVEL_9_1:
			{
				return fmt::format("{}{}", s, C_SHADER_FEATURE_LEVEL_9_1);
			}
			default:
				break;
			}
			return {};
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TShader>
		TShader* create_shader(ID3DBlob* shader_blob, ID3D11ClassLinkage* class_linkage);

		//------------------------------------------------------------------------------------------------------------------------
		template<>
		ID3D11VertexShader* create_shader<ID3D11VertexShader>(ID3DBlob* shader_blob, ID3D11ClassLinkage* class_linkage)
		{
			ID3D11VertexShader* vs = nullptr;

			auto result = dx_device->CreateVertexShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(),
				class_linkage, &vs);

			if (FAILED(result))
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error,
						"Failed to create vertex shader");
				}
				return nullptr;
			}

			safe_release(dx_input_layout);

			result = dx_device->CreateInputLayout(vertex_desc, _countof(vertex_desc),
				shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), &dx_input_layout);

			if(FAILED(result))
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error,
						"Failed to set up vertex input layout");
				}
				return nullptr;
			}

			return vs;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<>
		ID3D11PixelShader* create_shader<ID3D11PixelShader>(ID3DBlob* shader_blob, ID3D11ClassLinkage* class_linkage)
		{
			ID3D11PixelShader* ps = nullptr;

			const auto result = dx_device->CreatePixelShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(),
				class_linkage, &ps);

			if (FAILED(result))
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error,
						"Failed to create pixel shader");
				}
				return nullptr;
			}

			return ps;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<>
		ID3D11ComputeShader* create_shader<ID3D11ComputeShader>(ID3DBlob* shader_blob, ID3D11ClassLinkage* class_linkage)
		{
			ID3D11ComputeShader* cs = nullptr;

			const auto result = dx_device->CreateComputeShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(),
				class_linkage, &cs);

			if (FAILED(result))
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error,
						"Failed to create compute shader");
				}
				return nullptr;
			}

			return cs;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<>
		ID3D11HullShader* create_shader<ID3D11HullShader>(ID3DBlob* shader_blob, ID3D11ClassLinkage* class_linkage)
		{
			ID3D11HullShader* hs = nullptr;

			const auto result = dx_device->CreateHullShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(),
				class_linkage, &hs);

			if (FAILED(result))
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error,
						"Failed to create hull shader");
				}
				return nullptr;
			}

			return hs;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<>
		ID3D11DomainShader* create_shader<ID3D11DomainShader>(ID3DBlob* shader_blob, ID3D11ClassLinkage* class_linkage)
		{
			ID3D11DomainShader* ds = nullptr;

			const auto result = dx_device->CreateDomainShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(),
				class_linkage, &ds);

			if (FAILED(result))
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error,
						"Failed to create domain shader");
				}
				return nullptr;
			}

			return ds;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TShader>
		TShader* compile_and_create_shader(shader_type type, stringview_t shader_source, stringview_t entry_point,
			stringview_t profile = C_SHADER_LATEST_PROFILE)
		{
			ID3DBlob* shader_blob = nullptr;
			ID3DBlob* error_blob = nullptr;
			TShader* shader = nullptr;

			string_t __profile = profile.data();
			if (core::string_utils::compare(__profile, C_SHADER_LATEST_PROFILE.data()))
			{
				__profile = latest_profile(type);
			}

#ifdef DEBUG
			const unsigned flags = D3DCOMPILE_OPTIMIZATION_LEVEL0 | D3DCOMPILE_DEBUG;
#else
			const unsigned flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
			const auto result = D3DCompile2(shader_source.data(), shader_source.length(), nullptr, nullptr, nullptr, entry_point.data(),
				__profile.data(), flags, 0, 0, 0, 0, &shader_blob, &error_blob);

			if (error_blob)
			{
				//- failed to compile shader
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error, (const char*)error_blob->GetBufferPointer());
				}

				return nullptr;
			}

			shader = create_shader<TShader>(shader_blob, nullptr);

			safe_release(shader_blob);
			safe_release(error_blob);

			return shader;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void draw_decal_vertex_layout_index_set_stage()
		{
			const unsigned vertexStride = sizeof(svertex);
			const unsigned offset = 0;

			dx_device_context->IASetVertexBuffers(0, 1, &dx_vertex_buffer_quad, &vertexStride, &offset);

			dx_device_context->IASetInputLayout(dx_input_layout);

			dx_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			dx_device_context->IASetIndexBuffer(dx_vertex_indices_quad, DXGI_FORMAT_R32_UINT, 0);

			dx_device_context->VSSetShader(dx_vertex_shader, nullptr, 0);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void draw_decal_pixel_stage(unsigned id)
		{
			dx_device_context->PSSetShader(dx_pixel_shader, nullptr, 0);

			dx_device_context->PSSetShaderResources(0, 1, &dx_texture_srvs[id]);

			dx_device_context->PSSetSamplers(0, 1, &dx_texture_samplers[id]);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void draw_decal_rast_stage()
		{
			dx_device_context->RSSetState(dx_rasterizer_state_w);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void draw_decal_output_merge_stage()
		{
			dx_device_context->OMSetBlendState(dx_blend_state, glm::value_ptr(blend_state), 0xffffffff);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void draw_decal_indexed_draw(const sdecal& decal)
		{
			dx_device_context->DrawIndexed(decal.m_position.size(), 0, 0);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void draw_layer_vertex_layout_index_set_stage()
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		void draw_layer_pixel_stage()
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		void draw_layer_rast_stage()
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		void draw_layer_output_merge_stage()
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		void draw_layer_indexed_draw()
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		void unset_shader(bool unbind_resource)
		{
			dx_device_context->VSSetShader(nullptr, nullptr, 0);
			dx_device_context->PSSetShader(nullptr, nullptr, 0);
			dx_device_context->OMSetBlendState(NULL, glm::value_ptr(blend_state), 0xffffffff);
			dx_device_context->PSSetSamplers(0, 1, &dx_sampler_state);

			if (unbind_resource)
			{
				ID3D11ShaderResourceView* unbind = nullptr;
				dx_device_context->PSSetShaderResources(0, 1, &unbind);
			}
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_dx::prepare_device()
	{
		//- none
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crenderer_dx::init_device(void* nwh, bool fullscreen, bool vsync)
	{
#ifdef DEBUG
		const unsigned flags = D3D11_CREATE_DEVICE_DEBUG;
#else
		const unsigned flags = 0;
#endif

		fullscreen = fullscreen;
		enable_vsync = vsync;

		DEVMODEA WinMonitorInfo;

		EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &WinMonitorInfo);

		DXGI_RATIONAL refreshRateStatic;
		refreshRateStatic.Numerator = WinMonitorInfo.dmDisplayFrequency;
		refreshRateStatic.Denominator = 1;

		if (enable_vsync == false)
		{
			refreshRateStatic.Numerator = 0;
		}

		ZeroMemory(&dx_swapchain_desc, sizeof(DXGI_SWAP_CHAIN_DESC1));
		ZeroMemory(&dx_swapchain_desc_fullscreen, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));

		if (is_windows_10())
		{
			dx_swapchain_desc.BufferCount = 2;
			dx_swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		}
		else
		{
			dx_swapchain_desc.BufferCount = 2;
			dx_swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		}

		dx_swapchain_desc.Width = width;
		dx_swapchain_desc.Height = height;
		dx_swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dx_swapchain_desc.SampleDesc.Count = 1;
		dx_swapchain_desc.SampleDesc.Quality = 0;
		dx_swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		dx_swapchain_desc.Scaling = DXGI_SCALING_NONE;

		dx_swapchain_desc_fullscreen.Scaling = DXGI_MODE_SCALING_STRETCHED;
		dx_swapchain_desc_fullscreen.RefreshRate = refreshRateStatic;
		dx_swapchain_desc_fullscreen.Windowed = !fullscreen;

		auto result = D3D11CreateDevice( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, feature_levels, _countof(feature_levels),
			D3D11_SDK_VERSION, &dx_device, nullptr, &dx_device_context);

		if (FAILED(result) && serror_reporter::instance().m_callback)
		{
			serror_reporter::instance().m_callback(core::logging_verbosity_critical,
				"Failed to create device");
			return opresult_fail;
		}

		dx_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dx_gi_device);

		dx_gi_device->GetAdapter(&dx_adapter);

		dx_adapter->GetParent(__uuidof(IDXGIFactory2), (void**)&dx_factory);

		dx_factory->CreateSwapChainForHwnd(dx_device, *(HWND*)nwh, &dx_swapchain_desc, &dx_swapchain_desc_fullscreen, NULL, &dx_swapchain);

		ID3D11Texture2D* backbuffer;
		dx_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);

		dx_device->CreateRenderTargetView(backbuffer, nullptr, &dx_rendertarget_view);

		safe_release(backbuffer);

		D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
		ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

		depthStencilBufferDesc.ArraySize = 1;
		depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthStencilBufferDesc.CPUAccessFlags = 0;
		depthStencilBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		depthStencilBufferDesc.Width = 640;
		depthStencilBufferDesc.Height = 640;
		depthStencilBufferDesc.MipLevels = 1;

		depthStencilBufferDesc.SampleDesc.Count = 1;
		depthStencilBufferDesc.SampleDesc.Quality = 0;

		depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		dx_device->CreateTexture2D(&depthStencilBufferDesc, nullptr, &dx_depthstencil_buffer);

		dx_device->CreateDepthStencilView(dx_depthstencil_buffer, &dx_depthstencil_desc, &dx_depthstencil_view);

		D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
		ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

		depthStencilStateDesc.DepthEnable = FALSE;
		depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilStateDesc.StencilEnable = FALSE;

		dx_device->CreateDepthStencilState(&depthStencilStateDesc, &dx_depthstencil_state_default);

		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

		rasterizerDesc.AntialiasedLineEnable = FALSE;
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = false;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.MultisampleEnable = FALSE;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;

		dx_device->CreateRasterizerState(&rasterizerDesc, &dx_rasterizer_state_f);

		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;

		dx_device->CreateRasterizerState(&rasterizerDesc, &dx_rasterizer_state_w);

		dx_viewport.Width = static_cast<float>(width);
		dx_viewport.Height = static_cast<float>(height);
		dx_viewport.TopLeftX = 0.0f;
		dx_viewport.TopLeftY = 0.0f;
		dx_viewport.MinDepth = 0.0f;
		dx_viewport.MaxDepth = 1.0f;

		dx_pixel_shader = compile_and_create_shader<ID3D11PixelShader>(shader_type_pixel, C_PS_DEFAULT, C_PS_DEFAULT_ENTRY_POINT);
		dx_vertex_shader = compile_and_create_shader<ID3D11VertexShader>(shader_type_vertex, C_VS_DEFAULT, C_VS_DEFAULT_ENTRY_POINT);

		D3D11_SAMPLER_DESC tmp_sampler_desc;

		tmp_sampler_desc.Filter = D3D11_FILTER{ D3D11_FILTER_ANISOTROPIC };
		tmp_sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmp_sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmp_sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmp_sampler_desc.MipLODBias = 0;
		tmp_sampler_desc.MaxAnisotropy = 8;
		tmp_sampler_desc.ComparisonFunc = D3D11_COMPARISON_FUNC{ D3D11_COMPARISON_LESS };
		tmp_sampler_desc.MinLOD = 1;
		tmp_sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

		dx_device->CreateSamplerState(&tmp_sampler_desc, &dx_sampler_state);

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

		vertexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_VERTEX_BUFFER;

		vertexBufferDesc.ByteWidth = sizeof(vertices);
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		resourceData.pSysMem = &vertices;

		dx_device->CreateBuffer(&vertexBufferDesc, &resourceData, &dx_vertex_buffer_quad);

		D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc;

		UAVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		UAVdesc.Buffer.FirstElement = 0;
		UAVdesc.Buffer.NumElements = sizeof(vertices) / sizeof(svertex);
		UAVdesc.Buffer.Flags = 0;

		svertex verts[4] = {
		{ {-1.0f, -1.0f, 0.0f}, { 0.0f, 1.0f}, {0,0,0,0}},
		{ {+1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {0,0,0,0}},
		{ {-1.0f, +1.0f, 0.0f}, {0.0f, 0.0f}, {0,0,0,0}},
		{ {+1.0f, +1.0f, 0.0f}, {1.0f, 0.0f}, {0,0,0,0}},
		};

		ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

		vertexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_VERTEX_BUFFER;

		vertexBufferDesc.ByteWidth = sizeof(svertex) * 4;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

		ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		resourceData.pSysMem = &verts;

		dx_device->CreateBuffer(&vertexBufferDesc, &resourceData, &dx_vertex_buffer_layer);

		UAVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		UAVdesc.Buffer.FirstElement = 0;
		UAVdesc.Buffer.NumElements = sizeof(dx_vertex_buffer_layer) / sizeof(svertex);
		UAVdesc.Buffer.Flags = 0;

		UINT dxIndiceArr[C_VERTICES_COUNT_MAX];
		for (int i = 0; i < C_VERTICES_COUNT_MAX; i += 4)
		{
			if (i < 4)
			{
				dxIndiceArr[i] = i;
				dxIndiceArr[i + 1] = i + 1;
				dxIndiceArr[i + 2] = i + 3;
				dxIndiceArr[i + 3] = i + 2;
			}
		}

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

		indexBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.ByteWidth = sizeof(UINT) * C_VERTICES_COUNT_MAX;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		indexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

		resourceData.pSysMem = &dxIndiceArr;

		dx_device->CreateBuffer(&indexBufferDesc, &resourceData, &dx_vertex_buffer_quad);

		ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

		indexBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.ByteWidth = sizeof(UINT) * 5;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		indexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

		UINT dxIndiceArrLayer[5] = { 0,1,2,3,0 };

		resourceData.pSysMem = &dxIndiceArrLayer;

		dx_device->CreateBuffer(&indexBufferDesc, &resourceData, &dx_vertex_indices_quad_layer);

		D3D11_BLEND_DESC blendVal;
		blendVal.AlphaToCoverageEnable = false;
		blendVal.IndependentBlendEnable = false;
		blendVal.RenderTarget[0].BlendEnable = true;
		blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		dx_device->CreateBlendState(&blendVal, &dx_blend_state_default);
		dx_device->CreateBlendState(&blendVal, &dx_blend_state);

		//- create a blank quad
		blank_quad.create(1, 1);

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crenderer_dx::shutdown_device()
	{
		for (auto i = 0u; i < dx_texture_srvs.size(); ++i)
		{
			safe_release(dx_texture_srvs[i]);
			safe_release(dx_texture_uavs[i]);
			safe_release(dx_texture_samplers[i]);
		}

		safe_release(dx_vertex_indices_quad);
		safe_release(dx_vertex_indices_quad_layer);
		safe_release(dx_vertex_buffer_layer);
		safe_release(dx_vertex_buffer_quad);
		safe_release(dx_uav_layer);
		safe_release(dx_uav);
		safe_release(dx_sampler_state);
		safe_release(dx_vertex_shader);
		safe_release(dx_pixel_shader);
		safe_release(dx_rasterizer_state_w);
		safe_release(dx_rasterizer_state_f);
		safe_release(dx_depthstencil_state_default);
		safe_release(dx_depthstencil_view);
		safe_release(dx_depthstencil_buffer);
		safe_release(dx_rendertarget_view);
		safe_release(dx_blend_state_default);
		safe_release(dx_blend_state);
		safe_release(dx_gi_device);
		safe_release(dx_adapter);
		safe_release(dx_factory);
		safe_release(dx_swapchain);
		safe_release(dx_device_context);
		safe_release(dx_device);
		safe_release(dx_input_layout);

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_dx::prepare_frame()
	{
		set_blending_mode(blending_mode_normal);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_dx::display_frame()
	{
		dx_swapchain->Present(enable_vsync? 1 : 0, 0);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_dx::set_blending_mode(blending_mode mode)
	{
		if (blend_mode != mode && mode != blending_mode_none)
		{
			D3D11_BLEND_DESC blendVal;

			switch (mode)
			{
			default:
			case blending_mode_normal:
			{
				blendVal.AlphaToCoverageEnable = false;
				blendVal.IndependentBlendEnable = false;
				blendVal.RenderTarget[0].BlendEnable = true;
				blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				break;
			}
			case blending_mode_additive:
			{
				blendVal.AlphaToCoverageEnable = false;
				blendVal.IndependentBlendEnable = false;
				blendVal.RenderTarget[0].BlendEnable = true;
				blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
				blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
				blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				break;
			}
			case blending_mode_multiply:
			{
				blendVal.AlphaToCoverageEnable = true;
				blendVal.IndependentBlendEnable = false;
				blendVal.RenderTarget[0].BlendEnable = true;
				blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
				blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
				blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				break;
			}
			case blending_mode_stencil:
			{
				blendVal.AlphaToCoverageEnable = false;
				blendVal.IndependentBlendEnable = false;
				blendVal.RenderTarget[0].BlendEnable = true;
				blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
				blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_ALPHA;
				blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				break;
			}
			case blending_mode_illuminate:
			{
				blendVal.AlphaToCoverageEnable = false;
				blendVal.IndependentBlendEnable = false;
				blendVal.RenderTarget[0].BlendEnable = true;
				blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_SRC_ALPHA;
				blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_ALPHA;
				blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				break;
			}
			}

			safe_release(dx_blend_state);
			blend_mode = mode;
			dx_device->CreateBlendState(&blendVal, &dx_blend_state);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned crenderer_dx::create_texture(unsigned w, unsigned h, bool filtering, bool clamping)
	{
		uint32_t id = dx_texture_srvs.size();

		ID3D11ShaderResourceView* trash_memV = nullptr;
		ID3D11Resource* trash_memT = nullptr;
		ID3D11UnorderedAccessView* trash_memU = nullptr;

		ID3D11Texture2D* gpuTex = nullptr;
		ID3D11Texture2D* gpuTexS = nullptr;

		D3D11_TEXTURE2D_DESC gpuTexDesc;
		ZeroMemory(&gpuTexDesc, sizeof(gpuTexDesc));
		gpuTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpuTexDesc.Width = w;
		gpuTexDesc.Height = h;
		gpuTexDesc.MipLevels = 1;
		gpuTexDesc.ArraySize = 1;
		gpuTexDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |
			D3D11_BIND_SHADER_RESOURCE;
		gpuTexDesc.SampleDesc.Count = 1;
		gpuTexDesc.SampleDesc.Quality = 0;
		gpuTexDesc.MiscFlags = 0;
		gpuTexDesc.CPUAccessFlags = 0;
		gpuTexDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_TEXTURE2D_DESC gpuTexDescS;
		ZeroMemory(&gpuTexDescS, sizeof(gpuTexDescS));
		gpuTexDescS.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpuTexDescS.Width = w;
		gpuTexDescS.Height = h;
		gpuTexDescS.MipLevels = 1;
		gpuTexDescS.ArraySize = 1;
		gpuTexDescS.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		gpuTexDescS.SampleDesc.Count = 1;
		gpuTexDescS.SampleDesc.Quality = 0;
		gpuTexDescS.MiscFlags = 0;
		gpuTexDescS.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		gpuTexDescS.Usage = D3D11_USAGE_DYNAMIC;


		D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc;
		UAVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		UAVdesc.Buffer.FirstElement = 0;
		UAVdesc.Buffer.NumElements = 1;
		UAVdesc.Texture2D.MipSlice = 0;

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVdesc;
		SRVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVdesc.Buffer.FirstElement = 0;
		SRVdesc.Buffer.NumElements = 1;
		SRVdesc.Texture2D.MostDetailedMip = 0;
		SRVdesc.Texture2D.MipLevels = 1;

		ID3D11SamplerState* trash_memSamp;

		D3D11_SAMPLER_DESC tmpSampleDesc;

		tmpSampleDesc.Filter = D3D11_FILTER{ D3D11_FILTER_ANISOTROPIC };
		tmpSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmpSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmpSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmpSampleDesc.MipLODBias = 0;
		tmpSampleDesc.MaxAnisotropy = 8;
		tmpSampleDesc.ComparisonFunc = D3D11_COMPARISON_FUNC{ D3D11_COMPARISON_LESS };
		tmpSampleDesc.MinLOD = 1;
		tmpSampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

		if (filtering)
		{
			tmpSampleDesc.Filter = D3D11_FILTER{ D3D11_FILTER_MIN_MAG_MIP_LINEAR };
		}
		else
		{
			tmpSampleDesc.Filter = D3D11_FILTER{ D3D11_FILTER_MIN_MAG_MIP_POINT };
		}

		if (clamping)
		{
			tmpSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_CLAMP };
			tmpSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_CLAMP };
			tmpSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_CLAMP };
		}
		else
		{
			tmpSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_MIRROR };
			tmpSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_MIRROR };
			tmpSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_MIRROR };
		}

		dx_device->CreateTexture2D(&gpuTexDesc, NULL, &gpuTex);
		dx_device->CreateTexture2D(&gpuTexDescS, NULL, &gpuTexS);

		dx_device->CreateShaderResourceView(gpuTexS, &SRVdesc, &trash_memV);

		dx_device->CreateUnorderedAccessView(gpuTex, &UAVdesc, &trash_memU);

		dx_device->CreateSamplerState(&tmpSampleDesc, &trash_memSamp);

		dx_texture_uavs.push_back(trash_memU);
		dx_texture_srvs.push_back(trash_memV);
		dx_texture_samplers.push_back(trash_memSamp);

		return id;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned crenderer_dx::destroy_texture()
	{
		//- TODO: implement destroying from GPU and decide what the return value is for
		return MAX(unsigned);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_dx::draw_layer_quad(unsigned x, unsigned y, unsigned w, unsigned h, core::scolor tint)
	{
		const svertex verts[4] = {
				{ {-1.0f, -1.0f, 0.0f},	{0.0f * w + x, 1.0f * h + y},	{ tint.r(),tint.g(),tint.b(),tint.a() }},
				{ {1.0f, -1.0f, 0.0f},	{1.0f * w + x, 1.0f * h + y},	{ tint.r(),tint.g(),tint.b(),tint.a() }},
				{ {-1.0f, 1.0f, 0.0f},	{0.0f * w + x, 0.0f * h + y},	{ tint.r(),tint.g(),tint.b(),tint.a() }},
				{ {1.0f, 1.0f, 0.0f},	{1.0f * w + x, 0.0f * h + y},	{ tint.r(),tint.g(),tint.b(),tint.a() }},
		};

		D3D11_MAPPED_SUBRESOURCE resource;

		dx_device_context->Map(dx_vertex_buffer_layer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, &verts, sizeof(svertex) * 5);
		dx_device_context->Unmap(dx_vertex_buffer_layer, 0);

		draw_layer_vertex_layout_index_set_stage();
		draw_layer_pixel_stage();
		draw_layer_rast_stage();
		draw_layer_output_merge_stage();
		draw_layer_indexed_draw();
		unset_shader(true);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_dx::draw_decal(const sdecal& decal)
	{
		set_blending_mode(decal.m_blending);

		unsigned id = decal.m_texture ? decal.m_texture->id() : blank_quad.texture().id();

		for (auto i = 0u; i < decal.m_points; ++i)
		{
			vertices[i] = {
				{ decal.m_position[i].x, decal.m_position[i].y, 0.0f },
				{ decal.m_uv[i].x, decal.m_uv[i].y },
				{ decal.m_tint[i].r, decal.m_tint[i].g, decal.m_tint[i].b, decal.m_tint[i].a }
			};
		}

		D3D11_MAPPED_SUBRESOURCE resource;

		//- copy vertices to buffer
		dx_device_context->Map(dx_vertex_buffer_quad, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, vertices, sizeof(vertices));
		dx_device_context->Unmap(dx_vertex_buffer_quad, 0);

		//- proceed setting up stages and submit decal vertices
		draw_decal_vertex_layout_index_set_stage();
		draw_decal_pixel_stage(id);
		draw_decal_rast_stage();
		draw_decal_output_merge_stage();
		draw_decal_indexed_draw(decal);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_dx::bind_texture(unsigned id)
	{
		//- empty
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_dx::update_texture(unsigned id, unsigned w, unsigned h, void* data)
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		ID3D11Resource* texture_resource;

		dx_texture_uavs[id]->GetResource(&texture_resource);

		dx_device_context->Map(texture_resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		BYTE* mappedData = reinterpret_cast<BYTE*>(resource.pData);
		BYTE* buffer = reinterpret_cast<BYTE*>(data);

		for (auto i = 0u; i < h; ++i)
		{
			memcpy(mappedData, buffer, w * sizeof(unsigned));
			mappedData += resource.RowPitch;

			buffer += w * sizeof(unsigned);
		}

		dx_device_context->Unmap(texture_resource, 0);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_dx::update_image(unsigned id, unsigned w, unsigned h, void* data)
	{
		//- TODO
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_dx::clear(core::scolor tint, bool depth)
	{
		dx_device_context->ClearRenderTargetView(dx_rendertarget_view, glm::value_ptr(tint.normalize()));

		if (depth)
		{
			dx_device_context->ClearDepthStencilView(dx_depthstencil_view, D3D11_CLEAR_DEPTH, 1, 0);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_dx::update_viewport(unsigned x, unsigned y, unsigned w, unsigned h)
	{
		//- initial creation
		if (viewport.x == 0 || viewport.y == 0 || viewport.z == 0 || viewport.w == 0)
		{
			viewport.x = x;
			viewport.y = y;
			viewport.z = w;
			viewport.w = h;

			ID3D11RenderTargetView* tmpRendTarV = nullptr;

			dx_device_context->OMSetRenderTargets(1, &tmpRendTarV, nullptr);

			safe_release(dx_depthstencil_buffer);
			dx_rendertarget_view->Release();
			dx_depthstencil_view->Release();
			dx_device_context->Flush();

			dx_swapchain->ResizeBuffers(0, viewport.z, viewport.w, dx_swapchain_desc.Format, dx_swapchain_desc.Flags);

			ID3D11Texture2D* backBuffer;

			dx_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

			dx_device->CreateRenderTargetView(backBuffer, nullptr, &dx_rendertarget_view);

			safe_release(backBuffer);

			dx_device_context->OMSetRenderTargets(1, &dx_rendertarget_view, NULL);


			D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
			ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

			depthStencilBufferDesc.ArraySize = 1;
			depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			depthStencilBufferDesc.CPUAccessFlags = 0;
			depthStencilBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			depthStencilBufferDesc.Width = viewport.z;
			depthStencilBufferDesc.Height = viewport.w;
			depthStencilBufferDesc.MipLevels = 1;

			depthStencilBufferDesc.SampleDesc.Count = 1;
			depthStencilBufferDesc.SampleDesc.Quality = 0;

			depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

			dx_device->CreateTexture2D(&depthStencilBufferDesc, nullptr, &dx_depthstencil_buffer);

			dx_device->CreateDepthStencilView(dx_depthstencil_buffer, &dx_depthstencil_desc, &dx_depthstencil_view);

			dx_device_context->RSSetViewports(1, &dx_viewport);
		}

		//- change of dimension
		if (viewport.x != x || viewport.y != y || viewport.z != w || viewport.w != h)
		{
			viewport.x = x;
			viewport.y = y;
			viewport.z = w;
			viewport.w = h;

			ID3D11RenderTargetView* tmpRendTarV = nullptr;

			dx_device_context->OMSetRenderTargets(1, &tmpRendTarV, nullptr);

			safe_release(dx_depthstencil_buffer);
			dx_rendertarget_view->Release();
			dx_depthstencil_view->Release();
			dx_device_context->Flush();

			dx_swapchain->ResizeBuffers(0, viewport.x + viewport.z, viewport.y + viewport.w, dx_swapchain_desc.Format, dx_swapchain_desc.Flags);

			ID3D11Texture2D* backBuffer;

			dx_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

			dx_device->CreateRenderTargetView(backBuffer, nullptr, &dx_rendertarget_view);

			safe_release(backBuffer);

			dx_device_context->OMSetRenderTargets(1, &dx_rendertarget_view, NULL);

			D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
			ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

			depthStencilBufferDesc.ArraySize = 1;
			depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			depthStencilBufferDesc.CPUAccessFlags = 0;
			depthStencilBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			depthStencilBufferDesc.Width = viewport.x + viewport.z;
			depthStencilBufferDesc.Height = viewport.y + viewport.w;
			depthStencilBufferDesc.MipLevels = 1;

			depthStencilBufferDesc.SampleDesc.Count = 1;
			depthStencilBufferDesc.SampleDesc.Quality = 0;

			depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

			dx_device->CreateTexture2D(&depthStencilBufferDesc, nullptr, &dx_depthstencil_buffer);

			dx_device->CreateDepthStencilView(dx_depthstencil_buffer, &dx_depthstencil_desc, &dx_depthstencil_view);

			dx_device_context->RSSetViewports(1, &dx_viewport);
		}
	}

} //- sm