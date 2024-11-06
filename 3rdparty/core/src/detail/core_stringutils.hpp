#pragma once
#include "core_config.hpp"

namespace string_utils
{
	unsigned distance(stringview_t first, stringview_t second);
	unsigned distance_levenshtein(stringview_t first, stringview_t second);
	void split(const string_t& string, char delimiter, stl::vector<string_t>& storage);
	void insert(string_t& string, const string_t& to_insert_one, size_t index);
	void push_front(string_t& string, const string_t& to_prepend_one);
	void push_back(string_t& string, const string_t& to_append_one);
	void erase_substr(string_t& string, const string_t& string_to_erase);
	void erase_range(string_t& string, size_t begin, size_t end);
	void to_lower(string_t& string);
	size_t length(const string_t& string);
	size_t find_substr(const string_t& string, const string_t& substring);
	string_t substr(const string_t& string, size_t offset, size_t count = std::string::npos);
	string_t replace(const string_t& string, const string_t& substr, const string_t& replacement);
	bool does_substr_exist(const string_t& string, const string_t& substring);
	bool compare(const string_t& first, const string_t& second);
	bool starts_with(stringview_t string, stringview_t substr);
	bool ends_with(stringview_t string, stringview_t substr);
	bool is_in_range(char c, char from, char to);
	bool is_space(char c);
	bool is_upper(char c);
	bool is_lower(char c);
	bool is_alpha(char c);
	bool is_numeric(char c);
	bool is_alpha_numeric(char c);
	char to_lower(char c);
	char to_upper(char c);

} //- string_utils