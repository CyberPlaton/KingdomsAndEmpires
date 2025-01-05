#pragma once
#include "platform.hpp"
#include <core.h>
#include <argparse.h>
#include <bgfx.h>
#include <bimg.h>
#include <bx.h>

#if CORE_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif CORE_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif CORE_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <glfw.h>

namespace sm
{
	//- Similar to the error reporter for core library. Set own callback to get reports from inside the library.
	//------------------------------------------------------------------------------------------------------------------------
	struct serror_reporter
	{
		STATIC_INSTANCE_EX(serror_reporter);

		core::error_report_function_t m_callback = nullptr;
	};

	namespace entry
	{
		//------------------------------------------------------------------------------------------------------------------------
		class cfilereader final : public bx::FileReader
		{
		public:
			bool open(const bx::FilePath& path, bx::Error* error) override final;
		};

		//- Note: can be use anywhere, where bx::WriterI* is required, i.e. when you want to compile shaders as binary files
		//------------------------------------------------------------------------------------------------------------------------
		class cfilewriter final : public bx::FileWriter
		{
		public:
			bool open(const bx::FilePath& path, bool append, bx::Error* error) override final;
		};

		//- Write incoming data to a string form
		//------------------------------------------------------------------------------------------------------------------------
		class cstringwriter final : public bx::WriterI
		{
		public:
			int32_t write(const void* data, int32_t size, bx::Error* error) override final;

			[[nodiscard]] inline string_t take() const { return std::move(m_string); }

		private:
			string_t m_string;
		};

		//- Write incoming data to log. Convenience class to use with thirdparty libraries,
		//- such as bx, bgfx etc
		//------------------------------------------------------------------------------------------------------------------------
		class clogwriter final : public bx::WriterI
		{
		public:
			int32_t write(const void* data, int32_t size, bx::Error* error) override final;
		};

		cfilereader*		filereader();
		cfilewriter*		filewriter();
		cstringwriter*		stringwriter();
		clogwriter*			logwriter();
		bx::AllocatorI*		allocator();

	} //- entry

} //- sm