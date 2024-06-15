#pragma once
#include <core.h>

namespace io
{
	//------------------------------------------------------------------------------------------------------------------------
	class cnative_file final : public core::fs::ifile
	{
	public:
		cnative_file(core::fs::fileinfo_ref_t filepath);
		~cnative_file();

		core::fs::fileinfo_ref_t info() override final;
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
		core::fs::fileinfo_ref_t m_info;
		std::fstream m_stream;
		int m_state;	//- bitwise concated core::file_state
		int m_mode;		//- bitwise concated core::file_mode
	};

} //- io