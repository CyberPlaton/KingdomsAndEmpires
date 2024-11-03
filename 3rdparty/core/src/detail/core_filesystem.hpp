#pragma once
#include "core_config.hpp"
#include "core_future_type.hpp"
#include "core_service_manager.hpp"

//- Utility filesystem. Will be removed later.
//------------------------------------------------------------------------------------------------------------------------
namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	class cpath final
	{
	public:
		cpath(const char* path);
		cpath(const std::filesystem::path& path);
		cpath(const cpath& path);
		cpath& operator=(const cpath& path);
		~cpath() = default;

		std::filesystem::path path() const;
		std::filesystem::directory_entry dir() const;

		const char* view() const { return m_string_path.c_str(); }
		const char* extension() const { return m_string_ext.c_str(); }
		const char* stem() const { return m_string_stem.c_str(); }

		bool exists() const;
		explicit operator bool() const { return exists(); }

		bool has_extension() const;
		bool has_parent() const;
		bool is_dir() const;
		bool is_file() const;

		cpath parent() const;
		cpath& append(stringview_t path);
		cpath& operator /=(stringview_t path);
		bool operator==(stringview_t path);
		bool operator==(const std::filesystem::path& path);

	private:
		string_t m_string_path;
		string_t m_string_ext;
		string_t m_string_stem;
		std::filesystem::path m_path;
		std::filesystem::directory_entry m_dir;

	private:
		//- TODO: Reconsider later how to deal with filesystem wchar_t and ditch the 3 strings
		void update_strings();
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cfilesystem final
	{
	public:
		cfilesystem(cpath path);
		cfilesystem(const char* path);
		~cfilesystem() = default;

		static cpath construct(const char* path, const char* addition);
		static cpath join(const char* path, const char* addition);
		static cpath cwd();

		static bool create_dir(const char* path);
		static bool create_dir_in(const char* path, const char* name);
		static bool create_dir_recursive(const char* path);

		static bool create_file(const char* path);
		static bool create_file_in(const char* path, const char* stem, const char* ext);

		static bool remove(const char* path);
		static bool remove(const char* path, const char* name);

		static bool find_file(const char* path, const char* name);
		static bool find_file_by_stem(const char* path, const char* name);
		static bool find_file_by_extension(const char* path, const char* name);
		static bool find_dir(const char* path, const char* name);
		static bool find_at(const char* path, const char* name, filesystem_lookup_type type);

		static cpath construct_relative_to_cwd(const char* path);
		static bool is_contained(const cpath& contained, const cpath& container);

		inline const cpath& current() const { return m_current; }

		bool forwards(const char* path, bool forced = false);
		bool backwards();

		void append(const char* path);
		cfilesystem& operator/=(const char* path);

		inline bool exists() const { return m_current.exists(); };
		inline bool is_file() const { return m_current.is_file(); };
		inline bool is_dir() const { return m_current.is_dir(); };

	private:
		cpath m_current;
	};

} //- core


//- Implementation of a virtual filesystem inspired by
//- @reference https://github.com/yevgeniy-logachev/vfspp
//------------------------------------------------------------------------------------------------------------------------
namespace fs
{
	//- Utility functions to load data from files directly without using VFS
	byte_t*									load_text_file_data(stringview_t file_path);
	bool									save_text_file_data(stringview_t file_path, stringview_t text);
	void									unload_file_text_data(byte_t* text);

	byte_t*									load_binary_file_data(stringview_t file_path, unsigned* data_size_out);
	bool									save_binary_file_data(stringview_t file_path, byte_t* data, unsigned data_size);
	void									unload_file_binary_data(void* data);

	memory_ref_t							load_text_from_file(stringview_t filepath);
	core::cfuture_type<memory_ref_t>		load_text_from_file_async(stringview_t filepath);
	memory_ref_t							load_binary_from_file(stringview_t filepath);
	core::cfuture_type<memory_ref_t>		load_binary_from_file_async(stringview_t filepath);
	core::file_io_status					save_text_to_file(stringview_t filepath, const char* string);
	core::file_io_status					save_text_to_file(stringview_t filepath, const memory_ref_t& data);
	core::cfuture_type<core::file_io_status>save_text_to_file_async(stringview_t filepath, const memory_ref_t& data);
	core::cfuture_type<core::file_io_status>save_text_to_file_async(stringview_t filepath, const char* string);
	core::file_io_status					save_binary_to_file(stringview_t filepath, void* data, unsigned size);
	core::file_io_status					save_binary_to_file(stringview_t filepath, const memory_ref_t& data);
	core::cfuture_type<core::file_io_status>save_binary_to_file_async(stringview_t filepath, const memory_ref_t& data);
	core::cfuture_type<core::file_io_status>save_binary_to_file_async(stringview_t filepath, void* data, unsigned size);

	class ifile;
	class cfileinfo;
	class ifilesystem;

	using file_ref_t = ref_t<ifile>;
	using filelist_t = uset_t<file_ref_t>;
	using filesystem_ref_t = ref_t<ifilesystem>;

