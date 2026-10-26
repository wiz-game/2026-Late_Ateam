/*!
@file ActionCamPos.cpp
@brief 
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void ActionCamPos::OnCreate()
	{
		auto ptrTransform = GetComponent<Transform>();
		ptrTransform->SetPosition(m_pos);
		ptrTransform->SetScale(m_scale);
		ptrTransform->SetRotation(m_roomCenter);

		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_SQUARE");
		m_draw->SetEmissive(Col4(1.0f));
	}

	bool ActionCamPos::IsInRange(const Vec3& playerPos)
	{
		auto pos = GetComponent<Transform>()->GetPosition();

		Vec3 diff = playerPos - pos;

		return diff.length() <= m_radius;
	}
}
//end basecross
