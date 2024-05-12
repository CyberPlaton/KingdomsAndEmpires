#include "sm_shaderc.hpp"

#define MAX_TAGS 256
#include <fcpp/fpp.h>

namespace sm
{
	namespace shaderc
	{
		namespace
		{
			//------------------------------------------------------------------------------------------------------------------------
			struct ShadingLang
			{
				enum Enum
				{
					ESSL,
					GLSL,
					HLSL,
					Metal,
					PSSL,
					SpirV,

					Count
				};
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_shadingLangName[] =
			{
				"OpenGL ES Shading Language / WebGL (ESSL)",
				"OpenGL Shading Language (GLSL)",
				"High-Level Shading Language (HLSL)",
				"Metal Shading Language (MSL)",
				"PlayStation Shader Language (PSSL)",
				"Standard Portable Intermediate Representation - V (SPIR-V)",

				"Unknown?!"
			};
			BX_STATIC_ASSERT(BX_COUNTOF(s_shadingLangName) == ShadingLang::Count + 1, "ShadingLang::Enum and s_shadingLangName mismatch");

			//------------------------------------------------------------------------------------------------------------------------
			const char* getName(ShadingLang::Enum _lang)
			{
				return s_shadingLangName[_lang];
			}

			// c - compute
			// d - domain
			// f - fragment
			// g - geometry
			// h - hull
			// v - vertex
			//
			// OpenGL #version Features Direct3D Features Shader Model
			// 2.1    120      vf       9.0      vf       2.0
			// 3.0    130
			// 3.1    140
			// 3.2    150      vgf
			// 3.3    330               10.0     vgf      4.0
			// 4.0    400      vhdgf
			// 4.1    410
			// 4.2    420               11.0     vhdgf+c  5.0
			// 4.3    430      vhdgf+c
			// 4.4    440
			//
			// Metal Shading Language (MSL) profile naming convention:
			//  metal<MSL version>-<SPIR-V version>
			//
			// See section "Compiler Options Controlling the Language Version" from the
			// MSL spec for the correlation between MSL version and platform OS version:
			//	https://developer.apple.com/metal/Metal-Shading-Language-Specification.pdf
			//
			// MSL version | SPIR-V version | shaderc encoding
			//    1.0      |       1.0      |      1000         (deprecated)
			//    1.1      |       1.0      |      1110
			//    1.2      |       1.0      |      1210
			//    2.0      |       1.1      |      2011
			//    2.1      |       1.1      |      2111
			//    2.2      |       1.1      |      2211
			//    2.3      |       1.4      |      2314
			//    2.4      |       1.4      |      2414
			//    3.0      |       1.4      |      3014
			//    3.1      |       1.4      |      3114
			//
			// SPIR-V profile naming convention:
			//  spirv<SPIR-V version>-<Vulkan version>
			//
			// SPIR-V version | Vulkan version | shaderc encoding
			//       1.0      |       1.0      |      1010
			//       1.3      |       1.1      |      1311
			//       1.4      |       1.1      |      1411
			//       1.5      |       1.2      |      1512
			//       1.6      |       1.3      |      1613
			//------------------------------------------------------------------------------------------------------------------------
			struct Profile
			{
				ShadingLang::Enum lang;
				uint32_t id;
				const bx::StringLiteral name;
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const Profile s_profiles[] =
			{
				{  ShadingLang::ESSL,  100,    "100_es"     },
				{  ShadingLang::ESSL,  300,    "300_es"     },
				{  ShadingLang::ESSL,  310,    "310_es"     },
				{  ShadingLang::ESSL,  320,    "320_es"     },
				{  ShadingLang::HLSL,  400,    "s_4_0"      },
				{  ShadingLang::HLSL,  500,    "s_5_0"      },
				{  ShadingLang::Metal, 1210,   "metal"      },
				{  ShadingLang::Metal, 1000,   "metal10-10" },
				{  ShadingLang::Metal, 1110,   "metal11-10" },
				{  ShadingLang::Metal, 1210,   "metal12-10" },
				{  ShadingLang::Metal, 2011,   "metal20-11" },
				{  ShadingLang::Metal, 2111,   "metal21-11" },
				{  ShadingLang::Metal, 2211,   "metal22-11" },
				{  ShadingLang::Metal, 2314,   "metal23-14" },
				{  ShadingLang::Metal, 2414,   "metal24-14" },
				{  ShadingLang::Metal, 3014,   "metal30-14" },
				{  ShadingLang::Metal, 3114,   "metal31-14" },
				{  ShadingLang::PSSL,  1000,   "pssl"       },
				{  ShadingLang::SpirV, 1010,   "spirv"      },
				{  ShadingLang::SpirV, 1010,   "spirv10-10" },
				{  ShadingLang::SpirV, 1311,   "spirv13-11" },
				{  ShadingLang::SpirV, 1411,   "spirv14-11" },
				{  ShadingLang::SpirV, 1512,   "spirv15-12" },
				{  ShadingLang::SpirV, 1613,   "spirv16-13" },
				{  ShadingLang::GLSL,  120,    "120"        },
				{  ShadingLang::GLSL,  130,    "130"        },
				{  ShadingLang::GLSL,  140,    "140"        },
				{  ShadingLang::GLSL,  150,    "150"        },
				{  ShadingLang::GLSL,  330,    "330"        },
				{  ShadingLang::GLSL,  400,    "400"        },
				{  ShadingLang::GLSL,  410,    "410"        },
				{  ShadingLang::GLSL,  420,    "420"        },
				{  ShadingLang::GLSL,  430,    "430"        },
				{  ShadingLang::GLSL,  440,    "440"        },
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_ARB_shader_texture_lod[] =
			{
				"texture2DLod",
				"texture2DArrayLod", // BK - interacts with ARB_texture_array.
				"texture2DProjLod",
				"texture2DGrad",
				"texture2DProjGrad",
				"texture3DLod",
				"texture3DProjLod",
				"texture3DGrad",
				"texture3DProjGrad",
				"textureCubeLod",
				"textureCubeGrad",
				"shadow2DLod",
				"shadow2DProjLod",
				NULL
				// "texture1DLod",
				// "texture1DProjLod",
				// "shadow1DLod",
				// "shadow1DProjLod",
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_EXT_shader_texture_lod[] =
			{
				"texture2DLod",
				"texture2DProjLod",
				"textureCubeLod",
				"texture2DGrad",
				"texture2DProjGrad",
				"textureCubeGrad",
				NULL
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_EXT_shadow_samplers[] =
			{
				"shadow2D",
				"shadow2DProj",
				"sampler2DShadow",
				NULL
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_OES_standard_derivatives[] =
			{
				"dFdx",
				"dFdy",
				"fwidth",
				NULL
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_OES_texture_3D[] =
			{
				"texture3D",
				"texture3DProj",
				"texture3DLod",
				"texture3DProjLod",
				NULL
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_EXT_gpu_shader4[] =
			{
				"gl_VertexID",
				"gl_InstanceID",
				"texture2DLodOffset",
				NULL
			};

			// To be use from vertex program require:
			// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shader_viewport_layer_array.txt
			// DX11 11_1 feature level
			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_ARB_shader_viewport_layer_array[] =
			{
				"gl_ViewportIndex",
				"gl_Layer",
				NULL
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_ARB_gpu_shader5[] =
			{
				"bitfieldReverse",
				"floatBitsToInt",
				"floatBitsToUint",
				"intBitsToFloat",
				"uintBitsToFloat",
				NULL
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_ARB_shading_language_packing[] =
			{
				"packHalf2x16",
				"unpackHalf2x16",
				NULL
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_130[] =
			{
				"uint",
				"uint2",
				"uint3",
				"uint4",
				"isampler2D",
				"usampler2D",
				"isampler3D",
				"usampler3D",
				"isamplerCube",
				"usamplerCube",
				"textureSize",
				NULL
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_textureArray[] =
			{
				"sampler2DArray",
				"texture2DArray",
				"texture2DArrayLod",
				"shadow2DArray",
				NULL
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_ARB_texture_multisample[] =
			{
				"sampler2DMS",
				"isampler2DMS",
				"usampler2DMS",
				NULL
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_texelFetch[] =
			{
				"texelFetch",
				"texelFetchOffset",
				NULL
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_bitsToEncoders[] =
			{
				"floatBitsToUint",
				"floatBitsToInt",
				"intBitsToFloat",
				"uintBitsToFloat",
				NULL
			};

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_unsignedVecs[] =
			{
				"uvec2",
				"uvec3",
				"uvec4",
				NULL
			};

			//------------------------------------------------------------------------------------------------------------------------
			const char* s_uniformTypeName[] =
			{
				"int",  "int",
				NULL,   NULL,
				"vec4", "float4",
				"mat3", "float3x3",
				"mat4", "float4x4",
			};
			BX_STATIC_ASSERT(BX_COUNTOF(s_uniformTypeName) == bgfx::UniformType::Count * 2);

			//------------------------------------------------------------------------------------------------------------------------
			static const char* s_allowedVertexShaderInputs[] =
			{
				"a_position",
				"a_normal",
				"a_tangent",
				"a_bitangent",
				"a_color0",
				"a_color1",
				"a_color2",
				"a_color3",
				"a_indices",
				"a_weight",
				"a_texcoord0",
				"a_texcoord1",
				"a_texcoord2",
				"a_texcoord3",
				"a_texcoord4",
				"a_texcoord5",
				"a_texcoord6",
				"a_texcoord7",
				"i_data0",
				"i_data1",
				"i_data2",
				"i_data3",
				"i_data4",
				NULL
			};

		} //- unnamed

		//------------------------------------------------------------------------------------------------------------------------
		Uniform::Uniform()
			: type(bgfx::UniformType::Count)
			, num(0)
			, regIndex(0)
			, regCount(0)
			, texComponent(0)
			, texDimension(0)
			, texFormat(0)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		Options::Options()
			: shaderType(' ')
			, disasm(false)
			, raw(false)
			, preprocessOnly(false)
			, depends(false)
			, debugInformation(false)
			, avoidFlowControl(false)
			, noPreshader(false)
			, partialPrecision(false)
			, preferFlowControl(false)
			, backwardsCompatibility(false)
			, warningsAreErrors(false)
			, keepIntermediate(false)
			, optimize(false)
			, optimizationLevel(3)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::StringView nextWord(bx::StringView& _parse)
		{
			bx::StringView word = bx::strWord(bx::strLTrimSpace(_parse));
			_parse = bx::strLTrimSpace(bx::StringView(word.getTerm(), _parse.getTerm()));
			return word;
		}

		//------------------------------------------------------------------------------------------------------------------------
		const char* getUniformTypeName(bgfx::UniformType::Enum _enum)
		{
			uint32_t idx = _enum & ~(kUniformFragmentBit | kUniformSamplerBit);
			if (idx < bgfx::UniformType::Count)
			{
				return s_uniformTypeName[idx];
			}
			return "Unknown uniform type";
		}

		//------------------------------------------------------------------------------------------------------------------------
		bgfx::UniformType::Enum nameToUniformTypeEnum(const char* _name)
		{
			for (uint32_t ii = 0; ii < bgfx::UniformType::Count * 2; ++ii)
			{
				if (NULL != s_uniformTypeName[ii]
					&& 0 == bx::strCmp(_name, s_uniformTypeName[ii]))
				{
					return bgfx::UniformType::Enum(ii / 2);
				}
			}
			return bgfx::UniformType::Count;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void printCode(const char* _code, int32_t _line /*= 0*/, int32_t _start /*= 0*/, int32_t _end /*= INT32_MAX*/, int32_t _column /*= -1*/)
		{
			bx::printf("Code:\n---\n");

			bx::LineReader reader(_code);
			for (int32_t line = 1; !reader.isDone() && line < _end; ++line)
			{
				bx::StringView strLine = reader.next();

				if (line >= _start)
				{
					if (_line == line)
					{
						bx::printf("\n");
						bx::printf(">>> %3d: %.*s\n", line, strLine.getLength(), strLine.getPtr());
						if (-1 != _column)
						{
							bx::printf(">>> %3d: %*s\n", _column, _column, "^");
						}
						bx::printf("\n");
					}
					else
					{
						bx::printf("    %3d: %.*s\n", line, strLine.getLength(), strLine.getPtr());
					}
				}
			}

			bx::printf("---\n");
		}

		//------------------------------------------------------------------------------------------------------------------------
		void strReplace(char* _str, const char* _find, const char* _replace)
		{
			const int32_t len = bx::strLen(_find);

			char* replace = (char*)alloca(len + 1);
			bx::strCopy(replace, len + 1, _replace);
			for (int32_t ii = bx::strLen(replace); ii < len; ++ii)
			{
				replace[ii] = ' ';
			}
			replace[len] = '\0';

			BX_ASSERT(len >= bx::strLen(_replace), "");
			for (bx::StringView ptr = bx::strFind(_str, _find)
				; !ptr.isEmpty()
				; ptr = bx::strFind(ptr.getPtr() + len, _find)
				)
			{
				bx::memCopy(const_cast<char*>(ptr.getPtr()), replace, len);
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		int32_t writef(bx::WriterI* _writer, const char* _format, ...)
		{
			va_list argList;
			va_start(argList, _format);

			char temp[2048];

			char* out = temp;
			int32_t max = sizeof(temp);
			int32_t len = bx::vsnprintf(out, max, _format, argList);
			if (len > max)
			{
				out = (char*)alloca(len);
				len = bx::vsnprintf(out, len, _format, argList);
			}

			len = bx::write(_writer, out, len, bx::ErrorAssert{});

			va_end(argList);

			return len;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void writeFile(const char* _filePath, const void* _data, int32_t _size)
		{
			bx::FileWriter out;
			if (bx::open(&out, _filePath))
			{
				bx::write(&out, _data, _size, bx::ErrorAssert{});
				bx::close(&out);
			}
		}

	} //- shaderc

} //- sm