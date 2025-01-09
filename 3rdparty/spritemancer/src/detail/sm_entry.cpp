#include "sm_entry.hpp"

namespace sm
{
	namespace entry
	{
		static bx::DefaultAllocator S_ALLOCATOR_DEFAULT;
		static cfilereader S_FILEREADER;
		static cfilewriter S_FILEWRITER;
		static cstringwriter S_STRINGWRITER;
		static clogwriter S_LOGWRITER;

		//------------------------------------------------------------------------------------------------------------------------
		bool cfilereader::open(const bx::FilePath& path, bx::Error* error)
		{
			if (const auto result = bx::FileReader::open(path, error); !result)
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_warn,
						fmt::format("Failed to read from file '{}'", path.getCPtr()));
				}
				return false;
			}
			return true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cfilewriter::open(const bx::FilePath& path, bool append, bx::Error* error)
		{
			bx::Error err;

			if (const auto result = bx::FileWriter::open(path, append, &err); !result)
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_warn,
						fmt::format("Failed to write to file '{}'", path.getCPtr()));
				}
				return false;
			}
			return true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		int32_t cstringwriter::write(const void* data, int32_t size, bx::Error* error)
		{
			if (error->isOk())
			{
				const char* text = (const char*)data;
				string_t string(text, size);
				m_string += string;
				return size;
			}
			return -1;
		}

		//------------------------------------------------------------------------------------------------------------------------
		int32_t clogwriter::write(const void* data, int32_t size, bx::Error* error)
		{
			if (error->isOk() && serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_debug,
					(const char*)data);
				return size;
			}
			return -1;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cfilereader* filereader()
		{
			return &S_FILEREADER;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cfilewriter* filewriter()
		{
			return &S_FILEWRITER;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cstringwriter* stringwriter()
		{
			return &S_STRINGWRITER;
		}

		//------------------------------------------------------------------------------------------------------------------------
		clogwriter* logwriter()
		{
			return &S_LOGWRITER;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::AllocatorI* allocator()
		{
			return &S_ALLOCATOR_DEFAULT;
		}

	} //- entry

} //- sm