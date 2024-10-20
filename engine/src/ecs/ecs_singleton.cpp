#include "ecs_singleton.hpp"

namespace ecs
{

} //- ecs

RTTR_REGISTRATION
{
	using namespace ecs;

	rttr::registration::class_<sexample_singleton>("sexample_singleton")
		.property("m_data", &sexample_singleton::m_data)
		.method("serialize", &sexample_singleton::serialize)
		.method("add", &sexample_singleton::add)
		;
	
	rttr::default_constructor<sexample_singleton>();
	rttr::default_constructor<umap_t<std::string, bool>>();
}