#include "core_stringutils.hpp"

namespace string_utils
{
	//------------------------------------------------------------------------------------------------------------------------
	unsigned distance(stringview_t first, stringview_t second)
	{
		if (first.length() <= second.length())
		{
			unsigned d = 0u;

			for (auto i = 0u; i < first.length(); ++i)
			{
				if (first[i] != second[i]) ++d;
			}

			return d;
		}

		return distance(second, first);
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned distance_levenshtein(stringview_t first, stringview_t second)
	{
		const auto first_length = first.length();
		const auto second_length = second.length();

		if (first_length == 0) return second_length;
		if (second_length == 0) return first_length;

		//- Resize the matrices to required sizes
		vector_t<vector_t<int>> matrix(first_length + 1, vector_t<int>(second_length + 1));

		for (auto i = 0; i < first_length; ++i)
		{
			matrix[i][0] = i;
		}
		for (auto i = 0; i < second_length; ++i)
		{
			matrix[0][i] = i;
		}

		for (auto i = 1; i < first_length; ++i)
		{
			for (auto j = 1; j < second_length; ++j)
			{
				const auto cost = second[j - 1] == first[i - 1] ? 0 : 1;

				matrix[i][j] = std::min(
					{
						matrix[i - 1][j] + 1,		//- deletion
						matrix[i][j - 1] + 1,		//- insertion
						matrix[i - 1][j - 1] + cost //- substitution
					});
			}
		}

		return matrix[first_length][second_length];
	}

	//------------------------------------------------------------------------------------------------------------------------
	void split(const string_t& string, char delimiter, stl::vector<string_t>& storage)
	{
		std::stringstream ss(string.c_str());
		string_t token;

		while (std::getline(ss, token, delimiter))
		{
			storage.emplace_back(token);
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
	void erase_substr(string_t& string, const string_t& string_to_erase)
	{
		auto index = string.find(string_to_erase);
		if (index != string_t::npos)
		{
			string.erase(index, string_to_erase.size());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t replace(const string_t& string, const string_t& substr, const string_t& replacement)
	{
		string_t out = string;

		if (const auto it = find_substr(string, substr); it != std::string::npos)
		{
			out.replace(it, substr.length(), replacement);
		}

		return out;
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
	string_t substr(const string_t& string, size_t offset, size_t count /*= std::string::npos*/)
	{
		return string.substr(offset, count);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool does_substr_exist(const string_t& string, const string_t& substring)
	{
		return string.find(substring) != string_t::npos;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool compare(const string_t& first, const string_t& second)
	{
		return first == second;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool ends_with(stringview_t string, stringview_t substr)
	{
		if (string.length() >= substr.length())
		{
			return string.compare(string.length() - substr.length(), substr.length(), substr.data()) == 0;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t join(const string_t& string, const string_t& other)
	{
		return fmt::format("{}{}", string, other);
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t erase_numeric_in_path(const string_t& string)
	{
		size_t position = 0;

		if (const auto p = string.find_last_of("/"); p != std::string::npos)
		{
			position = p;
		}

		string_t dir = string.substr(0, position);
		string_t filename = string.substr(position + 1);

		filename.erase(std::remove_if(filename.begin(), filename.end(), [](const char& c)
			{
				return is_numeric(c);
			}));

		return join(dir, filename);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool starts_with(stringview_t string, stringview_t substr)
	{
		if (string.length() >= substr.length())
		{
			return string.compare(0, substr.length(), substr.data()) == 0;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_in_range(char c, char from, char to)
	{
		return unsigned(c - from) <= unsigned(to - from);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_space(char c)
	{
		return ' ' == c		//- Space.
			|| '\t' == c	//- Horizontal tab.
			|| '\n' == c	//- Line feed / new line.
			|| '\r' == c	//- Carriage return.
			|| '\v' == c	//- Vertical tab.
			|| '\f' == c	//- Form feed / new page.
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_upper(char c)
	{
		return is_in_range(c, 'A', 'Z');
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_lower(char c)
	{
		return is_in_range(c, 'a', 'z');
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_alpha(char c)
	{
		return is_lower(c) || is_upper(c);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_numeric(char c)
	{
		return is_in_range(c, '0', '9');
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_alpha_numeric(char c)
	{
		return is_alpha(c) || is_numeric(c);
	}

	//------------------------------------------------------------------------------------------------------------------------
	char to_lower(char c)
	{
		return c + (is_upper(c) ? 0x20 : 0);
	}

	//------------------------------------------------------------------------------------------------------------------------
	char to_upper(char c)
	{
		return c - (is_lower(c) ? 0x20 : 0);
	}

} //- string_utils