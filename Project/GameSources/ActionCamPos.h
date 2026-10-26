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
	class ActionCamPos : public GameObjectForEdit
	{
		std::shared_ptr<PNTStaticDraw> m_draw;

		Vec3 m_pos;
		Vec3 m_roomCenter;
		Vec3 m_cameraOffset;

		float m_radius;

	public :
		ActionCamPos(
			const shared_ptr<Stage>& stage,
			const Vec3 postion,
			float radius,
			const Vec3& roomCenter
		) :
			GameObjectForEdit(stage),
			m_pos(postion),
			m_radius(radius),
			m_roomCenter(roomCenter)
		{
		}

		virtual void OnCreate() override;

		bool IsInRange(const Vec3& playerPos);

		const Vec3& GetRoomCenter() const
		{
			return m_roomCenter;
		}

	};

}
//end basecross
