/*!
@file Koban.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void Koban::OnCreate()
	{
		AddTag(L"Koban");

		auto ptrTransform = GetComponent<Transform>();
		SetMemberRotation(m_Rotation);
		ptrTransform->SetScale(m_Scale);
		ptrTransform->SetRotation(m_Rotation);
		ptrTransform->SetPosition(m_Position);

		auto ptrDraw = AddComponent<PNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_SPHERE");

		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetFixed(true);

		m_player = GetStage()->GetSharedGameObject<Player>(L"Player");
	}

	void Koban::OnUpdate()
	{
		auto player = m_player.lock();
		if (!player) return;
		bool canClear = player->GetCanClear();
		
		if (canClear)
		{
			GetStage()->RemoveGameObject<Koban>(GetThis<Koban>());
		}
	}


	REGISTER(Koban, 3, L"Koban");

}
//end basecross
