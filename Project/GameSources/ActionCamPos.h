/*!
@file ActionCamPos.h
@brief 
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	class ActionCamPos 
	//--------------------------------------------------------------------------------------
	class ActionCamPos
	{
		Vec3 m_pos;
		float m_radius;

		Vec3 m_roomCenter;
	public :
		ActionCamPos(
			const Vec3 postion,
			float radius,
			const Vec3& roomCenter
		);

		bool IsInRange(const Vec3& playerPos);

		const Vec3& GetRoomCenter() const
		{
			return m_roomCenter;
		}

	};

}
//end basecross
