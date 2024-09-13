#include "editor_element_text_filter.hpp"
#include "editor_element_text_input.hpp"

namespace editor::ui
{
	namespace
	{
		constexpr stringview_t C_TEXT_INPUT_ID = "##textFilterTextInput";
		constexpr stringview_t C_TEXT_INPUT_HINT = "Filter...";
		constexpr stringview_t C_TEXT_INPUT_TOOLTIP = "Enter a string or UUID to filter entities and prefabs";
		constexpr vec2_t C_TEXT_INPUT_SIZE = { 100.0f, 25.0f };

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	ctext_filter::ctext_filter()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctext_filter& ctext_filter::text(const vector_t<string_t>& strings)
	{
		m_strings = strings;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctext_filter& ctext_filter::algorithm(matching_algorithm value)
	{
		m_algorithm = value;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctext_filter& ctext_filter::matches(vector_t<int>* out)
	{
		m_output = out;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool ctext_filter::draw()
	{
		ctext_input text_input(C_TEXT_INPUT_ID);

		text_input
			.value(&m_pattern)
			.multiline(false)
			.hint(C_TEXT_INPUT_HINT)
			.size(C_TEXT_INPUT_SIZE)
			.option(ctext_input::options_true_on_enter, true)
			.tooltip(C_TEXT_INPUT_TOOLTIP);

		auto result = false;
		if (const auto changed = text_input.draw(); changed)
		{
			for (const auto& text : m_strings)
			{
				switch (m_algorithm)
				{
				default:
				case matching_algorithm_direct:
				{
					if (const auto filtered = algorithm::matching::direct(m_pattern, text, *m_output); filtered)
					{
						result = true;
					}
					break;
				}
				case matching_algorithm_fuzzy:
				{
					if (const auto filtered = algorithm::matching::fuzzy(m_pattern, text, *m_output); filtered)
					{
						result = true;
					}
					break;
				}
				}
			}
		}
		return result;
	}

} //- editor::ui