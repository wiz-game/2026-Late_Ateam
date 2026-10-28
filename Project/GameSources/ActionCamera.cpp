/*!
@file ActionCamera.cpp
@brief アクションカメラ実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void ActionCamera::OnCreate()
	{

	}

	void ActionCamera::OnUpdate()
	{
		SetEye(m_cameraPosition);
		SetAt(m_cameraPosition + m_forward * 100.0f);
	}

	void ActionCamera::ApplyTrigger(ActionCamPos* trigger)
	{
		if (!trigger)
		{
			return;
		}

		SetCameraPosition(
			trigger->GetCameraPosition()
		);

		SetForward(
			trigger->GetCameraForward()
		);
	}
}
//end basecross
