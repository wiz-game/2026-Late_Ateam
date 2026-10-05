/*!
@file MainCamera.cpp
@brief カメラなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void MainCamera::OnCreate()
	{
		m_stateMachine.reset(new StateMachine<MainCamera>(GetThis<MainCamera>()));
		m_stateMachine->ChangeState(CameraNormalMoveState::Instance());
	}

	void MainCamera::OnUpdate()
	{
		if (m_targetObj.expired())
		{
			return;
		}
		auto targetObj = m_targetObj.lock();
		if (!targetObj)
		{
			return;
		}
		m_stateMachine->Update();

	}

	void MainCamera::NormalMove(bool isEditor)
	{
		//注視しているオブジェクトのポインタの確認
		if (m_targetObj.expired())
		{
			return;
		}
		auto targetObj = m_targetObj.lock();
		if (!targetObj)
		{
			return;
		}

		auto& app = App::GetApp();
		auto& device = app->GetInputDevice();
		auto& pad = device.GetControlerVec()[0];
		float delta = app->GetElapsedTime();

		auto trans = targetObj->GetComponent<Transform>();
		auto targetPos = trans->GetPosition();
		auto point = device.GetKeyState().m_MouseClientPoint;

		if (isEditor)
		{
			if (device.GetKeyState().m_bPushKeyTbl[VK_RBUTTON])
			{
				m_angleY -= XMConvertToRadians(point.x - m_pastPoint.x) / 6.0f;
				m_angleX -= XMConvertToRadians(point.y - m_pastPoint.y) / 9.0f;
			}
			m_pastPoint = point;
		}

		m_angleY -= XMConvertToRadians(135.0f) * delta * pad.fThumbRX;
		m_angleX += XMConvertToRadians(90.0f) * delta * pad.fThumbRY;
		if (m_angleX >= XM_PI - m_lowerLimit)
		{
			m_angleX = XM_PI - m_lowerLimit;
		}
		if (m_angleX <= m_upperLimit)
		{
			m_angleX = m_upperLimit;
		}
		Vec3 eye = targetPos + Vec3(cosf(m_angleY) * m_distance * sinf(m_angleX), m_distance * cosf(m_angleX), sinf(m_angleY) * m_distance * sinf(m_angleX));
		Vec3 upperVec = Vec3(0.0f, 0.0f, 0.0f);
		auto gaze = targetPos - eye;
		auto cross = Vec3(gaze.y * upperVec.z - gaze.z * upperVec.y, gaze.z * upperVec.x - gaze.x * upperVec.z, gaze.x * upperVec.y - gaze.y * upperVec.x) * -0.1f;
		auto adjustVec = Vec3(0.0f, 0.0f, 0.0f);
		eye += adjustVec + cross;
		auto at = targetPos + adjustVec + cross;
		//eyeとatが重なってしまった時に使う調整用のベクトル
		Vec3 dis = at - eye;
		if (!isEditor)
		{
			auto dis = eye - targetPos;
			if (dis.length() < 1.0f)
			{
				targetObj->SetDrawActive(false);
			}
			else
			{
				targetObj->SetDrawActive(true);

			}
		}
		if (eye == at)
		{
			eye = at - (dis * 0.1f);
		}
		SetEye(eye);
		SetAt(at);

	}
}
//end basecross

