#pragma once
#include "core_config.hpp"

namespace core
{
	//- Random number generator
	//------------------------------------------------------------------------------------------------------------------------
	class crandom final
	{
	public:
		crandom() = default;
		~crandom() = default;

		static void seed(unsigned value = 0);

		//- retrieve a random number deviating in range of given value by plus a positive amount or
		//- by minus a negative amount
		int random_deviation(int value, int negative, int positive);

		float random_float();
		unsigned random_int();
		float alternate_one_float();
		int alternate_one_int();
		float normalized_float();
		float in_range_float(float minimum, float maximum);
		unsigned in_range_int(unsigned minimum, unsigned maximum);
		vec2_t normalized_vec2();
		vec2_t in_range_vec2(float minimum, float maximum);

	private:
		inline static std::mt19937 C_RANDOM_ENGINE;
		std::uniform_int_distribution<std::mt19937::result_type> m_distribution;
	};

} //- core