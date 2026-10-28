/*!
@file ActionCamera.h
@brief アクションカメラ
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class ActionCamPos;

	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class ActionCamera : public Camera
	{
	private:
		weak_ptr<Player> m_Player;

		Vec3 m_TopOffset = Vec3(10.0f, 10.0f, -15.0f);

		Vec3 m_cameraPosition;
		Vec3 m_forward;

	public:

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新

		void SetPleyer(const weak_ptr<Player>& player) { m_Player = player; }

		void SetRoomCrnter(const Vec3& center)
		{
			m_roomCenter = center;
		}

		void SetCameraPosition(const Vec3& pos)
		{
			m_cameraPosition = pos;
		}

		void SetForward(const Vec3& forward)
		{
			m_forward = forward;
		}

		void ApplyTrigger(ActionCamPos* trigger);

	private:
		Vec3 m_roomCenter;

	};

}
//end basecross
