#pragma once
#include "../editor_dock_base.hpp"
#include <tree_sitter.h>

extern "C"
{
	TSLanguage* tree_sitter_cpp();
	TSLanguage* tree_sitter_lua();
}

namespace editor
{
	//- numeric identifier for a node or token
	using token_type_t = TSSymbol;
	//- numeric identifier for a node or token type color
	using token_type_palette_t = unsigned;
	//- character coordinate in the editor grid
	using coordinate_t = TSPoint;
	enum language_type
	{
		language_type_none = 0,
		language_type_lua,
		language_type_cpp,
	};

	//- Window responsible for editing a source text.
	//-------------------------------------------------------------------------------------------------------
	class ctext_editor final : public clayer_base
	{
	public:
		ctext_editor(scontext& ctx);
		~ctext_editor() = default;

		//- general
		void draw();
		void set_language(language_type language);
		void parse(const string_t& source);

		//- visual representation


		//- text editor functionality


		//- utility


	private:
		TSLanguage* m_language = nullptr;
		language_type m_language_type;

		//- Mapping symbol identifier to text name
		umap_t<TSSymbol, const char* > m_language_symbol_names;

		//- text editor part
		std::string m_source;

		coordinate_t m_cursor = { 0, 0 };

		//- visual representation part
		std::string m_title;
		bool m_showing = true;
		ImGuiWindowFlags m_flags = ImGuiWindowFlags_MenuBar;
		ImGuiWindowFlags m_child_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar;

	private:
		void parse();
		void prepare();
		void show_main_menu();
		void show_main_window();

		//- language utitilies
		void retrieve_language_symbols(const TSLanguage* language);
	};

} //- editor