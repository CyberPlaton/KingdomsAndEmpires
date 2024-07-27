#include "unittest_allocators.hpp"
#include <unittest.h>

namespace unittest
{
	//------------------------------------------------------------------------------------------------------------------------
	void linear_allocator_test(int object_count)
	{
		struct Object
		{
			std::string name;
			int value;
			float velocity;
			float mass;
		};

		//- setup data
		const auto C_OBJECTS = object_count;
		const auto C_BYTES = C_OBJECTS * sizeof(Object);

		//- timing
		core::ctimer timer;
		core::ctimer allocation_timer;
		core::ctimer access_timer;
		float allocation_time_cumulative = 0.0f;
		float object_access_time_cumulative = 0.0f;

		timer.start();
		core::clinear_allocator allocator(C_BYTES);

		//- perform test with allocation and setting some data
		for (auto i = 0u; i < C_OBJECTS; ++i)
		{
			allocation_timer.start();
			void* memory = allocator.allocate(sizeof(Object), alignof(Object));
			allocation_time_cumulative += allocation_timer.microsecs();

			//- object access
			access_timer.start();
			auto* object = core::iallocator::construct<Object>((Object*)memory);
			object->name = fmt::to_string(i);
			object->value = i;
			object->velocity = i * glm::pi<float>();
			object->mass = object->velocity * glm::pi<float>();
			object_access_time_cumulative += access_timer.microsecs();
		}

		//- stop time
		const auto ms = timer.millisecs();

		log_warn("-----------------------------------------------------------------------------------------");
		log_debug(fmt::format("Object Count: '{}'", C_OBJECTS));
		log_debug(fmt::format("Memory Capacity: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
		log_debug(fmt::format("Memory Used: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
		log_debug(fmt::format("Memory Peak Used: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
		log_info(fmt::format("Total Time: '{}ms'", ms));
		log_info(fmt::format("Allocation Time: '{}us' (Total), '{}us' (Per Object)",
			allocation_time_cumulative, allocation_time_cumulative / C_OBJECTS));
		log_info(fmt::format("Access Time: '{}us' (Total), '{}us' (Per Object)",
			object_access_time_cumulative, object_access_time_cumulative / C_OBJECTS));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void stack_allocator_test(int object_count)
	{
		struct Object
		{
			std::string name;
			int value;
			float velocity;
			float mass;
		};

		//- setup data
		const auto C_OBJECTS = object_count;
		const auto C_BYTES = C_OBJECTS * sizeof(Object);

		//- timing
		core::ctimer timer;
		core::ctimer allocation_timer;
		core::ctimer deallocation_timer;
		core::ctimer access_timer;
		float allocation_time_cumulative = 0.0f;
		float deallocation_time_cumulative = 0.0f;
		float object_access_time_cumulative = 0.0f;

		timer.start();
		core::cstack_allocator allocator(C_BYTES);

		//- perform test with allocation and setting some data
		for (auto i = 0u; i < C_OBJECTS; ++i)
		{
			//- allocation
			allocation_timer.start();
			void* memory = allocator.allocate(sizeof(Object), alignof(Object));
			allocation_time_cumulative += allocation_timer.microsecs();

			//- object access
			access_timer.start();
			auto* object = core::iallocator::construct<Object>((Object*)memory);
			object->name = fmt::to_string(i);
			object->value = i;
			object->velocity = i * glm::pi<float>();
			object->mass = object->velocity * glm::pi<float>();
			object_access_time_cumulative += access_timer.microsecs();

			//- deallocation
			deallocation_timer.start();
			allocator.deallocate(memory);
			deallocation_time_cumulative += deallocation_timer.microsecs();
		}

		//- stop time
		const auto ms = timer.millisecs();

		log_warn("-----------------------------------------------------------------------------------------");
		log_debug(fmt::format("Object Count: '{}'", C_OBJECTS));
		log_debug(fmt::format("Memory Capacity: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
		log_debug(fmt::format("Memory Used: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
		log_debug(fmt::format("Memory Peak Used: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
		log_info(fmt::format("Total Time: '{}ms'", ms));
		log_info(fmt::format("Allocation Time: '{}us' (Total), '{}us' (Per Object)",
			allocation_time_cumulative, allocation_time_cumulative / C_OBJECTS));
		log_info(fmt::format("Deallocation Time: '{}us' (Total), '{}us' (Per Object)",
			deallocation_time_cumulative, deallocation_time_cumulative / C_OBJECTS));
		log_info(fmt::format("Access Time: '{}us' (Total), '{}us' (Per Object)",
			object_access_time_cumulative, object_access_time_cumulative / C_OBJECTS));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void pool_allocator_test(int object_count)
	{
		struct Object
		{
			std::string name;
			int value;
			float velocity;
			float mass;
		};

		//- setup data
		const auto C_OBJECTS = object_count;

		//- timing
		core::ctimer timer;
		core::ctimer allocation_timer;
		core::ctimer deallocation_timer;
		core::ctimer access_timer;
		float allocation_time_cumulative = 0.0f;
		float deallocation_time_cumulative = 0.0f;
		float object_access_time_cumulative = 0.0f;

		timer.start();
		core::cpool_allocator<Object> allocator(C_OBJECTS);

		//- perform test with allocation and setting some data
		for (auto i = 0u; i < C_OBJECTS; ++i)
		{
			//- allocation
			allocation_timer.start();
			void* memory = allocator.allocate(0);
			allocation_time_cumulative += allocation_timer.microsecs();

			//- object access
			access_timer.start();
			auto* object = core::iallocator::construct<Object>((Object*)memory);
			object->name = fmt::to_string(i);
			object->value = i;
			object->velocity = i * glm::pi<float>();
			object->mass = object->velocity * glm::pi<float>();
			object_access_time_cumulative += access_timer.microsecs();

			//- deallocation
			deallocation_timer.start();
			allocator.deallocate(memory);
			deallocation_time_cumulative += deallocation_timer.microsecs();
		}

		//- stop time
		const auto ms = timer.millisecs();

		log_warn("-----------------------------------------------------------------------------------------");
		log_debug(fmt::format("Object Count: '{}'", C_OBJECTS));
		log_debug(fmt::format("Memory Capacity: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
		log_debug(fmt::format("Memory Used: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
		log_debug(fmt::format("Memory Peak Used: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
		log_info(fmt::format("Total Time: '{}ms'", ms));
		log_info(fmt::format("Allocation Time: '{}us' (Total), '{}us' (Per Object)",
			allocation_time_cumulative, allocation_time_cumulative / C_OBJECTS));
		log_info(fmt::format("Deallocation Time: '{}us' (Total), '{}us' (Per Object)",
			deallocation_time_cumulative, deallocation_time_cumulative / C_OBJECTS));
		log_info(fmt::format("Access Time: '{}us' (Total), '{}us' (Per Object)",
			object_access_time_cumulative, object_access_time_cumulative / C_OBJECTS));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void std_allocator_test_run(int object_count)
	{
		struct Object
		{
			std::string name;
			int value;
			float velocity;
			float mass;
		};

		//- setup data
		const auto C_OBJECTS = object_count;

		//- timing
		core::ctimer timer;
		core::ctimer allocation_timer;
		core::ctimer deallocation_timer;
		core::ctimer access_timer;
		float allocation_time_cumulative = 0.0f;
		float deallocation_time_cumulative = 0.0f;
		float object_access_time_cumulative = 0.0f;

		timer.start();
		std::allocator<Object> allocator;

		//- perform test with allocation and setting some data
		for (auto i = 0u; i < C_OBJECTS; ++i)
		{
			//- allocation
			allocation_timer.start();
			void* memory = allocator.allocate(1);
			allocation_time_cumulative += allocation_timer.microsecs();

			//- object access
			access_timer.start();
			auto* object = core::iallocator::construct<Object>((Object*)memory);
			object->name = fmt::to_string(i);
			object->value = i;
			object->velocity = i * glm::pi<float>();
			object->mass = object->velocity * glm::pi<float>();
			object_access_time_cumulative += access_timer.microsecs();

			//- deallocation
			deallocation_timer.start();
			allocator.deallocate((Object*)memory, 1);
			deallocation_time_cumulative += deallocation_timer.microsecs();
		}

		//- stop time
		const auto ms = timer.millisecs();

		log_warn("-----------------------------------------------------------------------------------------");
		log_debug(fmt::format("Object Count: '{}'", C_OBJECTS));
		log_debug("Memory Capacity: '-/-'");
		log_debug("Memory Used: '-/-'");
		log_debug("Memory Peak Used: '-/-'");
		log_info(fmt::format("Total Time: '{}ms'", ms));
		log_info(fmt::format("Allocation Time: '{}us' (Total), '{}us' (Per Object)",
			allocation_time_cumulative, allocation_time_cumulative / C_OBJECTS));
		log_info(fmt::format("Deallocation Time: '{}us' (Total), '{}us' (Per Object)",
			deallocation_time_cumulative, deallocation_time_cumulative / C_OBJECTS));
		log_info(fmt::format("Access Time: '{}us' (Total), '{}us' (Per Object)",
			object_access_time_cumulative, object_access_time_cumulative / C_OBJECTS));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void allocators_test_runs()
	{
		auto OBJECTS = 0;
		const auto C_RUNS = 8;
		const auto C_INCREMENT = 2;

		log_warn("\n#### Linear Allocator Testing Run ##########################################");
		OBJECTS = 2048;
		for (auto i = 0; i < C_RUNS; ++i)
		{
			linear_allocator_test(OBJECTS);
			OBJECTS *= C_INCREMENT;
		}

		log_warn("\n#### Stack Allocator Testing Run ###########################################");
		OBJECTS = 2048;
		for (auto i = 0; i < C_RUNS; ++i)
		{
			stack_allocator_test(OBJECTS);

			OBJECTS *= C_INCREMENT;
		}

		log_warn("\n#### Pool Allocator Testing Run ############################################");
		OBJECTS = 2048;
		for (auto i = 0; i < C_RUNS; ++i)
		{
			pool_allocator_test(OBJECTS);

			OBJECTS *= C_INCREMENT;
		}

		log_warn("\n#### STD Allocator Testing Run ############################################");
		OBJECTS = 2048;
		for (auto i = 0; i < C_RUNS; ++i)
		{
			std_allocator_test_run(OBJECTS);

			OBJECTS *= C_INCREMENT;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	SUITE(Allocators)
	{
		TEST(allocator_tests)
		{
			allocators_test_runs();

			CHECK(true);
		}
	}

} //- unittest