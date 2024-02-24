#pragma once
#include <core.h>

#include "box2d/b2_settings.h"
#include "box2d/b2_draw.h"
#include "box2d/b2_timer.h"
#include "box2d/b2_chain_shape.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_edge_shape.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_broad_phase.h"
#include "box2d/b2_dynamic_tree.h"
#include "box2d/b2_body.h"
#include "box2d/b2_contact.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_time_step.h"
#include "box2d/b2_world.h"
#include "box2d/b2_world_callbacks.h"
#include "box2d/b2_distance_joint.h"
#include "box2d/b2_friction_joint.h"
#include "box2d/b2_gear_joint.h"
#include "box2d/b2_motor_joint.h"
#include "box2d/b2_mouse_joint.h"
#include "box2d/b2_prismatic_joint.h"
#include "box2d/b2_pulley_joint.h"
#include "box2d/b2_revolute_joint.h"
#include "box2d/b2_weld_joint.h"
#include "box2d/b2_wheel_joint.h"

namespace physics
{
	//------------------------------------------------------------------------------------------------------------------------
	struct sray
	{
		operator b2RayCastInput() const
		{
			return {{m_origin.x, m_origin.y}, {m_direction.x, m_direction.y}, m_max_fraction};
		}

		vec2_t m_origin;
		vec2_t m_direction;
		float m_max_fraction = 1.0f;
	};

	using aabb_t = b2AABB;
	using ray_t = sray;

	aabb_t aabb(const core::srect& rect);
	aabb_t aabb(const vec2_t& center, const vec2_t& halfextents);
	aabb_t aabb(float x, float y, float size);
	aabb_t aabb(float x, float y, float w, float h);
	core::srect rect(const aabb_t& bbox);

} //- physics