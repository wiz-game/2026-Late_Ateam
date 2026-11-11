/*!
@file Ground.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void Ground::OnCreate()
	{
		auto ptrTransform = GetComponent<Transform>();
		SetMemberRotation(m_Rotation);
		ptrTransform->SetScale(m_Scale);
		ptrTransform->SetRotation(m_Rotation);
		ptrTransform->SetPosition(m_Position);

		auto ptrDraw = AddComponent<PNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		ptrDraw->SetEmissive(Col4(0.0f));
		ptrDraw->SetOwnShadowActive(true);

		Shadowmap::SetViewWidth(64.0f);
		Shadowmap::SetViewHeight(64.0f);

		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetFixed(true);
		//ptrColl->SetDrawActive(true);

	}

}
//end basecross
