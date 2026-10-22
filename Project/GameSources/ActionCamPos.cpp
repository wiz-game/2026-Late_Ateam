/*!
@file ActionCamPos.cpp
@brief 
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	ActionCamPos::ActionCamPos(const Vec3 postion,float radius, const Vec3& roomCenter)
		: m_pos(postion), m_radius(radius), m_roomCenter(roomCenter)
	{}

	bool ActionCamPos::IsInRange(const Vec3& playerPos)
	{
		Vec3 diff = playerPos - m_pos;

		return diff.length() <= m_radius;
	}
}
//end basecross
