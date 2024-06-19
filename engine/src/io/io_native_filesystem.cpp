#include "io_native_filesystem.hpp"
#include <plugin_logging.h>

namespace io
{
	//------------------------------------------------------------------------------------------------------------------------
	cnative_filesystem::cnative_filesystem() :
		m_basepath({}), m_ready(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cnative_filesystem::~cnative_filesystem()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_filesystem::init(stringview_t basepath)
	{
		m_basepath.assign(basepath.data());

		if (!core::string_utils::ends_with(m_basepath, "/"))
		{
			m_basepath += "/";
		}

		build_file_list(m_basepath, m_file_list);

		m_ready = true;
		return m_ready;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cnative_filesystem::shutdown()
	{
		m_ready = false;
		m_basepath.clear();
		m_file_list.clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_filesystem::ready() const
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t cnative_filesystem::filesystem_name() const
	{
		constexpr stringview_t C_NAME = "cnative_filesystem";

		return C_NAME;
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cnative_filesystem::base_path() const
	{
		return m_basepath;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::fs::filelist_t cnative_filesystem::files() const
	{
		return m_file_list;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_filesystem::does_exist(const core::fs::cfileinfo& filepath) const
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::fs::file_ref_t cnative_filesystem::open(const core::fs::cfileinfo& filepath, int file_mode)
	{
		core::fs::cfileinfo info(base_path(), filepath.relative());

		auto file = find_file(info);
		const auto exists = (file != nullptr);

		if (!exists)
		{
			file = std::move(std::make_shared<cnative_file>(info));
		}

		file->open(file_mode);

		if (!exists && file->opened())
		{
			m_file_list.insert(file);
		}

		return file;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cnative_filesystem::close(core::fs::file_ref_t file)
	{
		if (file)
		{
			file->close();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_filesystem::create_file(const core::fs::cfileinfo& filepath)
	{
		auto result = false;

		if (!does_exist(filepath))
		{
			core::fs::cfileinfo info(base_path(), filepath.relative());

			if (const auto file = open(info, core::file_mode_write | core::file_mode_truncate); file)
			{
				result = true;

				file->close();
			}
		}
		else
		{
			//- file does exist already
			result = true;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_filesystem::remove_file(const core::fs::cfileinfo& filepath)
	{
		auto result = true;

		if (const auto file = find_file(filepath); file)
		{
			core::fs::cfileinfo info(base_path(), filepath.relative());

			if (std::filesystem::remove(info.path().data()))
			{
				m_file_list.erase(file);
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_filesystem::copy_file(const core::fs::cfileinfo& source, const core::fs::cfileinfo& dest)
	{
		auto result = false;

		auto source_file = find_file(source);
		auto dest_file = open(dest, core::file_mode_write);

		if (source_file && dest_file)
		{
			auto datasize = source_file->size();
			vector_t<byte_t> buffer(SCAST(uint64_t, datasize));

			const auto bytes_read = source_file->read(buffer.data(), datasize);
			const auto bytes_written = dest_file->write(buffer.data(), datasize);

			//- success only if we have read and copied complete file data
			result = (bytes_read == bytes_written && bytes_read == datasize);
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_filesystem::rename_file(const core::fs::cfileinfo& source, const core::fs::cfileinfo& dest)
	{
		auto result = false;

		auto source_file = find_file(source);
		auto dest_file = find_file(dest);

		if (source_file && dest_file)
		{
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::fs::file_ref_t cnative_filesystem::find_file(const core::fs::cfileinfo& fileinfo) const
	{
		const auto it = algorithm::find_if(m_file_list.begin(), m_file_list.end(), [&](const core::fs::file_ref_t& file)
			{
				return file->info() == fileinfo;
			});

		if (it != m_file_list.end())
		{
			return *it;
		}

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cnative_filesystem::build_file_list(string_t path, core::fs::filelist_t& out)
	{
		if (!core::string_utils::ends_with(path, "/"))
		{
			path += "/";
		}

		//- recursively iterate over base directory and gather all files contained within
		for (const auto& entry : std::filesystem::directory_iterator{ path })
		{
			if (entry.is_directory())
			{
				build_file_list(entry.path().generic_u8string(), out);
			}
			else if (entry.is_regular_file())
			{
				core::fs::cfileinfo info(entry.path().generic_u8string());

				out.insert(std::move(std::make_shared<cnative_file>(info)));
			}
		}
	}

} //- io