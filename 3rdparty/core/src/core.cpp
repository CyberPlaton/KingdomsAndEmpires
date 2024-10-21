#include "core.hpp"
#include "detail/core_general_allocator.hpp"
#include "detail/core_stringutils.hpp"
#include "detail/core_service_manager.hpp"
#include "detail/core_filesystem.hpp"
#include "detail/core_event_service.hpp"
#include "detail/core_registrator.hpp"
#include "detail/core_uuid.hpp"
#include "detail/core_rect.hpp"
#include "detail/core_color.hpp"
#include <sstream>

namespace core
{
	namespace
	{
		static cgeneral_allocator S_GENERAL_ALLOCATOR;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void sentry::init()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void sentry::shutdown()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::iallocator* sentry::get_allocator()
	{
		return &S_GENERAL_ALLOCATOR;
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t sinfo::platform()
	{
#if CORE_PLATFORM_WINDOWS
		return "Windows";
#elif CORE_PLATFORM_XBOXONE
		return "Xbox One";
#elif CORE_PLATFORM_XBOXSERIES
		return "Xbox Series";
#elif CORE_PLATFORM_LINUX
		return "Linux";
#elif CORE_PLATFORM_ANDROID
		return "Android";
#elif CORE_PLATFORM_IOS
		return "iOS";
#elif CORE_PLATFORM_OSX
		return "macOS";
#elif CORE_PLATFORM_PS4
		return "PlayStation 4";
#elif CORE_PLATFORM_PS5
		return "PlayStation 5";
#elif CORE_PLATFORM_NX
		return "NX";
#endif
		return "<unknown>";
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t sinfo::architecture()
	{
#if CORE_ARCH_64BIT
		return "x64";
#elif CORE_ARCH_32BIT
		return "x32";
#endif
		return "<unknown>";
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t sinfo::compiler()
	{
#if CORE_COMPILER_MSVC
		return "msvc";
#elif CORE_COMPILER_GCC
		return "gcc";
#elif CORE_COMPILER_CLANG
		return "clang";
#endif
		return "<unknown>";
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t sinfo::configuration()
	{
#if DEBUG
		return "Debug";
#elif RELEASE
		return "Release";
#elif HYBRID
		return "Hybrid";
#endif
		return "<unknown>";
	}

} //- core

RTTR_REGISTRATION
{
	using namespace core;
	using namespace fs;

	rttr::registration::class_<cservice>("cservice")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
			)
		.method("on_start", &cservice::on_start)
		.method("on_shutdown", &cservice::on_shutdown)
		.method("on_update", &cservice::on_update)
		;

	rttr::registration::class_<cvirtual_filesystem>("cvirtual_filesystem")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
			)
		;

	rttr::registration::class_<cevent_service>("cevent_service")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
			)
		;

	rttr::registration::class_<cservice_manager::sconfig>("cservice_manager::sconfig")
		.property("m_services", &cservice_manager::sconfig::m_services)
		;

	rttr::detail::default_constructor<cservice_manager::sconfig>();
	rttr::detail::default_constructor<vector_t<string_t>>();

	rttr::registration::class_<cuuid>("cuuid")
		.constructor<>()
		(
			rttr::policy::ctor::as_object
			)
		.property("m_data", &cuuid::m_data)
		;

	rttr::detail::default_constructor<array_t<unsigned char, 16u>>();

	rttr::registration::class_<scolor>("scolor")
		.constructor<>()
		(
			rttr::policy::ctor::as_object
			)
		.property("m_r", &scolor::m_r)
		.property("m_g", &scolor::m_g)
		.property("m_b", &scolor::m_b)
		.property("m_a", &scolor::m_a)
		;

	rttr::registration::class_<srect>("srect")
		.constructor<>()
		(
			rttr::policy::ctor::as_object
			)
		.property("m_x", &srect::m_x)
		.property("m_y", &srect::m_y)
		.property("m_w", &srect::m_w)
		.property("m_h", &srect::m_h)
		;

	rttr::registration::class_<vec2_t>("vec2_t")
		.constructor<>()
		(
			rttr::policy::ctor::as_object
			)
		.property("x", &vec2_t::x)
		.property("y", &vec2_t::y)
		;

	rttr::registration::class_<vec3_t>("vec3_t")
		.constructor<>()
		(
			rttr::policy::ctor::as_object
			)
		.property("x", &vec3_t::x)
		.property("y", &vec3_t::y)
		.property("z", &vec3_t::z)
		;

	rttr::registration::class_<vec4_t>("vec4_t")
		.constructor<>()
		(
			rttr::policy::ctor::as_object
			)
		.property("x", &vec4_t::x)
		.property("y", &vec4_t::y)
		.property("z", &vec4_t::z)
		.property("w", &vec4_t::w)
		;
}
