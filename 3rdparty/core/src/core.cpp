#include "core.hpp"
#include <sstream>

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
	b2AABB aabb(const core::srect& rect)
	{
		return {{rect.x() + rect.w() / 2.0f, rect.y() + rect.h() / 2.0f},
				{rect.w() / 2.0f, rect.h() / 2.0f}};
	}

	//------------------------------------------------------------------------------------------------------------------------
	b2AABB aabb(const vec2_t& center, const vec2_t& halfextents)
	{
		b2AABB out{{ center.x - halfextents.x, center.y - halfextents.y },
					{ center.x + halfextents.x, center.y + halfextents.y }};
		return out;
	}

	//------------------------------------------------------------------------------------------------------------------------
	b2AABB aabb(float x, float y, float size)
	{
		return aabb({ x + size / 2.0f, y + size / 2.0f },
					{ size / 2.0f, size / 2.0f });
	}

	//------------------------------------------------------------------------------------------------------------------------
	b2AABB aabb(float x, float y, float w, float h)
	{
		return aabb({ x + w / 2.0f, y + h / 2.0f },
					{ w / 2.0f, h / 2.0f });
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::srect rect(const b2AABB& bbox)
	{
		const auto& c = bbox.GetCenter();
		const auto& he = bbox.GetExtents();

		return { c.x - he.x, c.y - he.y, he.x * 2.0f, he.y * 2.0f };
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
	const core::cuuid cuuid::C_INVALID_UUID;

	namespace
	{
		inline static constexpr stringview_t C_EMPTY_STRING = "";
		inline static constexpr size_t C_TO_SEC = 1 / (1000 * 1000);
		inline static constexpr size_t C_TO_MILLISEC = 1 / (1000);

		//------------------------------------------------------------------------------------------------------------------------
		inline static bool is_path_directory(stringview_t path)
		{
			return string_utils::find_substr(path.data(), ".") == MAX(size_t);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static bool is_path_file(stringview_t path)
		{
			return string_utils::find_substr(path.data(), ".") != MAX(size_t);
		}

		//- @reference: raylib UnloadFileData.
		//- unload data allocated by load_binary_file_data
		//------------------------------------------------------------------------------------------------------------------------
		inline static void unload_file_binary_data(uint8_t* data)
		{
			mi_free(data);
		}

		//- @reference: raylib LoadFileData
		//------------------------------------------------------------------------------------------------------------------------
		inline static uint8_t* load_binary_file_data(stringview_t file_path, unsigned* data_size_out)
		{
			uint8_t* data = nullptr;
			*data_size_out = 0;

			if (file_path.data() != nullptr)
			{
				FILE* file = fopen(file_path.data(), "rb");

				if (file != nullptr)
				{
					fseek(file, 0, SEEK_END);
					auto size = ftell(file);
					fseek(file, 0, SEEK_SET);

					if (size > 0)
					{
						data = (uint8_t*)mi_malloc(sizeof(uint8_t) * size);

						unsigned count = SCAST(unsigned, fread(data, sizeof(uint8_t), size, file));
						*data_size_out = count;
					}
				}
			}
			return data;
		}

		//- @reference: raylib SaveFileData
		//------------------------------------------------------------------------------------------------------------------------
		inline static bool save_binary_file_data(stringview_t file_path, uint8_t* data, unsigned data_size)
		{
			if (file_path.data() != nullptr)
			{
				FILE* file = fopen(file_path.data(), "wb");

				if (file != nullptr)
				{
					auto count = SCAST(unsigned, fwrite(data, sizeof(uint8_t), data_size, file));

					fclose(file);

					if (count == data_size)
					{
						return true;
					}
				}
			}
			return false;
		}

		//- @reference: raylib UnloadFileText.
		//- unload data allocated by load_text_file_data
		//------------------------------------------------------------------------------------------------------------------------
		inline static void unload_file_text_data(char* text)
		{
			mi_free(text);
		}

		//- @reference: raylib LoadFileText
		//------------------------------------------------------------------------------------------------------------------------
		inline static char* load_text_file_data(stringview_t file_path)
		{
			char* text = nullptr;

			if (file_path.data() != nullptr)
			{
				FILE* file = fopen(file_path.data(), "rt");

				if (file != NULL)
				{
					fseek(file, 0, SEEK_END);
					auto size = SCAST(unsigned, ftell(file));
					fseek(file, 0, SEEK_SET);

					if (size > 0)
					{
						text = SCAST(char*, mi_malloc((size + 1) * sizeof(char)));

						if (text != nullptr)
						{
							auto count = SCAST(unsigned, fread(text, sizeof(char), size, file));

							if (count < size)
							{
								text = SCAST(char*, mi_realloc(text, count + 1));
							}

							text[count] = '\0';
						}
					}
					fclose(file);
				}
			}
			return text;
		}

		//- @reference: raylib SaveFileText
		//------------------------------------------------------------------------------------------------------------------------
		inline static bool save_text_file_data(stringview_t file_path, stringview_t text)
		{
			if (file_path.data() != nullptr)
			{
				FILE* file = fopen(file_path.data(), "wt");

				if (file != nullptr)
				{
					auto count = fprintf(file, "%s", text.data());

					fclose(file);

					if (count > 0)
					{
						return true;
					}
				}
			}
			return false;
		}

	} //- unnamed

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

	namespace detail
	{

	} //- detail

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

	//------------------------------------------------------------------------------------------------------------------------
	scolor::scolor() :
		m_r(0), m_g(0), m_b(0), m_a(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	scolor::scolor(common_color color) :
		m_r(0), m_g(0), m_b(0), m_a(0)
	{
		static constexpr spair<common_color, array_t<uint8_t, 4>> C_COMMON_COLORS[] = {
		{common_color_red400,		{134, 38, 51, 255}},
		{common_color_red200,		{239, 83, 80, 255}},
		{common_color_red50,		{255, 205, 210, 255}},

		{common_color_orange400,	{245, 124, 0, 255}},
		{common_color_orange200,	{255, 171, 64, 255}},
		{common_color_orange50,		{255, 224, 178, 255}},

		{common_color_yellow400,	{251, 192, 45, 255}},
		{common_color_yellow200,	{255, 235, 59, 255}},
		{common_color_yellow50,		{255, 245, 157, 255}},

		{common_color_green400,		{52, 168, 83, 255}},
		{common_color_green200,		{129, 199, 132, 255}},
		{common_color_green50,		{200, 230, 201, 255}},

		{common_color_cyan400,		{0, 188, 212, 255}},
		{common_color_cyan200,		{77, 208, 225, 255}},
		{common_color_cyan50,		{178, 235, 242, 255}},

		{common_color_blue400,		{33, 150, 243, 255}},
		{common_color_blue200,		{144, 202, 249, 255}},
		{common_color_blue50,		{187, 222, 251, 255}},

		{common_color_magenta400,	{156, 39, 176, 255}},
		{common_color_magenta200,	{233, 30, 99, 255}},
		{common_color_magenta50,	{248, 187, 208, 255}},

		{common_color_pink400,		{233, 30, 99, 255}},
		{common_color_pink200,		{240, 98, 146, 255}},
		{common_color_pink50,		{248, 187, 208, 255}},

		{common_color_transparent,	{255, 255, 255, 255}},
		{common_color_neutral0,		{0, 0, 0, 255}},		//- black
		{common_color_neutral100,	{33, 33, 33, 255}},
		{common_color_neutral200,	{66, 66, 66, 255}},
		{common_color_neutral300,	{117, 117, 117, 255}},
		{common_color_neutral400,	{153, 153, 153, 255}},
		{common_color_neutral500,	{189, 189, 189, 255}},
		{common_color_neutral600,	{224, 224, 224, 255}},
		{common_color_neutral800,	{238, 238, 238, 255}},
		{common_color_neutral1000,	{255, 255, 255, 255}}, //- white
		};

		const auto& c = C_COMMON_COLORS[color].second;
		m_r = c[0];
		m_g = c[1];
		m_b = c[2];
		m_a = c[3];
	}

	//------------------------------------------------------------------------------------------------------------------------
	scolor::scolor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
		m_r(r), m_g(g), m_b(b), m_a(a)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	scolor::scolor(vec4_t normalized) :
		m_r(SCAST(uint8_t, glm::clamp(normalized.r, 0.0f, 1.0f) * 255.0f)),
		m_g(SCAST(uint8_t, glm::clamp(normalized.g, 0.0f, 1.0f) * 255.0f)),
		m_b(SCAST(uint8_t, glm::clamp(normalized.b, 0.0f, 1.0f) * 255.0f)),
		m_a(SCAST(uint8_t, glm::clamp(normalized.a, 0.0f, 1.0f) * 255.0f))
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	srect::srect(const vec2_t& xy, const vec2_t& wh) :
		m_x(xy.x), m_y(xy.y), m_w(wh.x), m_h(wh.y)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	srect::srect(float x /*= 0.0f*/, float y /*= 0.0f*/, float w /*= 0.0f*/, float h /*= 0.0f*/) :
		m_x(x), m_y(y), m_w(w), m_h(h)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void srect::set(float x, float y, float w, float h)
	{
		m_x = x;
		m_y = y;
		m_w = w;
		m_h = h;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void srect::set_position(float x, float y)
	{
		m_x = x;
		m_y = y;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void srect::set_dimension(float w, float h)
	{
		m_w = w;
		m_h = h;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath::cpath(stringview_t path) :
		m_path(path.data())
	{
		m_dir = std::filesystem::directory_entry(m_path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath::cpath(const std::filesystem::path& path) :
		m_path(path)
	{
		m_dir = std::filesystem::directory_entry(m_path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath::cpath(const cpath& path) :
		m_path(path.m_path)
	{
		m_dir = std::filesystem::directory_entry(m_path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	std::filesystem::path cpath::path() const
	{
		return m_path;
	}

	//------------------------------------------------------------------------------------------------------------------------
	std::filesystem::directory_entry cpath::dir() const
	{
		return m_dir;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::exists() const
	{
		return m_dir.exists();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::has_extension() const
	{
		return m_path.has_extension();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::has_parent() const
	{
		return m_path.has_parent_path();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::is_dir() const
	{
		return m_dir.is_directory();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::is_file() const
	{
		return m_dir.is_regular_file();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath cpath::parent() const
	{
		return { m_path.parent_path() };
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath& cpath::append(stringview_t path)
	{
		this->operator/=(path);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::operator==(const std::filesystem::path& path)
	{
		return m_path == path;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::operator==(stringview_t path)
	{
		return view() == path;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath& cpath::operator/=(stringview_t path)
	{
		m_path /= path.data();
		m_dir = std::filesystem::directory_entry(m_path);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath& cpath::operator=(const cpath& path)
	{
		m_path = path.m_path;
		m_dir = path.m_dir;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cfilesystem::cfilesystem(cpath path) :
		m_current(std::move(path))
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cpath cfilesystem::construct(stringview_t path, stringview_t addition)
	{
		return cpath(path).append(addition);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cpath cfilesystem::cwd()
	{
		try
		{
			return std::filesystem::current_path().c_str();
		}
		catch (...)
		{
		}
		return cpath(C_EMPTY_STRING);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_dir(stringview_t path)
	{
		return std::filesystem::create_directory(path.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_dir_in(stringview_t path, stringview_t name)
	{
		return create_dir(cpath(path).append(name).view());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_dir_recursive(stringview_t path)
	{
		return std::filesystem::create_directories(path.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_file(stringview_t path)
	{
		std::ofstream out(path.data());

		std::filesystem::permissions(path.data(),
			std::filesystem::perms::owner_all | std::filesystem::perms::group_all,
			std::filesystem::perm_options::add);

		out.close();

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_file_in(stringview_t path, stringview_t stem, stringview_t ext)
	{
		return create_file(fmt::format("{}/{}.{}", path.data(), stem.data(), ext.data()).data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_file(stringview_t path, stringview_t name)
	{
		return find_at(path, name, filesystem_lookup_type_file);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_file_by_stem(stringview_t path, stringview_t name)
	{
		cpath p(path);
		if (p.exists())
		{
			for (const auto& entry : std::filesystem::directory_iterator{ p.path() })
			{
				if (entry.is_regular_file() && entry.path().stem().generic_u8string().c_str() == name)
				{
					return true;
				}
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_file_by_extension(stringview_t path, stringview_t name)
	{
		cpath p(path);
		if (p.exists())
		{
			for (const auto& entry : std::filesystem::directory_iterator{ p.path() })
			{
				if (entry.is_regular_file() &&
					entry.path().has_extension() &&
					entry.path().extension().generic_u8string().c_str() == name)
				{
					return true;
				}
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_dir(stringview_t path, stringview_t name)
	{
		return find_at(path, name, filesystem_lookup_type_directory);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_at(stringview_t path, stringview_t name, filesystem_lookup_type type)
	{
		cpath p(path);
		if (p.exists())
		{
			p.append(name);

			for (const auto& entry : std::filesystem::directory_iterator{ p.path() })
			{
				switch (type)
				{
				default:
				case filesystem_lookup_type_any:
				{
					if (p == entry.path())
					{
						return true;
					}
					break;
				}
				case filesystem_lookup_type_directory:
				{
					if (entry.is_directory() && p == entry.path())
					{
						return true;
					}
					break;
				}
				case filesystem_lookup_type_file:
				{
					if (entry.is_regular_file() && p == entry.path())
					{
						return true;
					}
					break;
				}
				}
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cpath cfilesystem::construct_relative_to_cwd(stringview_t path)
	{
		auto absolute = std::filesystem::absolute(cfilesystem::cwd().path());

		return std::filesystem::relative(path.data(), absolute);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::forwards(stringview_t path, bool forced /*= false*/)
	{
		//- create copy
		cpath copy(m_current);

		copy /= path;

		if (copy.exists())
		{
			m_current = std::move(copy);
			return true;
		}
		else if(forced)
		{
			return std::filesystem::create_directory(m_current.path());
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::backwards()
	{
		if (m_current.has_parent())
		{
			m_current = m_current.parent();
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------

	void cfilesystem::append(stringview_t path)
	{
		m_current /= path;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfilesystem& cfilesystem::operator/=(stringview_t path)
	{
		append(path);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cfile::cfile(const cpath& path, int mode /*= file_read_write_mode_read | file_read_write_mode_text*/) :
		m_data(nullptr), m_datasize(0), m_mode(mode), m_status(file_io_status_none), m_path(path.view())
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cfile::~cfile()
	{
		if (m_task.valid())
		{
			m_task.wait();
		}
		if (m_data && m_datasize > 0)
		{
			if (!!(m_mode & file_read_write_mode_text))
			{
				//- unload text file data
				unload_file_text_data(SCAST(char*, m_data));
			}
			else if (!!(m_mode & file_read_write_mode_binary))
			{
				//- unload binary file data
				unload_file_binary_data(SCAST(uint8_t*, m_data));
			}
			m_data = nullptr;
			m_datasize = 0;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::file_io_status cfile::read_sync()
	{
		ASSERT(!!(m_mode & file_read_write_mode_read), "cfile must be created with read mode");

		if (!m_task.valid())
		{
			if (!!(m_mode & file_read_write_mode_text))
			{
				//- load text file
				m_data = SCAST(void*, load_text_file_data(m_path));
			}
			else if (!!(m_mode & file_read_write_mode_binary))
			{
				//- load binary file
				m_data = SCAST(void*, load_binary_file_data(m_path, &m_datasize));
			}

			if (m_data && m_datasize > 0)
			{
				m_status = file_io_status_success;
			}
			else
			{
				m_status = file_io_status_failed;
			}
		}
		else
		{
			m_task.wait();
		}
		return m_status;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::file_io_status cfile::write_sync(void* data, unsigned data_size)
	{
		ASSERT(!!(m_mode & file_read_write_mode_write), "cfile must be created with write mode");

		if (!m_task.valid())
		{
			if (!!(m_mode & file_read_write_mode_text) && m_data && m_datasize > 0)
			{
				//- write text file data
				m_status = save_text_file_data(m_path.data(), SCAST(char*, m_data)) == true ? file_io_status_success : file_io_status_failed;
			}
			else if (!!(m_mode & file_read_write_mode_binary) && m_data && m_datasize > 0)
			{
				//- write binary file date
				m_status = save_binary_file_data(m_path.data(), SCAST(uint8_t*, m_data), m_datasize) == true ? file_io_status_success : file_io_status_failed;
			}
			else
			{
				m_status = file_io_status_failed;
			}
		}
		else
		{
			m_task.wait();
		}
		return m_status;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::file_io_status cfile::read_async()
	{
		ASSERT(!!(m_mode & file_read_write_mode_read), "cfile must be created with read mode");

		if (!m_task.valid())
		{
			//- begin async load operation
			m_task = casync::launch_async([&]() -> void
				{
					m_status = file_io_status_pending;

					if (!!(m_mode & file_read_write_mode_text))
					{
						//- load text file
						m_data = SCAST(void*, load_text_file_data(m_path));
					}
					else if (!!(m_mode & file_read_write_mode_binary))
					{
						//- load binary file
						m_data = SCAST(void*, load_binary_file_data(m_path, &m_datasize));
					}
					else
					{
						m_status = file_io_status_failed;
						return;
					}

					if (m_data && m_datasize > 0)
					{
						m_status = file_io_status_success;
					}
					else
					{
						m_status = file_io_status_failed;
					}
				});
		}

		return m_status;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::file_io_status cfile::write_async(void* data, unsigned data_size)
	{
		ASSERT(!!(m_mode & file_read_write_mode_write), "cfile must be created with write mode");

		if (!m_task.valid())
		{
			//- begin async save operation
			m_task = casync::launch_async([&]() -> void
				{
					m_status = file_io_status_pending;

					if (!!(m_mode & file_read_write_mode_text) && m_data && m_datasize > 0)
					{
						//- write text file data
						m_status = save_text_file_data(m_path.data(), SCAST(char*, m_data)) == true ? file_io_status_success : file_io_status_failed;
					}
					else if (!!(m_mode & file_read_write_mode_binary) && m_data && m_datasize > 0)
					{
						//- write binary file data
						m_status = save_binary_file_data(m_path.data(), SCAST(uint8_t*, m_data), m_datasize) == true ? file_io_status_success : file_io_status_failed;
					}
					else
					{
						m_status = file_io_status_failed;
						return;
					}
				});
		}

		return m_status;
	}

	using namespace std::chrono_literals;

	//------------------------------------------------------------------------------------------------------------------------
	ctimer::ctimer(bool paused /*= true*/) :
		m_timepoint(0s)
	{
		if (!paused)
		{
			start();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctimer::start()
	{
		m_timepoint = std::chrono::high_resolution_clock::now();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool ctimer::started() const
	{
		return m_timepoint.time_since_epoch().count() > 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	size_t ctimer::secs() const
	{
		auto e = microsecs();

		return e * C_TO_SEC;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float ctimer::millisecs() const
	{
		auto e = microsecs();

		return SCAST(float, e * C_TO_MILLISEC);
	}

	//------------------------------------------------------------------------------------------------------------------------
	size_t ctimer::microsecs() const
	{
		ASSERT(started(), "Timer must be started before it can be used");

		auto now = std::chrono::high_resolution_clock::now();

		return SCAST(size_t, std::chrono::time_point_cast<std::chrono::microseconds>(now).time_since_epoch().count() -
			std::chrono::time_point_cast<std::chrono::microseconds>(m_timepoint).time_since_epoch().count());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cany::~cany()
	{
		if (!empty())
		{
			m_data.reset();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cany& cany::operator=(const cany& other)
	{
		m_data = other.m_data;
		return *this;
	}

} //- core