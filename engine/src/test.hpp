#pragma once
#include <core.h>

namespace engine
{
	class cengine;

	//- This is a dummy test to show how one should be defined. While creating one you must not inherit from it.
	//- Define all functions as shown below and reflect using macro REGISTER_TEST()
	//-
	//- Purpose of a test is to check a specific part of functionality, such as the native filesyste working as expected or
	//- the new fancy allocator allocating and deallocating correct and fast enough.
	//------------------------------------------------------------------------------------------------------------------------
	struct stest final
	{
		enum phase : uint8_t
		{
			phase_none = 0,
			phase_after_configure,	//- execute test after engine and spritemancer were configured (set platform, os, app and renderer class and read args)
			phase_after_init,		//- execute test after platform, os, renderer and app were initialized
			phase_after_run,		//- execute test after application finished running
		};

		static constexpr stringview_t C_TEST_CAN_RUN_FUNC_NAME	= "can_run";
		static constexpr stringview_t C_TEST_PHASE_FUNC_NAME	= "test_phase";
		static constexpr stringview_t C_TEST_RUN_FUNC_NAME		= "run";

		static void run(const core::error_report_function_t& log)	{}
		static bool can_run()										{return false;}
		static phase test_phase()									{return phase_none;}
	};

	//- Class containing array of tests to be executed.
	//------------------------------------------------------------------------------------------------------------------------
	class ctests final
	{
	public:
		STATIC_INSTANCE_EX(ctests);
		ctests() = default;
		~ctests() = default;

		void run(stest::phase phase);
		void set_log(const core::error_report_function_t& log);
		void add_test(stringview_t test);

		unsigned count() const;

	private:
		struct stest_data
		{
			stest_data(stringview_t name, const rttr::method& phase_func, const rttr::method& run_func, const rttr::method& can_run_func);

			string_t m_name;
			rttr::method m_run;
			rttr::method m_phase;
			rttr::method m_can_run;
		};

		umap_t<stest::phase, vector_t<stest_data>> m_tests;
		core::error_report_function_t m_log = nullptr;
	};

} //- engine

//- Use macro to reflect your test, the functions must be declared and implemented
//------------------------------------------------------------------------------------------------------------------------
#define REGISTER_TEST(test) \
{ \
	rttr::cregistrator<test>(STRINGIFY(test)) \
		.meth("run",		&test::run) \
		.meth("can_run",	&test::can_run) \
		.meth("test_phase",	&test::test_phase) \
		; \
}

#define RUN_TEST(test) \
engine::ctests::instance().add_test(STRINGIFY(test));