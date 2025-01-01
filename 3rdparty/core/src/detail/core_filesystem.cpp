#include "core_filesystem.hpp"
#include "core_stringutils.hpp"
#include "core_algorithm.hpp"
#include "core_async.hpp"
#include "core_memory.hpp"

namespace core
{
	namespace
	{
		constexpr stringview_t C_EMPTY_STRING = "";

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void cpath::update_strings()
	{
		m_string_path = m_path.generic_u8string().c_str();
		m_string_ext = m_path.extension().generic_u8string().c_str();
		m_string_stem = m_path.stem().generic_u8string().c_str();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath::cpath(const char* path) :
		m_path(path)
	{
		m_dir = std::filesystem::directory_entry(m_path);

		update_strings();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath::cpath(const std::filesystem::path& path) :
		m_path(path)
	{
		m_dir = std::filesystem::directory_entry(m_path);

		update_strings();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath::cpath(const cpath& path) :
		m_path(path.m_path)
	{
		m_dir = std::filesystem::directory_entry(m_path);

		update_strings();
	}

	//------------------------------------------------------------------------------------------------------------------------
	std::filesystem::path cpath::path() const
	{
		return m_path;
	}

	//------------------------------------------------------------------------------------------------------------------------
	std::filesystem::directory_entry cpath::dir() const
	{
		return m_dir;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::exists() const
	{
		return m_dir.exists();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::has_extension() const
	{
		return m_path.has_extension();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::has_parent() const
	{
		return m_path.has_parent_path();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::is_dir() const
	{
		return m_dir.is_directory();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::is_file() const
	{
		return m_dir.is_regular_file();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath cpath::parent() const
	{
		return { m_path.parent_path() };
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath& cpath::append(stringview_t path)
	{
		m_path = std::filesystem::path(fmt::format("{}{}", m_path.generic_u8string(), path).c_str());
		m_dir = std::filesystem::directory_entry(m_path);

		update_strings();

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::operator==(const std::filesystem::path& path)
	{
		return m_path == path;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::operator==(stringview_t path)
	{
		return view() == path;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath& cpath::operator/=(stringview_t path)
	{
		m_path /= path;
		m_dir = std::filesystem::directory_entry(m_path);

		update_strings();

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath& cpath::operator=(const cpath& path)
	{
		m_path = path.m_path;
		m_dir = path.m_dir;

		update_strings();

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cfilesystem::cfilesystem(cpath path) :
		m_current(std::move(path))
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cfilesystem::cfilesystem(const char* path) :
		m_current(path)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cpath cfilesystem::construct(const char* path, const char* addition)
	{
		return cpath(path).append(addition);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath cfilesystem::join(const char* path, const char* addition)
	{
		return cpath(path) /= addition;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cpath cfilesystem::cwd()
	{
		try
		{
			return std::filesystem::current_path().c_str();
		}
		catch (...)
		{
		}
		return cpath(C_EMPTY_STRING);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_dir(const char* path)
	{
		return std::filesystem::create_directory(path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_dir_in(const char* path, const char* name)
	{
		return create_dir(cpath(path).append(name).view());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_dir_recursive(const char* path)
	{
		return std::filesystem::create_directories(path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_file(const char* path)
	{
		std::ofstream out(path);

		std::filesystem::permissions(path,
			std::filesystem::perms::owner_all | std::filesystem::perms::group_all,
			std::filesystem::perm_options::add);

		out.close();

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_file_in(const char* path, const char* stem, const char* ext)
	{
		return create_file(fmt::format("{}/{}.{}", path, stem, ext).data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::remove(const char* path, const char* name)
	{
		cpath p(path);
		p.append(name);

		return remove(p.view());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::remove(const char* path)
	{
		//- filesystem throws on errors
		try
		{
			std::error_code err;

			return std::filesystem::remove_all(path, err) >= 1;
		}
		catch (const std::exception& /*e*/)
		{
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_file(const char* path, const char* name)
	{
		return find_at(path, name, filesystem_lookup_type_file);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_file_by_stem(const char* path, const char* name)
	{
		cpath p(path);
		if (p.exists())
		{
			for (const auto& entry : std::filesystem::directory_iterator{ p.path() })
			{
				if (entry.is_regular_file() && entry.path().stem().string().compare(name) == 0)
				{
					return true;
				}
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_file_by_extension(const char* path, const char* name)
	{
		cpath p(path);
		if (p.exists())
		{
			for (const auto& entry : std::filesystem::directory_iterator{ p.path() })
			{
				if (entry.is_regular_file() &&
					entry.path().has_extension() &&
					entry.path().extension().string().compare(name) == 0)
				{
					return true;
				}
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_dir(const char* path, const char* name)
	{
		return find_at(path, name, filesystem_lookup_type_directory);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_at(const char* path, const char* name, filesystem_lookup_type type)
	{
		cpath p(path);
		if (p.exists())
		{
			cpath check(p); check /= name;

			for (const auto& entry : std::filesystem::directory_iterator{ p.path() })
			{
				switch (type)
				{
				default:
				case filesystem_lookup_type_any:
				{
					if (check == entry.path())
					{
						return true;
					}
					break;
				}
				case filesystem_lookup_type_directory:
				{
					if (entry.is_directory() && check == entry.path())
					{
						return true;
					}
					break;
				}
				case filesystem_lookup_type_file:
				{
					if (entry.is_regular_file() && check == entry.path())
					{
						return true;
					}
					break;
				}
				}
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cpath cfilesystem::construct_relative_to_cwd(const char* path)
	{
		auto absolute = std::filesystem::absolute(cfilesystem::cwd().path());

		return std::filesystem::relative(path, absolute);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::is_contained(const cpath& contained, const cpath& container)
	{
		return container.path() / std::filesystem::relative(contained.path()) == contained.path();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::forwards(const char* path, bool forced /*= false*/)
	{
		//- create copy
		cpath copy(m_current);

		copy /= path;

		if (copy.exists())
		{
			m_current = std::move(copy);
			return true;
		}
		else if (forced)
		{
			return std::filesystem::create_directory(m_current.path());
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::backwards()
	{
		if (m_current.has_parent())
		{
			m_current = m_current.parent();
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cfilesystem::append(const char* path)
	{
		m_current /= path;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfilesystem& cfilesystem::operator/=(const char* path)
	{
		append(path);
		return *this;
	}

} //- core

namespace fs
{
	namespace
	{
	} //- unnamed

	//- @reference: raylib UnloadFileData.
	//- unload data allocated by load_binary_file_data
	//------------------------------------------------------------------------------------------------------------------------
	void unload_file_binary_data(void* data)
	{
		CORE_FREE(data);
	}

	//- @reference: raylib LoadFileData. Sevure version. If 'error' is not null then it will be filled with an error message.
	//- Does append a null terminator at the end.
	//------------------------------------------------------------------------------------------------------------------------
	byte_t* load_binary_file_data(stringview_t file_path, unsigned* data_size_out)
	{
		byte_t* data = nullptr;
		*data_size_out = 0;

		if (file_path.data() != nullptr)
		{
			FILE* file = fopen(file_path.data(), "rb");

			if (file != NULL)
			{
				fseek(file, 0, SEEK_END);
				auto size = ftell(file);
				fseek(file, 0, SEEK_SET);

				if (size > 0)
				{
					data = (byte_t*)CORE_MALLOC(sizeof(byte_t) * size + 1);
					data[sizeof(byte_t) * size - 1] = '\0';

					unsigned count = SCAST(unsigned, fread(data, sizeof(byte_t), size, file));
					*data_size_out = count + 1;
				}

				fclose(file);
			}
			else
			{
				if (core::serror_reporter::instance().m_callback)
				{
					core::serror_reporter::instance().m_callback(core::logging_verbosity_error,
						fmt::format("Could not open file '{}'", file_path));
				}
			}
		}
		else
		{
			if (core::serror_reporter::instance().m_callback)
			{
				core::serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Invalid file path provided for load_binary_file_data");
			}
		}
		return data;
	}

	//- @reference: raylib SaveFileData
	//------------------------------------------------------------------------------------------------------------------------
	bool save_binary_file_data(stringview_t file_path, byte_t* data, unsigned data_size)
	{
		if (file_path.data() != nullptr)
		{
			FILE* file = fopen(file_path.data(), "wb");

			if (file != NULL)
			{
				unsigned count = (unsigned)fwrite(data, sizeof(byte_t), data_size, file);

				fclose(file);

				return count == data_size;
			}
			else
			{
				if (core::serror_reporter::instance().m_callback)
				{
					core::serror_reporter::instance().m_callback(core::logging_verbosity_error,
						fmt::format("Could not open file '{}'", file_path));
				}
			}
		}
		else
		{
			if (core::serror_reporter::instance().m_callback)
			{
				core::serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Invalid file path provided for save_binary_file_data");
			}
		}
		return false;
	}

	//- @reference: raylib UnloadFileText.
	//- unload data allocated by load_text_file_data
	//------------------------------------------------------------------------------------------------------------------------
	void unload_file_text_data(byte_t* text)
	{
		CORE_FREE(text);
	}

	//- @reference: raylib LoadFileText
	//------------------------------------------------------------------------------------------------------------------------
	byte_t* load_text_file_data(stringview_t file_path)
	{
		byte_t* text = nullptr;

		if (file_path.data() != nullptr)
		{
			FILE* file = fopen(file_path.data(), "rt");

			if (file != NULL)
			{
				fseek(file, 0, SEEK_END);
				auto size = SCAST(unsigned, ftell(file));
				fseek(file, 0, SEEK_SET);

				if (size > 0)
				{
					text = SCAST(byte_t*, CORE_MALLOC((size + 1) * sizeof(byte_t)));

					if (text != nullptr)
					{
						auto count = SCAST(unsigned, fread(text, sizeof(byte_t), size, file));

						if (count < size)
						{
							text = SCAST(byte_t*, CORE_REALLOC(text, count + 1));
						}

						text[count] = '\0';
					}
				}
				else
				{
					if (core::serror_reporter::instance().m_callback)
					{
						core::serror_reporter::instance().m_callback(core::logging_verbosity_error,
							fmt::format("Could not read text from file '{}'", file_path));
					}
				}
				fclose(file);
			}
			else
			{
				if (core::serror_reporter::instance().m_callback)
				{
					core::serror_reporter::instance().m_callback(core::logging_verbosity_error,
						fmt::format("Could not open file '{}'", file_path));
				}
			}
		}
		else
		{
			if (core::serror_reporter::instance().m_callback)
			{
				core::serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Invalid file path provided for load_text_file_data");
			}
		}
		return text;
	}

	//- @reference: raylib SaveFileText
	//------------------------------------------------------------------------------------------------------------------------
	bool save_text_file_data(stringview_t file_path, stringview_t text)
	{
		if (file_path.data() != nullptr)
		{
			FILE* file = fopen(file_path.data(), "wt");

			if (file != NULL)
			{
				auto count = fprintf(file, "%s", text.data());

				fclose(file);

				if (count > 0)
				{
					return true;
				}
				else
				{
					if (core::serror_reporter::instance().m_callback)
					{
						core::serror_reporter::instance().m_callback(core::logging_verbosity_error,
							fmt::format("Could not write text to file '{}'", file_path));
					}
				}
			}
			else
			{
				if (core::serror_reporter::instance().m_callback)
				{
					core::serror_reporter::instance().m_callback(core::logging_verbosity_error,
						fmt::format("Could not open file '{}'", file_path));
				}
			}
		}
		else
		{
			if (core::serror_reporter::instance().m_callback)
			{
				core::serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Invalid file path provided for save_text_file_data");
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cfileinfo::cfileinfo(stringview_t filepath) :
		std::filesystem::path(filepath.data()), m_relative(filepath.data())
	{
		init();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cfileinfo::cfileinfo(const cfileinfo& other) :
		std::filesystem::path(other.generic_u8string()), m_relative(other.m_relative), m_directory(other.m_directory), m_exists(other.m_exists), m_size(other.m_size)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cfileinfo::cfileinfo(stringview_t basepath, stringview_t filepath) :
		std::filesystem::path(fmt::format("{}{}", basepath.data(), filepath.data()))
	{
		if (string_utils::starts_with(filepath, "/"))
		{
			//- ignore the '/'
			m_relative = filepath.substr(1).data();
		}
		else
		{
			m_relative = filepath.data();
		}

		init();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cfileinfo::init()
	{
		const auto entry = std::filesystem::directory_entry(*this);

		m_directory = entry.is_directory();
		m_exists = entry.exists();

		//- std::filesystem throws on errors
		try
		{
			m_size = SCAST(unsigned, entry.file_size());
		}
		catch (...)
		{
		}
	}

	//- Path is the path concatenated from a filesystems basepath and the filename
	//------------------------------------------------------------------------------------------------------------------------
	string_t cfileinfo::path() const
	{
		return base_t::generic_u8string();
	}

	//- Relative path is the path without the filesystems basepath
	//------------------------------------------------------------------------------------------------------------------------
	string_t cfileinfo::relative() const
	{
		return m_relative;
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cfileinfo::name() const
	{
		return base_t::filename().generic_u8string();
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cfileinfo::stem() const
	{
		return base_t::stem().generic_u8string();
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cfileinfo::ext() const
	{
		return base_t::extension().generic_u8string();
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cfileinfo::directory_path() const
	{
		return base_t::parent_path().generic_u8string();
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cfileinfo::size() const
	{
		return m_size;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfileinfo::is_file() const
	{
		return !is_directory();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfileinfo::exists() const
	{
		return m_exists;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfileinfo::operator==(const cfileinfo& other) const
	{
		return path() == other.path();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfileinfo::is_directory() const
	{
		return m_directory;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cvirtual_filesystem::cvirtual_filesystem()
	{
		m_filesystems.reserve(C_FILESYSTEMS_RESERVE_COUNT);
		m_opened_files.reserve(C_FILESYSTEM_OPENED_FILES_RESERVE_COUNT);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cvirtual_filesystem::~cvirtual_filesystem()
	{
		std::for_each(m_filesystems.begin(), m_filesystems.end(), [](const auto& pair)
			{
				pair.second->shutdown();
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cvirtual_filesystem::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cvirtual_filesystem::on_shutdown()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cvirtual_filesystem::on_update(float)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cvirtual_filesystem::add_filesystem(stringview_t alias, filesystem_ref_t filesystem)
	{
		if (filesystem && filesystem->ready())
		{
			string_t alias_string = alias.data();

			if (!string_utils::ends_with(alias_string, "/"))
			{
				alias_string += "/";
			}

			if (const auto it = m_filesystems.find(alias_string); it == m_filesystems.end())
			{
				m_filesystems[alias_string] = filesystem;
				m_sorted_aliases[alias_string] = filesystem;

				if (core::serror_reporter::instance().m_callback)
				{
					core::serror_reporter::instance().m_callback(core::logging_verbosity_info,
						fmt::format("Filesystem '{}' with alias '{}' for real path '{}' added to Virtual File System",
							filesystem->filesystem_name().data(),
							alias_string.data(),
							filesystem->base_path().data()));
				}
			}
		}
		else
		{
			if (core::serror_reporter::instance().m_callback)
			{
				core::serror_reporter::instance().m_callback(core::logging_verbosity_error,
					fmt::format("Filesystem with alias '{}' could not be added to Virtual File System, because it is not ready!", alias.data()));
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cvirtual_filesystem::remove_filesystem(stringview_t alias)
	{
		string_t _alias = alias.data();

		if (!string_utils::ends_with(_alias, "/"))
		{
			_alias += "/";
		}

		if (const auto it = m_filesystems.find(_alias); it != m_filesystems.end())
		{
			m_filesystems.erase(it);
		}

		if (const auto it = m_sorted_aliases.find(_alias); it != m_sorted_aliases.end())
		{
			m_sorted_aliases.erase(it);
		}

		if (core::serror_reporter::instance().m_callback)
		{
			core::serror_reporter::instance().m_callback(core::logging_verbosity_info,
				fmt::format("Filesystem alias '{}' removed from Virtual File System", _alias));
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cvirtual_filesystem::does_filesystem_exists(stringview_t alias) const
	{
		string_t _alias = alias.data();

		if (!string_utils::ends_with(_alias, "/"))
		{
			_alias += "/";
		}

		return m_filesystems.find(_alias.data()) != m_filesystems.end();
	}

	//------------------------------------------------------------------------------------------------------------------------
	fs::filesystem_ref_t cvirtual_filesystem::find_filesystem(stringview_t alias) const
	{
		string_t _alias = alias.data();

		if (!string_utils::ends_with(_alias, "/"))
		{
			_alias += "/";
		}

		return m_filesystems.at(_alias.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	fs::file_ref_t cvirtual_filesystem::open(const cfileinfo& filepath, int file_mode)
	{
		file_ref_t file = nullptr;

		if (std::all_of(m_sorted_aliases.begin(), m_sorted_aliases.end(), [&](const auto& pair)
			{
				const auto& path = pair.first;
				const auto& filesystem = pair.second;

				if (string_utils::starts_with(filepath.path(), path) &&
					filepath.path().length() != path.length())
				{
					file = filesystem->open(filepath, file_mode);
				}

				if (file)
				{
					uint64_t address = RCAST(uint64_t, (void*)file.get());
					m_opened_files[address] = filesystem;

					//- Opened file. Stop.
					return false;
				}
				//- Did not find file. Proceed.
				return true;
			}))
		{
			//- report success or failure
			if (core::serror_reporter::instance().m_callback)
			{
				if (file)
				{
					core::serror_reporter::instance().m_callback(core::logging_verbosity_info,
						fmt::format("Successfully opening file '{}' with mode '{}'", filepath.name(), file_mode));
				}
				else
				{
					core::serror_reporter::instance().m_callback(core::logging_verbosity_warn,
						fmt::format("Failed opening file '{}' with mode '{}'", filepath.name(), file_mode));
				}
			}
		}

		return file;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cvirtual_filesystem::close(file_ref_t file)
	{
		uint64_t address = RCAST(uint64_t, (void*)file.get());

		if (const auto it = m_opened_files.find(address); it != m_opened_files.end())
		{
			//- Close file and erase from storage
			it->second->close(file);

			m_opened_files.erase(it);

			if (core::serror_reporter::instance().m_callback)
			{
				core::serror_reporter::instance().m_callback(core::logging_verbosity_info,
					fmt::format("Successfully closed file '{}'", file->info().name()));
			}
		}
		else
		{
			if (core::serror_reporter::instance().m_callback)
			{
				core::serror_reporter::instance().m_callback(core::logging_verbosity_warn,
					fmt::format("Failed closing file '{}'", file->info().name()));
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cvirtual_filesystem::exists(const cfileinfo& filepath) const
	{
		auto path = filepath.path();

		//- Locate the filesystem responsible for alias
		if (const auto it = algorithm::find_if(m_sorted_aliases.begin(), m_sorted_aliases.end(), [&](const auto& pair)
			{
				return string_utils::starts_with(path, pair.first);

			}); it != m_sorted_aliases.end())
		{
			return it->second->does_exist(filepath);
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	vector_t<fs::cfileinfo> cvirtual_filesystem::iterate(const cfileinfo& filepath,
		core::filesystem_lookup_type type, bool recursive) const
	{
		vector_t<fs::cfileinfo> result;

		if (std::all_of(m_sorted_aliases.begin(), m_sorted_aliases.end(), [&](const auto& pair)
			{
				const auto& path = pair.first;
				const auto& filesystem = pair.second;

				if (string_utils::starts_with(filepath.path(), path) &&
					filepath.path().length() != path.length())
				{
					const auto files = filesystem->iterate(filepath, type, recursive);

					algorithm::insert(result, files.begin(), files.end());

					return false;
				}

				return true;
			}))
		{
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	memory_ref_t load_text_from_file(stringview_t filepath)
	{
		memory_ref_t result = nullptr;

		if (const auto* text = load_text_file_data(filepath.data()); text)
		{
			//- copy loaded data
			result = core::cmemory::make_ref((byte_t*)text, (unsigned)strlen(text));

			//- free original data
			unload_file_text_data((byte_t*)text);
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<memory_ref_t> load_text_from_file_async(stringview_t filepath)
	{
		auto result = core::casync::launch_async([p = filepath]()
			{
				return load_text_from_file(p);

			}).share();

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	memory_ref_t load_binary_from_file(stringview_t filepath)
	{
		memory_ref_t result = nullptr;
		unsigned size = 0;

		if (const auto* data = load_binary_file_data(filepath.data(), &size); data && size > 0)
		{
			//- copy loaded data
			result = core::cmemory::make_ref((byte_t*)data, size);

			//- free original data
			unload_file_binary_data((void*)data);
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<memory_ref_t> load_binary_from_file_async(stringview_t filepath)
	{
		auto result = core::casync::launch_async([p = filepath]()
			{
				return load_binary_from_file(p);

			}).share();

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::file_io_status save_text_to_file(stringview_t filepath, const char* string)
	{
		return save_text_file_data(filepath, string) ? core::file_io_status_success : core::file_io_status_failed;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::file_io_status save_text_to_file(stringview_t filepath, const memory_ref_t& data)
	{
		return save_text_to_file(filepath, (const char*)data->data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<core::file_io_status> save_text_to_file_async(stringview_t filepath, const memory_ref_t& data)
	{
		auto result = core::casync::launch_async([p = filepath, d = data]()
			{
				return save_text_to_file(p, d);

			}).share();

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<core::file_io_status> save_text_to_file_async(stringview_t filepath, const char* string)
	{
		auto result = core::casync::launch_async([p = filepath, s = string]()
			{
				return save_text_to_file(p, s);

			}).share();

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::file_io_status save_binary_to_file(stringview_t filepath, const memory_ref_t& data)
	{
		return save_binary_to_file(filepath, (void*)data->data(), (unsigned)data->size());
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::file_io_status save_binary_to_file(stringview_t filepath, void* data, unsigned size)
	{
		return save_binary_file_data(filepath, (byte_t*)data, size) ? core::file_io_status_success : core::file_io_status_failed;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<core::file_io_status> save_binary_to_file_async(stringview_t filepath, const memory_ref_t& data)
	{
		auto result = core::casync::launch_async([p = filepath, d = data]()
			{
				return save_binary_to_file(p, d);

			}).share();

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<core::file_io_status> save_binary_to_file_async(stringview_t filepath, void* data, unsigned size)
	{
		auto result = core::casync::launch_async([p = filepath, d = data, s = size]()
			{
				return save_binary_to_file(p, d, s);

			}).share();

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	fs::cfileinfo ifilesystem::convert(const ifilesystem* filesystem, const cfileinfo& info)
	{
		auto path = info.relative();

		string_utils::erase_substr(path, filesystem->alias());

		return { filesystem->base_path(), path };
	}

} //- fs