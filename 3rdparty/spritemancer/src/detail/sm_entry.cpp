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
		static bx::AllocatorI* S_ALLOCATOR = &S_ALLOCATOR_DEFAULT;

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
			if (const auto result = bx::FileWriter::open(path, append, error); !result)
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
			if (!error)
			{
				m_string.append((const char*)data, size);
				return -1;
			}
			return size;
		}

		//------------------------------------------------------------------------------------------------------------------------
		int32_t clogwriter::write(const void* data, int32_t size, bx::Error* error)
		{
			if (!error && serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_info,
					(const char*)data);
				return -1;
			}
			return size;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::FileReaderI* filereader()
		{
			return &S_FILEREADER;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::FileWriterI* filewriter()
		{
			return &S_FILEWRITER;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::WriterI* stringwriter()
		{
			return &S_STRINGWRITER;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::WriterI* logwriter()
		{
			return &S_LOGWRITER;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::AllocatorI* allocator()
		{
			return S_ALLOCATOR;
		}

	} //- entry

} //- sm