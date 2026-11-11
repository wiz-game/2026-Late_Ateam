/*!
@file ActionCamPosMark.cpp
@brief
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void ActionCamPosMark::OnCreate()
	{
		m_drawComp = AddComponent<PNTStaticDraw>();
		m_drawComp->SetMeshResource(L"DEFAULT_SQUARE");
		m_drawComp->SetEmissive(Col4(1.0f, 0.0f, 0.0f, 1.0f));
		m_drawComp->SetRasterizerState(RasterizerState::CullNone);

		m_transComp = GetComponent<Transform>();
		m_transComp->SetPosition(Vec3(0.0f, 1.0f, 0.0f));
	}

	void ActionCamPosMark::OnUpdate()
	{
		auto awner = m_awner.lock();
		if (!awner) return;

		auto ownerTrans = awner->GetComponent<Transform>();

		if (!ownerTrans)
		{
			return;
		}

		auto pos = ownerTrans->GetPosition();

		m_transComp->SetPosition(pos + Vec3(0.0f, 1.0f, 0.0f));
	}
}
//end basecross
