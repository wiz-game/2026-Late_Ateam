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
		Vec3 m_scale;

		float m_radius;
	private :
		static constexpr float ACTION_RADIUS = 1.5f;
		static constexpr float TopAdd = 20.0f;
	public :
		ActionCamPos(
			const shared_ptr<Stage>& stage,
			const Vec3& Scale,
			const Vec3& Rotation,
			const Vec3& Position
		) :
			GameObjectForEdit(stage),
			m_pos(Position),
			m_scale(Scale),
			m_roomCenter(Rotation)
		{
		}

		virtual void OnCreate() override;

		bool IsInRange(const Vec3& playerPos);

		const Vec3& GetRoomCenter() const
		{
			return m_roomCenter;
		}

		Vec3 GatCameraPosition() const;

	};

}
//end basecross
