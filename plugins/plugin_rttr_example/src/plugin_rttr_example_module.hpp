#pragma once
#include "config.hpp"
#include <core.h>
#include <rttr.h>

//------------------------------------------------------------------------------------------------------------------------
struct RTTR_EXAMPLE_API smy_class
{
	void function()
	{
		value += 12;
		data += name.size() * value;
	}

	int value;
	std::string name;
	float data;

	RTTR_ENABLE();
};