/*!
@file Tourou.cpp
@brief 灯籠など実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void Tourou::OnCreate()
	{
		auto ptrTransform = GetComponent<Transform>();
		ptrTransform->SetScale(m_Scale);
		ptrTransform->SetRotation(m_Rotation);
		ptrTransform->SetPosition(m_Position);

		auto ptrDraw = AddComponent<PNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
	}

}
//end basecross
