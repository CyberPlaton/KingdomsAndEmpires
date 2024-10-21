#include "core_uuid.hpp"
#include "core_algorithm.hpp"

namespace core
{
	const array_t<unsigned char, 16u> cuuid::C_INVALID_DATA = { 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f' };
	const cuuid cuuid::C_INVALID_UUID = { C_INVALID_DATA };

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid()
	{
		generate(std::random_device()());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(const string_t& uuid)
	{
		parse_string(uuid, m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(size_t seed)
	{
		generate(seed);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(const cuuid& other)
	{
		copy_from(other);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(array_t<unsigned char, 16u> data)
	{
		std::memcpy(&m_data[0], &data[0], sizeof(unsigned char) * 16);
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cuuid::hash() const
	{
		return algorithm::hash(string().c_str());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::generate(size_t seed)
	{
		//- setup random number generator
		static thread_local std::mt19937_64 random_engine;
		static thread_local bool seeded = false;
		if (!seeded)
		{
			random_engine.seed(seed);
			seeded = true;
		}
		std::uniform_int_distribution<std::mt19937_64::result_type> dist(MIN(size_t), MAX(size_t));

		//- compute
		unsigned i, j, rnd;
		for (i = 0; i < (16 / C_RANDOM_BYTES_COUNT); i++)
		{
			rnd = SCAST(unsigned, dist(random_engine));
			for (j = 0; j < C_RANDOM_BYTES_COUNT; j++)
			{
				m_data[i * C_RANDOM_BYTES_COUNT + j] = (0xff & rnd >> (8 * j));
			}
		}
		//- set the version to 4
		m_data[6] = (m_data[6] & 0x0f) | 0x40;
		//- set the variant to 1 (a)
		m_data[8] = (m_data[8] & 0x0f) | 0xa0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::parse_string(const string_t& uuid, array_t<unsigned char, 16u>& out)
	{
		unsigned i = 0, j = 0;
		while (i < 36 && j < 16)
		{
			if (uuid[i] != '-')
			{
				out[j++] = (hex2dec(uuid[i++]) << 4) | hex2dec(uuid[i]);
			}
			i++;
		}

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::write_string(const array_t<unsigned char, 16>& data, string_t& out) const
	{
		out.resize(36);
		unsigned i = 0, j = 0;
		while (j < 16)
		{
			if (j == 4 || j == 6 || j == 8 || j == 10)
			{
				out[i++] = '-';
			}
			out[i++] = C_HEX[(data[j] >> 4)];
			out[i++] = C_HEX[(0xf & data[j])];
			j++;
		}
		out[36] = '\0';
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cuuid::hex2dec(char c)
	{
		for (auto i = 0u; i < 16; ++i)
		{
			if (C_HEX[i] == c)
			{
				return i;
			}
		}
		return -1;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::copy_to(cuuid& other)
	{
		std::memcpy(&other.m_data[0], &m_data[0], 16);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::copy_from(const cuuid& other)
	{
		std::memcpy(&m_data[0], &other.m_data[0], 16);
	}

	//------------------------------------------------------------------------------------------------------------------------
	int cuuid::compare(const cuuid& other) const
	{
		return std::memcmp(&m_data[0], &other.m_data[0], 16);
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cuuid::generate_string() const
	{
		string_t s;
		write_string(m_data, s);
		return s;
	}

} //- core