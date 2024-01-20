#include "core.hpp"
#include <sstream>

RTTR_REGISTRATION
{

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
		unsigned len = string.length();
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
		return (part_value / total_value) * 100;
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
		void split(const string_t& string, char delimiter, std::vector< string_t >& storage)
		{
			std::stringstream ss(string);
			string_t token;

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

	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(const string_t& uuid)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(::size_t seed)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(const cuuid& other)
	{
		copy_from(other);
		m_string = generate_string();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::generate(::size_t seed)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::parse_string(const string_t& uuid, array_t<unsigned char, 16u>& out)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::write_string(const array_t<unsigned char, 16>& data, string_t& out) const
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cuuid::hex2dec(char c)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::copy_to(cuuid& other)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::copy_from(const cuuid& other)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	int cuuid::compare(const cuuid& other) const
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cuuid::generate_string() const
	{

	}

} //- core