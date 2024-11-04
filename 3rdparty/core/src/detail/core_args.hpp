#pragma once
#include "core_config.hpp"
#include <argparse.h>

namespace core
{
	//- Simple wrapper aroung argparse. Allows configuring expected arguments, parses them and answers queries.
	//- Furthermore, argparse expections are wrapped and removed.
	//- Note: left not final, because shared_ptr construction has to extend the class by one level. This is due to the
	//- constructor being protected.
	//------------------------------------------------------------------------------------------------------------------------
	class cargs :
		private cnon_copyable,
		private cnon_movable
	{
	public:
		static args_ref_t make_ref(stringview_t name = "", stringview_t version = "");

		~cargs() = default;

		bool parse(int argc, char* argv[]);

		//- Add an optional argument that can only be true or false
		cargs& flag(stringview_t name, stringview_t short_name, const bool default_value = false);

		template<typename TType>
		cargs& arg(stringview_t name, stringview_t short_name, TType default_value);

		//- Check whether argument has the value
		template<typename TType>
		bool check(stringview_t name, TType value) const;

		//- Get the value for argument and store it to provided reference
		template<typename TType>
		cargs& get(stringview_t name, TType& value);

		template<typename TType>
		TType get(stringview_t name);

		bool used(stringview_t name) const;

	private:
		argparse::ArgumentParser m_parser;

	protected:
		cargs(stringview_t name = "", stringview_t version = "");
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	bool cargs::check(stringview_t name, TType value) const
	{
		auto result = false;

		try
		{
			result = m_parser.get<TType>(name.data()) == value;
		}
		catch (...)
		{
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	cargs& cargs::arg(stringview_t name, stringview_t short_name, TType default_value)
	{
		m_parser
			.add_argument(short_name.data(), name.data())
			.default_value(default_value);

		return *this;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	cargs& cargs::get(stringview_t name, TType& value)
	{
		try
		{
			m_parser.get(name.data()).store_into(value);
		}
		catch (...)
		{
		}

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	TType cargs::get(stringview_t name)
	{
		TType value;

		get_argument(name, value);

		return value;
	}

} //- core