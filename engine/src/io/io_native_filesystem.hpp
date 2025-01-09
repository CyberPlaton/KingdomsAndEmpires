#pragma once
#include <core.h>
#include "io_native_file.hpp"

namespace io
{
	//------------------------------------------------------------------------------------------------------------------------
	class cnative_filesystem final : public fs::ifilesystem
	{
	public:
		cnative_filesystem(bool build_file_list = true);
		~cnative_filesystem();

		bool init(stringview_t basepath, stringview_t alias) override final;
		void shutdown() override final;
		bool ready() const override final;
		stringview_t filesystem_name() const override final;

		string_t alias() const override final;
		string_t base_path() const override final;
		fs::filelist_t files() const override final;
		bool does_exist(const fs::cfileinfo& filepath) const override final;

		fs::file_ref_t open(const fs::cfileinfo& filepath, int file_mode) override final;
		void close(fs::file_ref_t file) override final;
		bool create_file(const fs::cfileinfo& filepath) override final;
		bool remove_file(const fs::cfileinfo& filepath) override final;
		bool copy_file(const fs::cfileinfo& source, const fs::cfileinfo& dest) override final;
		bool rename_file(const fs::cfileinfo& source, const fs::cfileinfo& dest) override final;

		vector_t<fs::cfileinfo> iterate(const fs::cfileinfo& path, core::filesystem_lookup_type type, bool recursive) const override final;

	private:
		fs::filelist_t m_file_list;
		string_t m_basepath;
		string_t m_alias;
		bool m_ready;
		bool m_build_file_list;

	private:
		fs::file_ref_t find_file(const fs::cfileinfo& fileinfo) const;
		void build_file_list(string_t path, fs::filelist_t& out);
	};

} //- io