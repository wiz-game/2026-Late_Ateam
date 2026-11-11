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

		// 壁の手前にカメラを置くので、近くまで描画できるようにする(既定の0.5だと壁が欠けて見える)
		SetNear(0.1f);
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

		if (device.GetKeyState().m_bPushKeyTbl[VK_RBUTTON])
		{
			m_angleY -= XMConvertToRadians(point.x - m_pastPoint.x) / 6.0f;
			m_angleX -= XMConvertToRadians(point.y - m_pastPoint.y) / 9.0f;
		}
		m_pastPoint = point;

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
		auto adjustVec = Vec3(0.0f, 1.0f, 0.0f);
		eye += adjustVec + cross;
		auto at = targetPos + adjustVec + cross;

		// 壁にめり込まないように、注視点からの距離を調整する(エディター中は壁を無視して自由に動かす)
		if (!isEditor)
		{
			AvoidCollision(eye, at, delta);
		}

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


	// 注視点(at)からカメラ(eye)までの間に壁・床があれば、カメラを手前に引く
	void MainCamera::AvoidCollision(Vec3& eye, const Vec3& at, float delta)
	{
		Vec3 offset = eye - at;
		float wantDis = offset.length();
		if (wantDis < 0.001f)
		{
			return;
		}
		Vec3 dir = offset * (1.0f / wantDis);

		// 注視点から壁にぶつかるまでの距離(球で測るので、壁から球の半径ぶん手前で止まる)
		float safeDis = JoltStatic::GetSphereCastDistance(at, dir, wantDis, m_collisionRadius);
		safeDis = (std::max)(safeDis, 0.1f);

		if (safeDis < m_curDis)
		{
			// 壁が近づいたらすぐ引く(壁の向こう側が見えてしまうのを防ぐ)
			m_curDis = safeDis;
		}
		else
		{
			// 壁から離れたら、ゆっくり元の距離へ戻す(カメラが急に飛ばないように)
			m_curDis = (std::min)(m_curDis + m_returnSpeed * delta, safeDis);
		}

		eye = at + dir * m_curDis;
	}
}
//end basecross

