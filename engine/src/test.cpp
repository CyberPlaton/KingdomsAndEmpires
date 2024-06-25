#include "test.hpp"
#include <plugin_logging.h>

namespace engine
{
	//- TODO: we could consider executing the test async or diluted between frames, i.e. one test each frame tick or something,
	//- this could be useful for triggering them from a kind of editor and displaying results to a console, but when executing
	//- everyone at once this is enough.
	//------------------------------------------------------------------------------------------------------------------------
	void ctests::run(stest::phase phase)
	{
		for (const auto& test : m_tests[phase])
		{
			if (test.m_can_run.invoke({}).to_bool())
			{
				test.m_run.invoke({}, m_log);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctests::set_log(const core::error_report_function_t& log)
	{
		m_log = log;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctests::add_test(stringview_t test)
	{
		if (auto type = rttr::type::get_by_name(test.data()); type.is_valid())
		{
			auto run_func = type.get_method(stest::C_TEST_RUN_FUNC_NAME.data());
			auto phase_func = type.get_method(stest::C_TEST_PHASE_FUNC_NAME.data());
			auto can_run_func = type.get_method(stest::C_TEST_CAN_RUN_FUNC_NAME.data());

			if (!(run_func.is_valid() && can_run_func.is_valid() && phase_func.is_valid()))
			{
				logging::log_error(fmt::format("Failed adding test '{}', it has an undefined function!", test.data()));
				return;
			}
			if (!(run_func.is_static() && can_run_func.is_static() && phase_func.is_static()))
			{
				logging::log_error(fmt::format("Failed adding test '{}', it has a function not declared 'static'!", test.data()));
				return;
			}

			//- Get phase automatically from static class function
			const auto p = phase_func.invoke({}).convert<stest::phase>();

			m_tests[p].emplace_back(test, phase_func, run_func, can_run_func);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned ctests::count() const
	{
		return SCAST(unsigned, m_tests.size());
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctests::stest_data::stest_data(stringview_t name, const rttr::method& phase_func, const rttr::method& run_func, const rttr::method& can_run_func) :
		m_name(name.data()), m_phase(phase_func), m_run(run_func), m_can_run(can_run_func)
	{
	}

} //- engine