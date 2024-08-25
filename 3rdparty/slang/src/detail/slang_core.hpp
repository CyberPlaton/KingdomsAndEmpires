#pragma once
#include "slang_config.hpp"

namespace slang
{
	class cany;
	template<typename>
	class cfunction;

	//------------------------------------------------------------------------------------------------------------------------
	struct stoken
	{
		string_t m_text;
		unsigned m_line = 0;
		token_type m_type = token_type_null;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct stoken_stream
	{
		vector_t<stoken> m_tokens;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cnon_copyable
	{
	public:
		cnon_copyable() = default;
		virtual ~cnon_copyable() = default;
	private:
		cnon_copyable& operator=(const cnon_copyable&) = delete;
		cnon_copyable(const cnon_copyable&) = delete;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cnon_movable
	{
	public:
		cnon_movable() = default;
		virtual ~cnon_movable() = default;
	private:
		cnon_movable(cnon_movable&&) = delete;
		cnon_movable& operator=(const cnon_movable&&) = delete;
	};

	//- Class holding any type of object that can switch an runtime.
	//- Note: data type the class is holding must be copy-constructible
	//------------------------------------------------------------------------------------------------------------------------
	class cany final
	{
	public:
		cany() = default;
		cany(rttr::variant var) : m_data(var) {}
		template<typename T>
		cany(T var) : m_data(rttr::variant(var)) {}
		cany& operator=(const cany& other);
		~cany();

		inline rttr::type::type_id type() const { return m_data.get_type().get_id(); }
		inline stringview_t type_name() const { return m_data.get_type().get_name().data(); }
		inline bool empty() const { return !m_data.is_valid(); }
		inline const rttr::variant& data() const { return m_data; }

		template<typename TType>
		inline bool is() const { return rttr::type::get<TType>().get_id() == type(); }

		template<typename TType>
		TType copy();

		template<typename TType>
		TType& get();

		template<typename TType>
		const TType& get() const;

	private:
		rttr::variant m_data;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	TType cany::copy()
	{
		SLANG_ASSERT(is<TType>(), "Invalid operation. Typecast to another data type is not allowed!");

		return m_data.get_value<TType>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	TType& cany::get()
	{
		SLANG_ASSERT(is<TType>(), "Invalid operation. Typecast to another data type is not allowed!");

		return m_data.get_value<TType>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	const TType& cany::get() const
	{
		SLANG_ASSERT(is<TType>(), "Invalid operation. Typecast to another data type is not allowed!");

		return m_data.get_value<TType>();
	}

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

		inline TReturn operator()(ARGS... args) const { SLANG_ASSERT(verify(), "Invalid operation. Function was not set!"); return m_invoker(m_function, std::forward<ARGS>(args)...); }

		explicit operator bool() const noexcept { return verify(); }

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
	bool cfunction<TReturn(ARGS...)>::verify() const
	{
		return m_invoker && m_function;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TReturn, typename... ARGS>
	template<typename T>
	cfunction<TReturn(ARGS...)>::cfunction(T&& function) :
		m_invoker(&invoke<typename std::decay<T>::type>), m_function(&function)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TReturn, typename... ARGS>
	cfunction<TReturn(ARGS...)>::cfunction(invoker_ref_t function) :
		m_invoker(invoke<invoker_ref_t>), m_function((void*)function)
	{
		static_assert(sizeof(void*) == sizeof function);
	}

	using slang_logger_t = cfunction<void(log_level, const char*)>;

	//------------------------------------------------------------------------------------------------------------------------
	struct slogger
	{
		STATIC_INSTANCE(slogger);

		slang_logger_t m_callback = nullptr;
		log_level m_level = log_level_none;
	};

} //- slang