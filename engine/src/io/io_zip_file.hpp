#pragma once
#include <core.h>

namespace io
{
	namespace detail
	{
		class czip;
		using zipfile_ref_t = ref_t<czip>;

		//------------------------------------------------------------------------------------------------------------------------
		class czip
		{
		public:
			czip(stringview_t path);
			~czip();

			bool map_file(stringview_t filename, vector_t<uint8_t>& data);
			stringview_t filepath() const;
			bool read_only() const;

		private:
			string_t m_filename;
			miniz::mz_zip_archive* m_zip_archive;
			map_t<string_t, std::pair<unsigned, uint64_t>> m_entries;
		};

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	class czip_file final : public core::fs::ifile
	{
	public:
		czip_file(const core::fs::cfileinfo& filepath, detail::zipfile_ref_t zip);
		~czip_file();

		const core::fs::cfileinfo& info() const override final;
		unsigned size() override final;
		bool read_only() override final;
		bool opened() override final;
		void open(int file_mode) override final;
		void close() override final;
		unsigned seek(unsigned offset, core::file_seek_origin origin) override final;
		unsigned tell() override final;
		unsigned read(byte_t* buffer, unsigned datasize) override final;
		unsigned write(const byte_t* buffer, unsigned datasize) override final;

	private:
		core::fs::cfileinfo m_info;
		detail::zipfile_ref_t m_zip;
		vector_t<uint8_t> m_memory;
		int m_state;	//- bitwise concated core::file_state
		int m_mode;		//- bitwise concated core::file_mode
		unsigned m_seek_pos;
	};

} //- io