	//- Class holding general information about a file. Dividing cold and hot data, can also be stored separately where required.
	//- Contains basically all information currently provided by core::cpath but with a smaller memory footprint.
	//------------------------------------------------------------------------------------------------------------------------
	class cfileinfo final : public std::filesystem::path
	{
		using base_t = std::filesystem::path;
	public:
		cfileinfo(const cfileinfo& other);
		cfileinfo(stringview_t basepath, stringview_t filepath);
		cfileinfo(stringview_t filepath);
		~cfileinfo() = default;

		//- complete path of file containing the basepath from filesystem and relative path from file
		string_t path() const;

		//- path from file only, not containing the basepath of any fileystem
		string_t relative() const;

		//- equivalent to std::filesystem::filename
		string_t name() const;
		string_t stem() const;
		string_t ext() const;
		string_t directory_path() const;
		unsigned size() const;
		bool is_directory() const;
		bool is_file() const;
		bool exists() const;

		bool operator==(const cfileinfo& other) const;

	private:
		string_t m_relative;
		unsigned m_size;
		bool m_directory;
		bool m_exists;

	private:
		void init();
	};

	//- File interface. For some functionality you can use enums file_state and file_mode
	//------------------------------------------------------------------------------------------------------------------------
	class ifile
	{
	public:
		virtual const cfileinfo& info() const = 0;
		virtual unsigned size() = 0;
		virtual bool read_only() = 0;
		virtual bool opened() = 0;
		virtual void open(int file_mode) = 0;
		virtual void close() = 0;
		virtual unsigned seek(int offset, core::file_seek_origin origin) = 0;
		virtual unsigned tell() = 0;
		virtual unsigned read(byte_t* buffer, unsigned datasize) = 0;
		virtual unsigned write(const byte_t* buffer, unsigned datasize) = 0;

		//- helper functions that reads full data from file
		virtual memory_ref_t read_sync() = 0;
		virtual core::cfuture_type<memory_ref_t> read_async() = 0;


		//- Seek to start of file
		virtual bool seek_to_start() = 0;

		//- Seek to end of file
		virtual bool seek_to_end() = 0;

		//- Seek to position between start and end of file
		virtual bool seek_to(unsigned offset) = 0;

		int filemode() const { return m_filemode; }

		template<typename TType>
		bool read(TType& value) { return read(&value, sizeof(TType)) == sizeof(TType); }

		template<typename TType>
		bool write(const TType& value);

	protected:
		int m_filemode = core::file_mode_none;
	};

	//- Interface for a file system implemenation.
	//------------------------------------------------------------------------------------------------------------------------
	class ifilesystem
	{
	public:
		virtual bool init(stringview_t basepath, stringview_t alias) = 0;
		virtual void shutdown() = 0;
		virtual bool ready() const = 0;
		virtual stringview_t filesystem_name() const = 0;

		virtual string_t alias() const = 0;
		virtual string_t base_path() const = 0;
		virtual filelist_t files() const = 0;
		virtual bool does_exist(const cfileinfo& filepath) const = 0;

		virtual file_ref_t open(const cfileinfo& filepath, int file_mode) = 0;
		virtual void close(file_ref_t file) = 0;
		virtual bool create_file(const cfileinfo& filepath) = 0;
		virtual bool remove_file(const cfileinfo& filepath) = 0;
		virtual bool copy_file(const cfileinfo& source, const cfileinfo& dest) = 0;
		virtual bool rename_file(const cfileinfo& source, const cfileinfo& dest) = 0;

		virtual vector_t<cfileinfo> iterate(const cfileinfo& path, core::filesystem_lookup_type type, bool recursive) const = 0;

	protected:
		//- Retrieve fileinfo in a format that will be useful to filesystem, i.e. erase alias path
		static cfileinfo convert(const ifilesystem* filesystem, const cfileinfo& info);
	};

	//- Virtual file system implementation.
	//------------------------------------------------------------------------------------------------------------------------
	class cvirtual_filesystem final : public core::cservice
	{
	public:
		static constexpr auto C_FILESYSTEMS_RESERVE_COUNT = 16;
		static constexpr auto C_FILESYSTEM_OPENED_FILES_RESERVE_COUNT = 256;

		cvirtual_filesystem();
		~cvirtual_filesystem();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		void add_filesystem(stringview_t alias, filesystem_ref_t filesystem);
		void remove_filesystem(stringview_t alias);
		bool does_filesystem_exists(stringview_t alias) const;
		filesystem_ref_t find_filesystem(stringview_t alias) const;
		file_ref_t open(const cfileinfo& filepath, int file_mode);
		void close(file_ref_t file);
		bool exists(const cfileinfo& filepath) const;

		vector_t<cfileinfo> iterate(const cfileinfo& filepath, core::filesystem_lookup_type type, bool recursive) const;

	private:
		umap_t<string_t, filesystem_ref_t> m_filesystems;
		umap_t<uint64_t, filesystem_ref_t> m_opened_files;
		map_t<string_t, filesystem_ref_t> m_sorted_aliases;

		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	bool fs::ifile::write(const TType& value)
	{
		if constexpr (std::is_same_v<TType, string_t> || std::is_same_v<TType, stringview_t>)
		{
			return write((const byte_t*)value.c_str(), SCAST(unsigned, value.size()));
		}
		else
		{
			return write(&value, sizeof(TType)) == sizeof(TType);
		}
	}

} //- fs