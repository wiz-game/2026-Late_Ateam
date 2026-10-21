/*!
@file InvisibleWall.cpp
@brief 侵入不可オブジェクトなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "InvisibleWall.h"

namespace basecross {

	//初期化
	void InvisibleWall::OnCreate()
	{
		auto ptrTransform = GetComponent<Transform>();
		ptrTransform->SetScale(m_Scale);
		ptrTransform->SetRotation(m_Rotation);
		ptrTransform->SetPosition(m_Position);

		auto ptrDraw = AddComponent<PNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		ptrDraw->SetEmissive(Col4(1.0f,1.0f,1.0f,0.1f));

		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(true);
		// ptrColl->SetFixed(true);

	}

	void InvisibleWall::OnUpdate()
	{
		
	}

	REGISTER(InvisibleWall, 6, L"InvisibleWall");

}
//end basecross
