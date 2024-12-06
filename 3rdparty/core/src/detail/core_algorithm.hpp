#pragma once
#include "core_config.hpp"

namespace algorithm
{
	namespace matching
	{
		//- Direct matching algorithm that matches any number of words and returns true if
		//- there is at least one match.
		//------------------------------------------------------------------------------------------------------------------------
		bool direct(stringview_t pattern, stringview_t text, vector_t<int>& matches_out);

		//------------------------------------------------------------------------------------------------------------------------
		bool fuzzy(stringview_t pattern, stringview_t text, vector_t<int>& matches_out);

	} //- matching

	unsigned hash(stringview_t string);
	unsigned percentage(float total_value, float part_value);
	float percent_value(unsigned p, float total_value);
	bool is_valid_handle(handle_type_t h);
	bool is_valid_proxy(entity_proxy_t h);
	handle_type_t invalid_handle();
	float bytes_to_kilobytes(unsigned b);
	float bytes_to_megabytes(unsigned b);
	float bytes_to_gigabytes(unsigned b);
	byte_t encode_utf8(byte_t out[4], unsigned scancode);

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void bit_set(int& byte, T bit)
	{
		static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "Invalid operation. T must be an integral or enum type");

		byte |= static_cast<int>(bit);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void bit_clear(int& byte, T bit)
	{
		static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "Invalid operation. T must be an integral or enum type");

		byte &= ~static_cast<int>(bit);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void bit_flip(int& byte, T bit)
	{
		static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "Invalid operation. T must be an integral or enum type");

		byte ^= static_cast<int>(bit);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	bool bit_check(const int& byte, T bit)
	{
		static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "Invalid operation. T must be an integral or enum type");

		return (byte & static_cast<int>(bit)) != 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType, typename TIterator>
	bool find(TIterator begin, TIterator end, const TType& value)
	{
		return stl::find(begin, end, value) != end;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TCallable, typename TIterator>
	TIterator find_if(TIterator begin, TIterator end, TCallable&& callable)
	{
		return stl::find_if(begin, end, callable);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType, typename TIterator>
	TIterator find_at(TIterator begin, TIterator end, const TType& value)
	{
		return stl::find(begin, end, value);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType, typename TIterator>
	TType& get(TIterator begin, TIterator end, const TType& value)
	{
		auto it = stl::find(begin, end, value);

		return *it;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TEnum>
	string_t enum_to_string(TEnum value)
	{
		return magic_enum::enum_name(value).data();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TEnum>
	TEnum string_to_enum(stringview_t value)
	{
		if (const auto opt = magic_enum::enum_cast<TEnum>(std::string_view(value.data())); opt.has_value())
		{
			return opt.value();
		}
		return (TEnum)0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TStructure>
	void swap(TStructure& swap_to, TStructure& swap_from)
	{
		stl::swap(swap_to, swap_from);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TStructure>
	void erase_at_index(TStructure& structure, unsigned index)
	{
		structure.erase(structure.begin() + index);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TStructure, typename TIterator>
	void erase_at(TStructure& structure, TIterator it)
	{
		structure.erase(it);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TStructure, typename TType>
	void erase_first(TStructure& structure, TType value)
	{
		auto it = structure.find(value);
		if (it != structure.end())
		{
			structure.erase(it);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TStructure, typename TIterator>
	void insert(TStructure& structure, TIterator begin, TIterator end)
	{
		structure.insert(structure.end(), begin, end);
	}

	//- Iterate a data structure and call given function with the iterator as argument.
	//- If the callable returns true then we erase the element and return true, if nothing is removed we return false.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TStructure, typename TCallable>
	bool erase_if(TStructure& structure, TCallable&& function)
	{
		for (auto it = structure.begin(); it != structure.end(); ++it)
		{
			if (function(it))
			{
				structure.erase(it);
				return true;
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TStructure, typename TCallable>
	void copy_if(const TStructure& from, TStructure& to, TCallable&& function)
	{
		stl::copy_if(from.begin(), from.end(), to, function);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TIterator>
	void shuffle(TIterator begin, TIterator end, unsigned seed = time(NULL))
	{
		std::random_device device;
		std::mt19937 generator(device());

		stl::shuffle(begin, end, generator);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TIterator, typename TCallable>
	void sort(TIterator begin, TIterator end, TCallable&& function)
	{
		stl::sort(begin, end, function);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TIterator>
	void copy_to(TIterator from_begin, TIterator from_end, TIterator to_begin)
	{
		stl::copy(from_begin, from_end, to_begin);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TIterator, typename TCallable>
	void for_each(TIterator begin, TIterator end, TCallable&& function)
	{
		stl::for_each(begin, end, function);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TIterator>
	void reverse(TIterator begin, TIterator end)
	{
		stl::reverse(begin, end);
	}

} //- algorithm