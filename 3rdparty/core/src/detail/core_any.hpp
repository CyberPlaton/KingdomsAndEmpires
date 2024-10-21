#pragma once
#include "core_config.hpp"

namespace core
{
	//- Note: data type the class is holding must be copy-constructible
	//------------------------------------------------------------------------------------------------------------------------
	class cany final
	{
	public:
		cany() = default;
		cany(rttr::variant var) : m_data(std::move(var)) { }
		cany& operator=(const cany& other);
		~cany();

		inline rttr::type::type_id type() const { return m_data.get_type().get_id(); }
		inline stringview_t type_name() const { return m_data.get_type().get_name().data(); }
		inline bool empty() const { return !m_data.is_valid(); }
		inline const rttr::variant& data() const { return m_data; }

		template<typename TType>
		inline bool is() const
		{
			return rttr::type::get<TType>().get_id() == type();
		}

		template<typename TType>
		TType copy()
		{
			ASSERT(is<TType>(), "Invalid operation. Typecast to another data type is not allowed!");

			return m_data.get_value<TType>();
		}

		template<typename TType>
		TType& get()
		{
			ASSERT(is<TType>(), "Invalid operation. Typecast to another data type is not allowed!");

			return m_data.get_value<TType>();
		}

		template<typename TType>
		const TType& get() const
		{
			ASSERT(is<TType>(), "Invalid operation. Typecast to another data type is not allowed!");

			return m_data.get_value<TType>();
		}

	private:
		rttr::variant m_data;
	};

} //- core