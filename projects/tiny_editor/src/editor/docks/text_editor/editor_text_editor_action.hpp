#pragma once
#include "../../context/editor_context.hpp"

namespace editor
{
	namespace textedit
	{
		//- Interface for text editor actions such as 'add_character', 'erase_character' etc.
		//-------------------------------------------------------------------------------------------------------
		class iaction
		{
		public:
			virtual ~iaction() = default;
			virtual bool execute_do() = 0;
			virtual bool execute_undo() = 0;
		};

		//-------------------------------------------------------------------------------------------------------
		class add_character final : iaction
		{
		public:

			bool execute_do() override final;
			bool execute_undo() override final;
		};

		//-------------------------------------------------------------------------------------------------------
		class erase_character final : iaction
		{
		public:

			bool execute_do() override final;
			bool execute_undo() override final;
		};

	} //- textedit

} //- editor