#pragma once
#include "core_config.hpp"

namespace core
{
	template<typename>
	class cfunction;

	//- A light-weight alternative to std::function<> with the same capabilities inspired by
	//- https://gist.github.com/twoscomplement/030818a6c38c5a983482dc3a385a3ab8
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TReturn, typename... ARGS>
	class cfunction<TReturn(ARGS...)> final
	{
	private:
		using invoker_t = TReturn(*)(void*, ARGS...);
		using invoker_ref_t = TReturn(ARGS...);

	public:
		template<typename T>
		cfunction(T&& function);
		cfunction(invoker_ref_t function);
		cfunction() : m_invoker(nullptr), m_function(nullptr) {}
		~cfunction() = default;

		inline TReturn operator()(ARGS... args) const { CORE_ASSERT(verify(), "Invalid operation. Function was not set!"); return m_invoker(m_function, std::forward<ARGS>(args)...); }

		explicit operator bool() const noexcept { return verify(); }

		cfunction(cfunction&& other) noexcept :
			m_invoker(other.m_invoker), m_function(other.m_function)
		{
			other.m_invoker = nullptr;
			other.m_function = nullptr;
		}

		cfunction& operator=(cfunction&& other) noexcept
		{
			m_invoker = other.m_invoker;
			m_function = other.m_function;
			other.m_invoker = nullptr;
			other.m_function = nullptr;
			return *this;
		}

		cfunction(const cfunction& other) noexcept :
			m_invoker(other.m_invoker), m_function(other.m_function)
		{
		}

		cfunction& operator=(const cfunction& other) noexcept
		{
			m_invoker = other.m_invoker;
			m_function = other.m_function;
			return *this;
		}

	private:
		invoker_t m_invoker = nullptr;
		void* m_function = nullptr;

	private:
		template<typename T>
		static TReturn invoke(void* function, ARGS... args) { return (*(T*)function)(std::forward<ARGS>(args)...); }
		bool verify() const;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TReturn, typename... ARGS>
	bool core::cfunction<TReturn(ARGS...)>::verify() const
	{
		return m_invoker && m_function;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TReturn, typename... ARGS>
	template<typename T>
	core::cfunction<TReturn(ARGS...)>::cfunction(T&& function) :
		m_invoker(&invoke<typename std::decay<T>::type>), m_function(&function)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TReturn, typename... ARGS>
	core::cfunction<TReturn(ARGS...)>::cfunction(invoker_ref_t function) :
		m_invoker(invoke<invoker_ref_t>), m_function((void*)function)
	{
		static_assert(sizeof(void*) == sizeof function);
	}

} //- core