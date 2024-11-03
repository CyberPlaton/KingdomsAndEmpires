#pragma once
#include <core.h>
#include <engine.h>
#include <spritemancer.h>

namespace editor::ui
{
	//- 
	//------------------------------------------------------------------------------------------------------------------------
	class ctext_filter final
	{
	public:
		enum matching_algorithm : uint8_t
		{
			matching_algorithm_none = 0,
			matching_algorithm_direct,
			matching_algorithm_fuzzy,
		};

		ctext_filter();
		~ctext_filter() = default;

		ctext_filter& text(const vector_t<string_t>& strings);
		ctext_filter& algorithm(matching_algorithm value);
		ctext_filter& matches(vector_t<int>* out);

		bool draw();

	private:
		string_t m_pattern;
		vector_t<string_t> m_strings;
		vector_t<int>* m_output;
		matching_algorithm m_algorithm = matching_algorithm_direct;
	};

} //- editor::ui