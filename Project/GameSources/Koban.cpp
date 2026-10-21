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
		auto ptrTransform = GetComponent<Transform>();
		SetMemberRotation(m_Rotation);
		ptrTransform->SetScale(m_Scale);
		ptrTransform->SetRotation(m_Rotation);
		ptrTransform->SetPosition(m_Position);

		auto ptrDraw = AddComponent<PNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_SPHERE");

		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetFixed(true);

	}

	void Koban::OnUpdate()
	{

	}

	REGISTER(Koban, 3, L"Koban");

}
//end basecross
