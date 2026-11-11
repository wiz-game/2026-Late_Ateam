/*!
@file Wall.cpp
@brief 壁など実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void Wall::OnCreate()
	{
		AddTag(L"Wall");
		auto ptrTransform = GetComponent<Transform>();
		ptrTransform->SetScale(m_Scale);
		ptrTransform->SetRotation(m_Rotation);
		ptrTransform->SetPosition(m_Position);

		auto ptrDraw = AddComponent<PNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		ptrDraw->SetTextureResource(L"Wall");
		ptrDraw->SetEmissive(Col4(0.05f));

		auto shadowComp = AddComponent<Shadowmap>();
		shadowComp->SetMeshResource(L"DEFAULT_CUBE");

		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetFixed(true);
	}

}
//end basecross
