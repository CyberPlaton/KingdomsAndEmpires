#include "editor_text_editor_frontend.hpp"

namespace sm
{
	namespace textedit
	{
		namespace
		{
			//-------------------------------------------------------------------------------------------------------
			vec2_t character_size(char c)
			{
				const char* start = &c;
				const char* end = start + sizeof(char);

				const auto vec = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), MAX(float), -1.0f, start, end);

				return {vec.x, vec.y};
			}

			//-------------------------------------------------------------------------------------------------------
			ImU32 imgui_color(const core::scolor& color)
			{
				const auto c = color.normalize();

				return ImGui::ColorConvertFloat4ToU32({ c.r, c.g, c.b, c.a });
			}

		} //- unnamed

		//-------------------------------------------------------------------------------------------------------
		cfrontend::cfrontend(const ref_t<cbackend>& backend) :
			m_backend(backend), m_cursor({0, 0})
		{
		}

		//-------------------------------------------------------------------------------------------------------
		cfrontend::~cfrontend()
		{

		}

		//-------------------------------------------------------------------------------------------------------
		bool cfrontend::init()
		{
			return true;
		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::shutdown()
		{

		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::draw()
		{
			prepare();

			if (ImGui::BeginChild("##container"))
			{
				show_main_window();

				ImGui::EndChild();
			}

			finish();
		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::prepare()
		{
			//- If styling has changed, reset required variables
		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::show_main_menu()
		{

		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::show_main_window()
		{
			const auto zoom = 1.0f;
			const auto fontsize = ImGui::GetFontSize(); //- does include font window scale
			const auto screenpos = ImGui::GetCursorScreenPos();
			const auto row_number = glm::floor(ImGui::GetScrollY() / fontsize);
			const auto row_we_at = row_number == 0 ? 0 : static_cast<int>(0.5f + screenpos.y / (row_number));
			const auto rows_we_see = static_cast<unsigned>(0.5f + ImGui::GetWindowContentRegionMax().y / (fontsize * zoom));
			const auto rows_to_draw_down = glm::min(m_backend->row_count(), row_we_at + rows_we_see);
			const auto rows_to_draw_up = glm::max(0, static_cast<int>(row_we_at - rows_we_see));
			const auto x = screenpos.x + fontsize; //- add one character for x, so we can add line numbers later
			const auto y = screenpos.y;

			//- rows to draw start such that our cursor is always in the middle
			for (auto i = rows_to_draw_up; i < rows_to_draw_down; ++i)
			{
				const auto& source = m_backend->row_at(i);

				const auto row_x = x;
				const auto row_y = y + i * fontsize;

				draw_text(row_x, row_y, {255, 150, 150, 255}, source.m_text.data());
			}
		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::finish()
		{

		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::draw_text(float x, float y, const core::scolor& color,
			const char* begin, const char* end /*= (const char*)0*/)
		{
			const auto draw_list = ImGui::GetWindowDrawList();

			draw_list->AddText({x, y}, imgui_color(color), begin, end);
		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::draw_char(float x, float y, const core::scolor& color, char c)
		{
			const char* start = &c;
			const char* end = start + sizeof(char);

			draw_text(x, y, color, start, end);
		}

	} //- textedit

} //- sm