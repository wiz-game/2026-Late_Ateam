/*!
@file ActionCamera.h
@brief アクションカメラ
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class ActionCamera : public Camera
	{
	private:
		weak_ptr<Player> m_Player;

		Vec3 m_TopOffset = Vec3(10.0f, 10.0f, -15.0f);

	public:

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新

		void SetPleyer(const weak_ptr<Player>& player) { m_Player = player; }

		void SetRoomCrnter(const Vec3& center)
		{
			m_roomCenter = center;
		}

	private:
		Vec3 m_roomCenter;

	};

}
//end basecross
