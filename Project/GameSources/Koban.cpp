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

		ptrDraw->SetEmissive(Col4(1.0f, 1.0f, 0.0f, 1.0f));


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


}
//end basecross
