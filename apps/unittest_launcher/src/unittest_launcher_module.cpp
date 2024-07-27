#include "unittest_launcher_module.hpp"
#include "tests/core/unittest_core.hpp"
#include "tests/spritemancer/unittest_spritemancer.hpp"
#include "tests/engine/unittest_engine.hpp"
#include "tests/plugin_ai/unittest_plugin_ai.hpp"
#include "tests/plugin_camera_system/unittest_plugin_camera_system.hpp"
#include "tests/plugin_kingdoms_and_empires/unittest_plugin_kingdoms_and_empires.hpp"
#include "tests/plugin_kingdoms_core/unittest_plugin_kingdoms_core.hpp"
#include "tests/plugin_race_human/unittest_plugin_race_human.hpp"
#include "tests/plugin_render_system/unittest_plugin_render_system.hpp"
#include "tests/plugin_status_effects/unittest_plugin_status_effects.hpp"
#include "tests/plugin_tiny_editor/unittest_plugin_tiny_editor.hpp"
#include <iostream>

//------------------------------------------------------------------------------------------------------------------------
void core_io_error_function(uint8_t level, const std::string& message)
{
	switch (level)
	{
	case SPDLOG_LEVEL_TRACE:
	{
		log_trace(message);
		break;
	}
	case SPDLOG_LEVEL_DEBUG:
	{
		log_debug(message);
		break;
	}
	case SPDLOG_LEVEL_INFO:
	{
		log_info(message);
		break;
	}
	case SPDLOG_LEVEL_WARN:
	{
		log_warn(message);
		break;
	}
	case SPDLOG_LEVEL_ERROR:
	{
		log_error(message);
		break;
	}
	case SPDLOG_LEVEL_CRITICAL:
	{
		log_critical(message);
		break;
	}
	}
}

namespace UnitTest
{
	//------------------------------------------------------------------------------------------------------------------------
	class UNITTEST_LINKAGE CoreReporter : public TestReporter
	{
	private:
		virtual void ReportTestStart(TestDetails const& test)
		{
			log_debug(fmt::format("Beginning test '{}::{}' from '{}::{}'",
				test.suiteName, test.testName, test.filename, test.lineNumber));
		}

		virtual void ReportFailure(TestDetails const& test, char const* failure)
		{
			log_error(fmt::format("\t...failed with message: '{}'", failure));
		}

		virtual void ReportTestFinish(TestDetails const& test, float secondsElapsed)
		{
			log_info(fmt::format("\t...successfully finished in '{}'secs", secondsElapsed));
		}

		virtual void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed)
		{
			if (failedTestCount > 0)
			{
				log_error(fmt::format("Finished test run in '{}'secs: Total '{}', Success '{}', Failed '{}'",
					secondsElapsed, totalTestCount, (totalTestCount - failedTestCount), failedTestCount));
			}
			else
			{
				log_info(fmt::format("Finished test run in '{}'secs: Total '{}', Success '{}', Failed '{}'",
					secondsElapsed, totalTestCount, (totalTestCount - failedTestCount), failedTestCount));
			}
		}
	};

} //- UnitTest

//- Unittest launcher is a console application. So dont bother with winmain.
//- Note: Tests are structured as follows, in 'tests' folder are subfolders for all plugins, engine, core etc. and inside
//- those are thematically named tests and suites that are included into this source and executed when ran.
//------------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	logging::clog::instance().init(core::logging_verbosity::logging_verbosity_trace);

	UnitTest::CoreReporter reporter;
	UnitTest::TestRunner runner(reporter);

	return runner.RunTestsIf(UnitTest::Test::GetTestList(), NULL, UnitTest::True(), 0);
}