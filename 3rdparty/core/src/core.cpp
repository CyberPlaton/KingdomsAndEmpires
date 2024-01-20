#include "core.hpp"
#include <sstream>

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace core;
	registration::class_<cuuid>("cuuid")
		.property("m_data",		&cuuid::m_data)
		.property("m_string",	&cuuid::m_string)
		.property("m_hash",		&cuuid::m_hash);

	registration::class_<vec2_t>("vec2_t")
		.property("x",			&vec2_t::x)
		.property("y",			&vec2_t::y);

	registration::class_<spair<handle_type_t, handle_type_t>>("spair")
		.property("first",		&spair<handle_type_t, handle_type_t>::first)
		.property("second",		&spair<handle_type_t, handle_type_t>::second);

	registration::class_<smaterial_pair>("smaterial_pair")
		.property("first",		&smaterial_pair::first)
		.property("second",		&smaterial_pair::second);


};

namespace algorithm
{
	//------------------------------------------------------------------------------------------------------------------------
	vec3_t decompose_rotation(const mat4_t& transform)
	{
		vec3_t out;

		decompose_rotation(transform, out);

		return out;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void decompose_rotation(const mat4_t& transform, vec3_t& out)
	{
		glm::extractEulerAngleXYZ(transform, out.x, out.y, out.z);
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec3_t decompose_translation(const mat4_t& transform)
	{
		return { transform[3] };
	}

	//------------------------------------------------------------------------------------------------------------------------
	void decompose_translation(const mat4_t& transform, vec3_t& out)
	{
		out = { transform[3] };
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec3_t decompose_scale(const mat4_t& transform)
	{
		return { glm::length(vec3_t(transform[0])), glm::length(vec3_t(transform[1])), glm::length(vec3_t(transform[2])) };
	}

	//------------------------------------------------------------------------------------------------------------------------
	void decompose_scale(const mat4_t& transform, vec3_t& out)
	{
		out.x = glm::length(vec3_t(transform[0]));
		out.y = glm::length(vec3_t(transform[1]));
		out.z = glm::length(vec3_t(transform[2]));
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned hash(stringview_t string)
	{
		auto len = static_cast<unsigned>(string.length());
		const char* s = string.data();
		unsigned h = len;
		for (auto i = 0u; i < len; ++s, ++i)
		{
			h = ((h << 5) + h) + (*s);
		}
		return h;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned hash(const core::cuuid& uuid)
	{
		return hash(uuid.view());
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned percentage(float total_value, float part_value)
	{
		return SCAST(unsigned, (part_value / total_value) * 100);
	}

	//------------------------------------------------------------------------------------------------------------------------
	float percent_value(unsigned p, float total_value)
	{
		if (p < 1) p = 1;
		if (p > 100) p = 100;
		return total_value * (SCAST(float, p) / 100.0f);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid_handle(handle_type_t h)
	{
		return h != MAX(handle_type_t);
	}

	//------------------------------------------------------------------------------------------------------------------------
	handle_type_t invalid_handle()
	{
		return MAX(handle_type_t);
	}

} //- algorithm

namespace core
{

	namespace string_utils
	{
		//------------------------------------------------------------------------------------------------------------------------
		void split(const string_t& string, char delimiter, stl::vector< string_t >& storage)
		{
			std::stringstream ss(string.c_str());
			std::string token;

			while (std::getline(ss, token, delimiter))
			{
				storage.emplace_back(token.c_str());
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void insert(string_t& string, const string_t& to_insert_one, size_t index)
		{
			string.insert(index, to_insert_one);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void push_front(string_t& string, const string_t& to_prepend_one)
		{
			string.insert(0, to_prepend_one);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void push_back(string_t& string, const string_t& to_append_one)
		{
			string.append(to_append_one);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void erase_substr(string_t& string, const string_t& substring_to_erase)
		{
			auto index = string.find(substring_to_erase);
			if (index != std::string::npos)
			{
				string.erase(index, substring_to_erase.size());
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void erase_range(string_t& string, size_t begin, size_t end)
		{
			string.erase(string.begin() + begin, string.begin() + end);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void to_lower(string_t& string)
		{
			std::transform(string.begin(), string.end(), string.begin(), [](unsigned char c) {return std::tolower(c); });
		}

		//------------------------------------------------------------------------------------------------------------------------
		size_t length(const string_t& string)
		{
			return string.size();
		}

		//------------------------------------------------------------------------------------------------------------------------
		size_t find_substr(const string_t& string, const string_t& substring)
		{
			return string.find(substring);
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool does_substr_exist(const string_t& string, const string_t& substring)
		{
			return string.find(substring) != std::string::npos;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool compare(const string_t& first, const string_t& second)
		{
			return first == second;
		}

	} //- string_utils

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid()
	{
		generate(std::random_device()());
		m_hash = algorithm::hash(*this);
		m_string = generate_string();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(const string_t& uuid)
	{
		parse_string(uuid, m_data);
		m_hash = algorithm::hash(uuid.data());
		m_string = generate_string();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(size_t seed)
	{
		generate(seed);
		m_hash = algorithm::hash(*this);
		m_string = generate_string();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(const cuuid& other)
	{
		copy_from(other);
		m_string = generate_string();
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
		other.m_hash = m_hash;
		other.m_string = other.generate_string();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::copy_from(const cuuid& other)
	{
		std::memcpy(&m_data[0], &other.m_data[0], 16);
		m_hash = other.m_hash;
		m_string = generate_string();
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

	//------------------------------------------------------------------------------------------------------------------------
	void crandom::seed(unsigned value/* = 0*/)
	{
		if (value == 0)
		{
			C_RANDOM_ENGINE.seed(std::random_device()());
		}
		else
		{
			C_RANDOM_ENGINE.seed(value);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	float crandom::random_float()
	{
		return SCAST(float, m_distribution(C_RANDOM_ENGINE));
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned crandom::random_int()
	{
		return m_distribution(C_RANDOM_ENGINE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	float crandom::alternate_one_float()
	{
		return (m_distribution(C_RANDOM_ENGINE) % 2 == 0) ? -1.0f : 1.0f;
	}

	//------------------------------------------------------------------------------------------------------------------------
	int crandom::alternate_one_int()
	{
		return (m_distribution(C_RANDOM_ENGINE) % 2 == 0) ? -1 : 1;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float crandom::normalized_float()
	{
		return SCAST(float, m_distribution(C_RANDOM_ENGINE) / MAX(unsigned));
	}

	//------------------------------------------------------------------------------------------------------------------------
	float crandom::in_range_float(float minimum, float maximum)
	{
		return minimum + normalized_float() * maximum;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned crandom::in_range_int(unsigned minimum, unsigned maximum)
	{
		std::uniform_int_distribution<std::mt19937::result_type> dist(minimum, maximum);

		return dist(C_RANDOM_ENGINE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t crandom::normalized_vec2()
	{
		return { normalized_float(), normalized_float() };
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t crandom::in_range_vec2(float minimum, float maximum)
	{
		return { in_range_float(minimum, maximum), in_range_float(minimum, maximum) };
	}

#if CORE_PLATFORM_WINDOWS || CORE_PLATFORM_XBOXONE || CORE_PLATFORM_XBOXSERIES
	#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <errno.h>
	typedef CRITICAL_SECTION pthread_mutex_t;
	typedef unsigned pthread_mutexattr_t;
	inline int pthread_mutex_lock(pthread_mutex_t* _mutex)
	{
		EnterCriticalSection(_mutex);
		return 0;
	}

	inline int pthread_mutex_unlock(pthread_mutex_t* _mutex)
	{
		LeaveCriticalSection(_mutex);
		return 0;
	}

	inline int pthread_mutex_trylock(pthread_mutex_t* _mutex)
	{
		return TryEnterCriticalSection(_mutex) ? 0 : EBUSY;
	}

	inline int pthread_mutex_init(pthread_mutex_t* _mutex, pthread_mutexattr_t* /*_attr*/)
	{
		InitializeCriticalSection(_mutex);
		return 0;
	}

	inline int pthread_mutex_destroy(pthread_mutex_t* _mutex)
	{
		DeleteCriticalSection(_mutex);
		return 0;
	}
#endif

	//------------------------------------------------------------------------------------------------------------------------
	cmutex::cmutex()
	{
		static_assert(sizeof(pthread_mutex_t) <= sizeof(m_internal), "Critical");

		pthread_mutexattr_t attr;

#if CORE_PLATFORM_WINDOWS || CORE_PLATFORM_XBOXONE || CORE_PLATFORM_XBOXSERIES
#else
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#endif
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_init(handle, &attr);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmutex::~cmutex()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_destroy(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmutex::lock()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_lock(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmutex::unlock()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_unlock(handle);
	}

} //- core