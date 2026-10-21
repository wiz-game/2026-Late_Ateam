/*!
@file ClearObject.cpp
@brief ゴールなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void ClearObject::OnCreate()
	{
		AddTag(L"ClearObject");

		auto ptrTransform = GetComponent<Transform>();
		ptrTransform->SetScale(m_Scale);
		ptrTransform->SetRotation(m_Rotation);
		ptrTransform->SetPosition(m_Position);

		auto ptrDraw = AddComponent<PNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		ptrDraw->SetEmissive(Col4(1.0f, 0.0f, 1.0f, 1.0f));
		ptrDraw->SetBlendState(BlendState::Additive);
	}

	REGISTER(ClearObject, 5, L"ClearObject");

}
//end basecross
