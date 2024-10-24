#pragma once
#include <core.h>

namespace io
{
	class cmemory_filesystem;

	//------------------------------------------------------------------------------------------------------------------------
	class cmemory_file final : public fs::ifile
	{
		friend class cmemory_filesystem;
	public:
		cmemory_file(const fs::cfileinfo& filepath);
		~cmemory_file();

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
		vector_t<uint8_t> m_memory;
		int m_state;	//- bitwise concated core::file_state
		unsigned m_seek_position;
	};

} //- io