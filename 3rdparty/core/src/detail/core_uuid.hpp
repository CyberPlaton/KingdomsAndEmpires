#pragma once
#include "core_config.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	class cuuid final
	{
	public:
		static const cuuid C_INVALID_UUID;
		static const array_t<unsigned char, 16u> C_INVALID_DATA;

		cuuid();
		cuuid(array_t<unsigned char, 16u> data);
		cuuid(const string_t& uuid);
		cuuid(size_t seed);
		cuuid(const cuuid& other);
		~cuuid() = default;

		string_t string() const { return generate_string(); }
		unsigned hash() const;
		bool is_equal_to(const cuuid& uuid) const { return compare(uuid) == 0; }
		bool is_smaller_as(const cuuid& uuid) const { return compare(uuid) < 0; }
		bool is_higher_as(const cuuid& uuid) const { return compare(uuid) > 0; }
		bool operator<(const cuuid& uuid) const { return is_smaller_as(uuid); }
		bool operator==(const cuuid& other) const { return is_equal_to(other); }
		bool operator!=(const cuuid& other) const { return !is_equal_to(other); };

	private:
		inline static const auto C_RANDOM_BYTES_COUNT = 4;
		inline static const unsigned char C_HEX[] = "0123456789abcdef";

		array_t<unsigned char, 16u> m_data = C_INVALID_DATA;

	private:
		void generate(size_t seed);
		void parse_string(const string_t& uuid, array_t<unsigned char, 16u>& out);
		void write_string(const array_t<unsigned char, 16>& data, string_t& out) const;
		unsigned hex2dec(char c);
		void copy_to(cuuid& other);
		void copy_from(const cuuid& other);
		int compare(const cuuid& other) const;
		string_t generate_string() const;

		RTTR_ENABLE();
		RTTR_REGISTRATION_FRIEND;
	};

} //- core

namespace std
{
	//- Specializations for use with std template structures
	//------------------------------------------------------------------------------------------------------------------------
	template<>
	struct hash<core::cuuid>
	{
		std::size_t operator()(const core::cuuid& uuid) const
		{
			return uuid.hash();
		}
	};
}