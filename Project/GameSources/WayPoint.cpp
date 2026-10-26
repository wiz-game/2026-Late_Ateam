/*!
@file WayPoint.cpp
@brief WayPointなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void WayPoint::OnCreate()
	{
		m_transComp = GetComponent<Transform>();
		m_transComp->SetScale(m_Scale);
		m_transComp->SetRotation(m_Rotation);
		m_transComp->SetPosition(m_Position);

		auto ptrDraw = AddComponent<PNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");

		m_drawComp = GetComponent<PNTStaticDraw>();
		m_drawComp->SetDrawActive(false);
	}

	void WayPoint::OnUpdate()
	{

	}

}
//end basecross
