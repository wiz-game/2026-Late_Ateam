/*!
@file Player.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void Player::OnCreate()
	{
		//各コンポーネントの初期化
		InitDrawComp();
		InitTransComp();
	}

	void Player::OnUpdate()
	{
		//Appとゲームパッドの取得
		auto& app = App::GetApp();
		auto pad = app->GetInputDevice().GetControlerVec()[0];
		//プレイヤーの動き
		NormalMove();
	}

	// Drawコンポーネントの初期化
	void Player::InitDrawComp()
	{
		//m_drawComp = AddComponent<PNTDXModelDraw>();
		m_drawComp = AddComponent<PNTStaticDraw>();
		m_drawComp->SetMeshResource(L"DEFAULT_CAPSULE");

		// 影付け
		auto shadowComp = AddComponent<Shadowmap>();
		shadowComp->SetMeshResource(L"DEFAULT_CAPSULE");
		//shadowComp->SetMultiMeshResource(L"MODEL_PLAYER_DX_NORMAL");
	}

	// Transformコンポーネントの初期化
	void Player::InitTransComp()
	{
		Vec3 pos = Vec3(0.0f, 0.0f, 0.0f);
		Vec3 rot;

		m_transComp = GetComponent<Transform>();
		m_transComp->SetPosition(pos);
		m_transComp->SetScale(Vec3(m_scale));
		m_transComp->SetRotation(rot);
	}

	//入力された移動操作のベクトルを返す関数
	Vec3 Player::InputVec()
	{
		//各種入力機器と経過時間の取得
		auto& app = App::GetApp();
		auto device = app->GetInputDevice();
		auto pad = device.GetControlerVec()[0];
		auto keyState = device.GetKeyState();
		float delta = app->GetElapsedTime();

		Vec3 v = Vec3(0);
		//X方向の操作
		v.x = pad.fThumbLX;
		if (keyState.m_bPushKeyTbl['A'])
		{
			v.x = -1.0f;
		}
		if (keyState.m_bPushKeyTbl['D'])
		{
			v.x = 1.0f;
		}
		//Z方向の操作
		v.z = pad.fThumbLY;
		if (keyState.m_bPushKeyTbl['W'])
		{
			v.z = 1.0f;
		}
		if (keyState.m_bPushKeyTbl['S'])
		{
			v.z = -1.0f;
		}
		//大きさの調整
		if (v.length() > 1.0f)
		{
			v = v.normalize();
		}

		//ジャンプの確認

		return v;
	}

	// m_velocityXZの更新
	void Player::VelocityXZUpdate(const Vec3& input)
	{
		float delta = App::GetApp()->GetElapsedTime();
		//入力がないときにVelocityを減少させる
		if (input.length() < 0.1f)
		{
			m_velocityXZ.x *= 0.9f;
			m_velocityXZ.z *= 0.9f;
		}
		else
		{
			float Acceleration = 10.0f;//加速度(最高速に達するまでの時間(秒)の逆数)
			//入力をもとに速度を加算
			m_velocityXZ.x += (input.x * cosf(m_cameraAngleY) - input.z * sinf(m_cameraAngleY)) * delta * Acceleration * m_speed;
			m_velocityXZ.z += (input.x * sinf(m_cameraAngleY) + input.z * cosf(m_cameraAngleY)) * delta * Acceleration * m_speed;
			//指定の速度と入力をもとに最大速度を設定
			if (m_velocityXZ.length() > m_speed * input.length())
			{
				m_velocityXZ = m_velocityXZ.normalize() * m_speed * input.length();
			}
		}
	}

	// m_velocityYの更新
	void Player::VelocityYUpdate(bool isJump)
	{
		auto delta = App::GetApp()->GetElapsedTime();
		//常に重力加速度がかかっている
		m_velocityY += -9.8f * delta;
		//ジャンプの処理
		if (isJump)
		{
			m_velocityY = 5.0f;
		}
	}

	bool Player::NormalMove()
	{
		float delta = App::GetApp()->GetElapsedTime();

		//移動の処理
		auto inputVec = InputVec();
		VelocityXZUpdate(inputVec);

		VelocityYUpdate(false);
		Vec3 pos = m_transComp->GetPosition();

		// プレイヤーの方向
		float playerAngle = m_transComp->GetQuaternion().toRotVec().y;
		if (inputVec.length() > 0.1f)playerAngle = atan2f(-(inputVec.x * sinf(m_cameraAngleY) + inputVec.z * cosf(m_cameraAngleY)), (inputVec.x * cosf(m_cameraAngleY) - inputVec.z * sinf(m_cameraAngleY))) - XM_PIDIV2;

		//プレイヤーの向きを変更
		m_transComp->SetRotation(Vec3(0.0f, playerAngle, 0.0f));

		return false;
	}
}
//end basecross

