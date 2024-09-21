#pragma once
#include <core.h>
#include "io_native_file.hpp"

namespace io
{
	//------------------------------------------------------------------------------------------------------------------------
	class cnative_filesystem final : public core::fs::ifilesystem
	{
	public:
		cnative_filesystem();
		~cnative_filesystem();

		bool init(stringview_t basepath, stringview_t alias) override final;
		void shutdown() override final;
		bool ready() const override final;
		stringview_t filesystem_name() const override final;

		string_t alias() const override final;
		string_t base_path() const override final;
		core::fs::filelist_t files() const override final;
		bool does_exist(const core::fs::cfileinfo& filepath) const override final;

		core::fs::file_ref_t open(const core::fs::cfileinfo& filepath, int file_mode) override final;
		void close(core::fs::file_ref_t file) override final;
		bool create_file(const core::fs::cfileinfo& filepath) override final;
		bool remove_file(const core::fs::cfileinfo& filepath) override final;
		bool copy_file(const core::fs::cfileinfo& source, const core::fs::cfileinfo& dest) override final;
		bool rename_file(const core::fs::cfileinfo& source, const core::fs::cfileinfo& dest) override final;

		vector_t<core::fs::cfileinfo> iterate(const core::fs::cfileinfo& path, core::filesystem_lookup_type type, bool recursive) const override final;

	private:
		core::fs::filelist_t m_file_list;
		string_t m_basepath;
		string_t m_alias;
		bool m_ready;

	private:
		core::fs::file_ref_t find_file(const core::fs::cfileinfo& fileinfo) const;
		void build_file_list(string_t path, core::fs::filelist_t& out);
	};

} //- io