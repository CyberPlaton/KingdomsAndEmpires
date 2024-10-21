#include "core_algorithm.hpp"
#include "core_stringutils.hpp"

namespace algorithm
{
	namespace matching
	{
		//------------------------------------------------------------------------------------------------------------------------
		bool direct(stringview_t pattern, stringview_t text, vector_t<int>& matches_out)
		{
			auto result = false;
			const auto pattern_length = pattern.length();
			const auto text_length = text.length();

			if (pattern_length > text_length ||
				pattern_length == 0 || text_length == 0)
			{
				return result;
			}

			//- initialize pattern mask to highest possible character in ASCII format
			array_t<int, 256> mask; std::memset(&mask[0], -1, 256 * sizeof(int));

			for (auto i = 0; i < pattern_length; ++i)
			{
				mask[(byte_t)pattern[i]] &= ~BIT(i);
			}

			auto state = ~1;

			for (auto i = 0; i < text_length; ++i)
			{
				state |= mask[(byte_t)text[i]];
				state <<= 1;

				if (!algorithm::bit_check(state, BIT(pattern_length)))
				{
					matches_out.push_back(i - pattern_length + 1);
					result = true;
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool fuzzy(stringview_t pattern, stringview_t text, vector_t<int>& matches_out)
		{
			static constexpr auto  C_STRING_DISTANCE_MAX = 2;

			const char* pattern_string = pattern.data();
			const char* text_string = text.data();
			auto index = 0;

			while (*pattern_string != '\0' && *text_string != '\0')
			{
				if (string_utils::to_lower(*pattern_string) == string_utils::to_lower(*text_string) &&
					string_utils::distance_levenshtein(pattern_string, text_string) <= C_STRING_DISTANCE_MAX)
				{
					++pattern_string;
					matches_out.push_back(index);
				}
				++text_string;
				++index;
			}

			return *pattern_string == '\0' ? true : false;
		}

	} //- matching

	//------------------------------------------------------------------------------------------------------------------------
	unsigned hash(const char* string)
	{
		unsigned len = SCAST(unsigned, strlen(string));
		const char* s = string;
		unsigned h = len;
		for (auto i = 0u; i < len; ++s, ++i)
		{
			h = ((h << 5) + h) + (*s);
		}
		return h;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned hash(stringview_t string)
	{
		return hash(string.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned hash(const string_t& string)
	{
		return hash(string.data());
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
	bool is_valid_proxy(entity_proxy_t h)
	{
		return h >= 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	handle_type_t invalid_handle()
	{
		return MAX(handle_type_t);
	}

	//------------------------------------------------------------------------------------------------------------------------
	float bytes_to_kilobytes(unsigned b)
	{
		return SCAST(float, b) / 1024.0f;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float bytes_to_megabytes(unsigned b)
	{
		return bytes_to_kilobytes(b) / 1024.0f;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float bytes_to_gigabytes(unsigned b)
	{
		return bytes_to_megabytes(b) / 1024.0f;
	}

	//------------------------------------------------------------------------------------------------------------------------
	byte_t encode_utf8(byte_t out[4], unsigned scancode)
	{
		uint8_t length = 0;

		if (scancode < 0x80)
		{
			out[length++] = (char)scancode;
		}
		else if (scancode < 0x800)
		{
			out[length++] = (scancode >> 6) | 0xc0;
			out[length++] = (scancode & 0x3f) | 0x80;
		}
		else if (scancode < 0x10000)
		{
			out[length++] = (scancode >> 12) | 0xe0;
			out[length++] = ((scancode >> 6) & 0x3f) | 0x80;
			out[length++] = (scancode & 0x3f) | 0x80;
		}
		else if (scancode < 0x110000)
		{
			out[length++] = (scancode >> 18) | 0xf0;
			out[length++] = ((scancode >> 12) & 0x3f) | 0x80;
			out[length++] = ((scancode >> 6) & 0x3f) | 0x80;
			out[length++] = (scancode & 0x3f) | 0x80;
		}

		return length;
	}

} //- algorithm