#pragma once
#include "../include/config.hpp"
#include <core.h>
#include <rttr.h>

//------------------------------------------------------------------------------------------------------------------------
struct smy_class
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