#pragma once
#include <core.h>

namespace io
{
	//- Native file implementation. Note that after each read/write operation the current cursor position shifts
	//- forwards by amount of bytes written or read.
	//------------------------------------------------------------------------------------------------------------------------
	class cnative_file final : public fs::ifile
	{
	public:
		cnative_file(const fs::cfileinfo& filepath);
		~cnative_file();

		const fs::cfileinfo& info() const override final;
		unsigned size() override final;
		bool read_only() override final;
		bool opened() override final;
		void open(int file_mode) override final;
		void close() override final;
		unsigned tell() override final;
		unsigned read(byte_t* buffer, unsigned datasize) override final;
		memory_ref_t read_sync() override final;
		core::cfuture_type<memory_ref_t> read_async() override final;
		unsigned write(const byte_t* buffer, unsigned datasize) override final;
		unsigned seek(int offset, core::file_seek_origin origin) override final;
		bool seek_to_start() override final;
		bool seek_to_end() override final;
		bool seek_to(unsigned offset) override final;

	private:
		fs::cfileinfo m_info;
		FILE* m_file;
		int m_state;					//- bitwise concated core::file_state
	};

} //- io
