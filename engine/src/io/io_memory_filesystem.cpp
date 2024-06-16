#include "io_memory_filesystem.hpp"

namespace io
{
	namespace
	{
		constexpr stringview_t C_BASE_PATH = "/";

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cmemory_filesystem::cmemory_filesystem() :
		m_ready(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmemory_filesystem::~cmemory_filesystem()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_filesystem::init()
	{
		m_ready = true;
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmemory_filesystem::shutdown()
	{
		m_file_list.clear();
		m_ready = false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_filesystem::ready() const
	{
		return m_ready;
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cmemory_filesystem::base_path() const
	{
		return C_BASE_PATH.data();
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::fs::filelist_t cmemory_filesystem::files() const
	{
		return m_file_list;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_filesystem::does_exist(const core::fs::cfileinfo& filepath) const
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	core::fs::file_ref_t cmemory_filesystem::open(const core::fs::cfileinfo& filepath, int file_mode)
	{
		using namespace core::fs;

		file_ref_t file = find_file(filepath);
		const auto exists = (file != nullptr);

		if (!exists)
		{
			file = std::move(std::make_shared<cmemory_file>(filepath));
		}

		file->open(file_mode);

		if (!exists && file->opened())
		{
			m_file_list.insert(file);
		}

		return file;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmemory_filesystem::close(core::fs::file_ref_t file)
	{
		if (file)
		{
			file->close();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_filesystem::create_file(const core::fs::cfileinfo& filepath)
	{
		using namespace core::fs;

		auto result = false;

		//- fails if file does not exist and we could not create one
		if (!does_exist(filepath))
		{
			if (file_ref_t file = open(filepath, core::file_mode_write | core::file_mode_truncate); file)
			{
				result = true;

				file->close();
			}
		}
		else
		{
			result = true;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_filesystem::remove_file(const core::fs::cfileinfo& filepath)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_filesystem::copy_file(const core::fs::cfileinfo& source, const core::fs::cfileinfo& dest)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_filesystem::rename_file(const core::fs::cfileinfo& source, const core::fs::cfileinfo& dest)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	core::fs::file_ref_t cmemory_filesystem::find_file(const core::fs::cfileinfo& fileinfo) const
	{

	}

} //- io