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

	public :
		ActionCamPos(
			const Vec3 postion,
			float radius
		);

		bool IsInRange(const Vec3& playerPos);
	};

}
//end basecross
