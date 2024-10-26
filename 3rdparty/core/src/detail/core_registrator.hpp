#pragma once
#include "core_config.hpp"

namespace rttr
{
	namespace detail
	{
		template<class T, typename = void> struct skey_type { using type = void; };
		template<class T, typename = void> struct svalue_type { using type = void; };
		template<class T> struct skey_type<T, std::void_t<typename T::key_type>> { using type = typename T::key_type; };
		template<class T> struct svalue_type<T, std::void_t<typename T::value_type>> { using type = typename T::value_type; };
		template<class T, typename = void> struct sis_iterator : std::false_type { };
		template<class T> struct sis_iterator<T, std::void_t<typename T::iterator>> : std::true_type { };
		template<class T> constexpr bool sis_container = sis_iterator<T>::value && !std::is_same_v<T, string_t>;
		struct no_default {};

		enum constructor_policy : uint8_t
		{
			constructor_policy_none = 0,
			constructor_policy_as_object,
			constructor_policy_as_shared_ptr,
			constructor_policy_as_raw_ptr,
			constructor_policy_no_default,
		};

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TClass>
		static rttr::type type_of()
		{
			return rttr::type::get<TClass>();
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TClass, typename TKey, typename TValue>
		static TValue get_meta(rttr::string_view prop, TKey key)
		{
			auto type = rttr::type::get<TClass>();

			auto property = type.get_global_property(prop);

			auto value = property.get_metadata(key);

			return value.template get_value<TValue>();
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TClass, typename TValue>
		static TValue get_meta(rttr::string_view prop, rttr::string_view key)
		{
			return get_meta<TClass, rttr::string_view, TValue>(prop, key);
		}

		//------------------------------------------------------------------------------------------------------------------------
		static rttr::method get_method(const rttr::type& type, stringview_t name)
		{
			return type.get_method(name.data());
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TClass>
		static rttr::method get_method(stringview_t name)
		{
			return get_method(rttr::type::get<TClass>(), name);
		}

		//- Register a default constructor for a class. Can be container types, such as vector_t<> or normal classes.
	//------------------------------------------------------------------------------------------------------------------------
		template<typename TType>
		static void default_constructor()
		{
			//- i.e. vector_t or array_t (c-style arrays are not supported)
			if constexpr (!std::is_array_v<TType>)
			{
				if (const auto type = rttr::type::get<TType>(); type.is_valid() && type.get_constructors().empty())
				{
					typename rttr::registration::template class_<TType> __class(type.get_name());
					__class.constructor()(rttr::policy::ctor::as_object);
				}
			}
			//- recursive register default constructor for i.e. map_t or nested containers, i.e. vector_t< map_t<>> etc.
			if constexpr (detail::sis_container<TType>)
			{
				using key_t = typename detail::skey_type<TType>::type;
				using value_t = typename detail::svalue_type<TType>::type;

				if constexpr (detail::sis_container<key_t>)
				{
					default_constructor<key_t>();
				}
				if constexpr (detail::sis_container<value_t>)
				{
					default_constructor<value_t>();
				}
			}
		}

	} //- detail

	//- Utility class for RTTR registration. Adds a default constructor.
	//- Intended for classes. Use the class_() function to register metas etc.
	//- We do not check for duplicate registrations as those might be a side-effect of REFLECT_INLINE() usage,
	//- just be aware that the latest registration can override a previous one with same type name.
	//-
	//- TPolicy can be one of
	//-		- rttr::detail::as_object
	//-		- rttr::detail::as_std_shared_ptr
	//-		- rttr::detail::as_raw_pointer
	//-		- rttr::detail::no_default (omits registering a default constructor, useful when you explicitly don\B4t want one)
	//------------------------------------------------------------------------------------------------------------------------
	template<class TClass, typename TPolicy = rttr::detail::as_object>
	class cregistrator
	{
	public:
		cregistrator(rttr::string_view name);

		template<typename TMethod, typename... TMeta>
		cregistrator& meth(rttr::string_view name, TMethod method, TMeta&&... metadata);

		template<typename TProp, typename... TMeta>
		cregistrator& prop(rttr::string_view name, TProp property, TMeta&&... metadata);

		template<typename TKey, typename TValue>
		cregistrator& meta(TKey key, TValue value);

		template<typename TValue>
		cregistrator& meta(rttr::string_view name, TValue value);

		template<typename... ARGS>
		cregistrator& ctor(detail::constructor_policy policy = detail::constructor_policy::constructor_policy_as_object)
		{
			auto& c = m_object.template constructor<ARGS...>();

			switch (policy)
			{
			default:
			case detail::constructor_policy_none:
			case detail::constructor_policy_as_object:
			{
				CORE_ASSERT(std::is_copy_constructible_v<TClass>, "Invalid operation. Class must be copy-constructible when registered as with 'as_object' policy");

				c(rttr::policy::ctor::as_object);
				break;
			}
			case detail::constructor_policy_as_shared_ptr:
			{
				c(rttr::policy::ctor::as_std_shared_ptr);
				break;
			}
			case detail::constructor_policy_as_raw_ptr:
			{
				c(rttr::policy::ctor::as_raw_ptr);
				break;
			}
			}
			return *this;
		}

	private:
		rttr::registration::class_<TClass> m_object;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<class TClass, typename TPolicy /*= rttr::detail::as_object*/>
	template<typename TValue>
	cregistrator<TClass, TPolicy>& rttr::cregistrator<TClass, TPolicy>::meta(rttr::string_view name, TValue value)
	{
		return meta<rttr::string_view, TValue>(name, value);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TClass, typename TPolicy /*= rttr::detail::as_object*/>
	template<typename TKey, typename TValue>
	cregistrator<TClass, TPolicy>& rttr::cregistrator<TClass, TPolicy>::meta(TKey key, TValue value)
	{
		this->m_object
		(
			rttr::metadata(key, value)
		);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TClass, typename TPolicy /*= rttr::detail::as_object*/>
	rttr::cregistrator<TClass, TPolicy>::cregistrator(rttr::string_view name) :
		m_object(name)
	{
		if constexpr (!std::is_same_v<rttr::detail::no_default, TPolicy>)
		{
			//- class should be registered with RTTR by this point
			if constexpr (std::is_same_v<rttr::detail::as_object, TPolicy>)
			{
				static_assert(std::is_copy_constructible_v<TClass>, "Invalid operation. Class must be copy-constructible when registered as with 'as_object' policy");

				m_object.constructor()(rttr::policy::ctor::as_object);
			}
			else if constexpr (std::is_same_v<rttr::detail::as_raw_pointer, TPolicy>)
			{
				m_object.constructor()(rttr::policy::ctor::as_raw_ptr);
			}
			else if constexpr (std::is_same_v<rttr::detail::as_std_shared_ptr, TPolicy>)
			{
				m_object.constructor()(rttr::policy::ctor::as_std_shared_ptr);
			}
			else
			{
				CORE_ASSERT(false, "Invalid operation. TPolicy must be one of 'rttr::detail::as_object', 'rttr::detail::as_std_shared_ptr' or 'rttr::detail::as_raw_pointer'");
			}

			if (core::serror_reporter::instance().m_callback)
			{
				core::serror_reporter::instance().m_callback(core::logging_verbosity_error,
					fmt::format("Registering RTTR class '{}' with policy '{}'",
						rttr::type::get<TClass>().get_name().data(), rttr::type::get<TPolicy>().get_name().data()));
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TClass, typename TPolicy /*= rttr::detail::as_object*/>
	template<typename TProp, typename... TMeta>
	cregistrator<TClass, TPolicy>& rttr::cregistrator<TClass, TPolicy>::prop(rttr::string_view name, TProp property, TMeta&&... metadata)
	{
		if constexpr (sizeof... (TMeta) > 0)
		{
			m_object.property(name, std::move(property))
				(
					std::forward<TMeta>(metadata)...
					);
		}
		else
		{
			m_object.property(name, std::move(property));
		}

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TClass, typename TPolicy /*= rttr::detail::as_object*/>
	template<typename TMethod, typename... TMeta>
	cregistrator<TClass, TPolicy>& rttr::cregistrator<TClass, TPolicy>::meth(rttr::string_view name, TMethod method, TMeta&&... metadata)
	{
		if constexpr (sizeof... (TMeta) > 0)
		{
			m_object.method(name, std::move(method))
				(
					std::forward<TMeta>(metadata)...
					);
		}
		else
		{
			m_object.method(name, std::move(method));
		}

		return *this;
	}

} //- rttr