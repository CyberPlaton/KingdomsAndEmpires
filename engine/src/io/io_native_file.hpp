#pragma once
#include <core.h>

namespace io
{
	//- Native file implementation. Note that after each read/write operation the current cursor position shifts
	//- forwards by amount of bytes written or read.
	//------------------------------------------------------------------------------------------------------------------------
	class cnative_file final : public core::fs::ifile
	{
	public:
		cnative_file(const core::fs::cfileinfo& filepath);
		~cnative_file();

		const core::fs::cfileinfo& info() const override final;
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
		core::fs::cfileinfo m_info;
		ptr_t<asio::stream_file> m_file;
		ptr_t<asio::io_context> m_context;
		int m_state;					//- bitwise concated core::file_state
		unsigned m_seek_position = 0;
	};

} //- io
