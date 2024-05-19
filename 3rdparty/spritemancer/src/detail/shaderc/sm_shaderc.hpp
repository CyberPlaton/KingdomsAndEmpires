#pragma once
#include "../sm_config.hpp"
#include <src/vertexlayout.h>

//- Available on DirectX platforms
//------------------------------------------------------------------------------------------------------------------------
#ifndef SHADERC_CONFIG_HLSL
	#if CORE_PLATFORM_WINDOWS
		#define SHADERC_CONFIG_HLSL 1
	#else
		#define SHADERC_CONFIG_HLSL 0
	#endif
#endif

namespace sm
{
	namespace shaderc
	{

		//- shaderc is a bgfx tool https://bkaradzic.github.io/bgfx/tools.html#shader-compiler-shaderc
		//------------------------------------------------------------------------------------------------------------------------
		constexpr static uint8_t kUniformFragmentBit	= 0x10;
		constexpr static uint8_t kUniformSamplerBit		= 0x20;
		constexpr static uint8_t kUniformReadOnlyBit	= 0x40;
		constexpr static uint8_t kUniformCompareBit		= 0x80;
		constexpr static uint8_t kUniformMask			= 0
			| kUniformFragmentBit | kUniformSamplerBit
			| kUniformReadOnlyBit | kUniformCompareBit;

		//------------------------------------------------------------------------------------------------------------------------
		struct Uniform
		{
			Uniform();

			std::string name;
			bgfx::UniformType::Enum type;
			uint8_t num;
			uint16_t regIndex;
			uint16_t regCount;
			uint8_t texComponent;
			uint8_t texDimension;
			uint16_t texFormat;
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct Options
		{
			Options();

			char shaderType;
			std::string platform;
			std::string profile;

			std::string	inputFilePath;
			std::string	outputFilePath;

			std::vector<std::string> includeDirs;
			std::vector<std::string> defines;
			std::vector<std::string> dependencies;

			bool disasm;
			bool raw;
			bool preprocessOnly;
			bool depends;

			bool debugInformation;

			bool avoidFlowControl;
			bool noPreshader;
			bool partialPrecision;
			bool preferFlowControl;
			bool backwardsCompatibility;
			bool warningsAreErrors;
			bool keepIntermediate;

			bool optimize;
			uint32_t optimizationLevel;
		};

		typedef std::vector<Uniform> UniformArray;

		bx::StringView nextWord(bx::StringView& _parse);
		const char* getUniformTypeName(bgfx::UniformType::Enum _enum);
		bgfx::UniformType::Enum nameToUniformTypeEnum(const char* _name);
		void printCode(const char* _code, int32_t _line = 0, int32_t _start = 0, int32_t _end = INT32_MAX, int32_t _column = -1);
		void strReplace(char* _str, const char* _find, const char* _replace);
		int32_t writef(bx::WriterI* _writer, const char* _format, ...);
		void writeFile(const char* _filePath, const void* _data, int32_t _size);

		bool compileGLSLShader(const Options& _options, uint32_t _version, const std::string& _code, bx::WriterI* _writer, bx::WriterI* _messages);
		bool compileHLSLShader(const Options& _options, uint32_t _version, const std::string& _code, bx::WriterI* _writer, bx::WriterI* _messages);
		bool compileMetalShader(const Options& _options, uint32_t _version, const std::string& _code, bx::WriterI* _writer, bx::WriterI* _messages);
		bool compilePSSLShader(const Options& _options, uint32_t _version, const std::string& _code, bx::WriterI* _writer, bx::WriterI* _messages);
		bool compileSPIRVShader(const Options& _options, uint32_t _version, const std::string& _code, bx::WriterI* _writer, bx::WriterI* _messages);
		bool compileShader(const char* _varying, const char* _comment, char* _shader, uint32_t _shaderLen, const Options& _options,
			bx::WriterI* _shaderWriter, bx::WriterI* _messageWriter);

		const char* getPsslPreamble();

	} //- shaderc

} //- sm