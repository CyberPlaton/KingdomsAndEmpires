#include "io_memory_filesystem.hpp"

namespace io
{
	//------------------------------------------------------------------------------------------------------------------------
	cmemory_filesystem::cmemory_filesystem() :
		m_basepath({}), m_ready(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmemory_filesystem::~cmemory_filesystem()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_filesystem::init(stringview_t basepath, stringview_t alias)
	{
		m_alias.assign(alias.data());
		m_basepath.assign(basepath.data());

		if (!string_utils::ends_with(m_basepath, "/"))
		{
			m_basepath += "/";
		}

		m_ready = true;
		return m_ready;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmemory_filesystem::shutdown()
	{
		m_ready = false;
		m_file_list.clear();
		m_basepath.clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_filesystem::ready() const
	{
		return m_ready;
	}

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t cmemory_filesystem::filesystem_name() const
	{
		constexpr stringview_t C_NAME = "cmemory_filesystem";

		return C_NAME;
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cmemory_filesystem::alias() const
	{
		return m_alias;
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cmemory_filesystem::base_path() const
	{
		return m_basepath;
	}

	//------------------------------------------------------------------------------------------------------------------------
	fs::filelist_t cmemory_filesystem::files() const
	{
		return m_file_list;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_filesystem::does_exist(const fs::cfileinfo& filepath) const
	{
		auto info = fs::ifilesystem::convert(this, filepath);

		if (info.is_file())
		{
			return find_file(info) != nullptr;
		}

		return info.exists();
	}

	//------------------------------------------------------------------------------------------------------------------------
	fs::file_ref_t cmemory_filesystem::open(const fs::cfileinfo& filepath, int file_mode)
	{
		auto info = fs::ifilesystem::convert(this, filepath);

		auto file = find_file(info);
		const auto exists = (file != nullptr);

		if (!exists)
		{
			file = std::move(std::make_shared<cmemory_file>(info));
		}

		file->open(file_mode);

		if (!exists && file->opened())
		{
			m_file_list.insert(file);
		}

		return file;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmemory_filesystem::close(fs::file_ref_t file)
	{
		if (file)
		{
			file->close();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_filesystem::create_file(const fs::cfileinfo& filepath)
	{
		auto result = false;

		//- fails if file does not exist and we could not create one
		if (!does_exist(filepath))
		{
			fs::cfileinfo info(base_path(), filepath.relative());

			if (fs::file_ref_t file = open(info, core::file_mode_read_write | core::file_mode_truncate); file)
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
	bool cmemory_filesystem::remove_file(const fs::cfileinfo& filepath)
	{
		auto result = true;

		fs::cfileinfo info(base_path(), filepath.relative());

		if (const auto file = find_file(info); file)
		{
			m_file_list.erase(file);
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_filesystem::copy_file(const fs::cfileinfo& source, const fs::cfileinfo& dest)
	{
		auto result = false;

		fs::cfileinfo source_info(base_path(), source.relative());

		auto source_file = std::static_pointer_cast<cmemory_file>(find_file(source_info));
		auto dest_file = std::static_pointer_cast<cmemory_file>(open(dest, core::file_mode_write));

		if (source_file && dest_file)
		{
			dest_file->m_memory.assign(source_file->m_memory.begin(), source_file->m_memory.end());
			dest_file->close();

			result = true;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_filesystem::rename_file(const fs::cfileinfo& source, const fs::cfileinfo& dest)
	{
		auto result = copy_file(source, dest);

		if (result)
		{
			result = remove_file(source);
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	vector_t<fs::cfileinfo> cmemory_filesystem::iterate(const fs::cfileinfo& path,
		core::filesystem_lookup_type type, bool recursive) const
	{
		CORE_ASSERT(false, "Invalid operation. Iterating a memory filesystem is undefined!");
		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	fs::file_ref_t cmemory_filesystem::find_file(const fs::cfileinfo& fileinfo) const
	{
		const auto it = algorithm::find_if(m_file_list.begin(), m_file_list.end(), [&](const fs::file_ref_t& file)
			{
				return file->info() == fileinfo;
			});

		if (it != m_file_list.end())
		{
			return *it;
		}

		return nullptr;
	}

} //- io