/*!
@file ActionCamera.cpp
@brief アクションカメラ実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void ActionCamera::OnCreate()
	{

	}

	void ActionCamera::OnUpdate()
	{
		auto& player = m_Player.lock();
		if (!player) {
			return;
		}

		auto& playerPos = player->GetComponent<Transform>()->GetWorldPosition();

		SetEye(m_roomCenter + m_TopOffset);
		SetAt(m_roomCenter);
	}
}
//end basecross
