#include "plugin_camera_system_module.hpp"

RTTR_PLUGIN_REGISTRATION
{
	using namespace rttr;
	using namespace camera_system;

	engine::cservice_manager::emplace<ccamera_manager>();
};

namespace camera_system
{

} //- camera_system