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

		auto ptrColl = AddComponent<CollisionCapsule>();
		//ptrColl->SetFixed(true);
		//ptrColl->SetDrawActive(true);

	}

	void Player::OnUpdate()
	{
		//Appとゲームパッドの取得
		auto& app = App::GetApp();
		auto pad = app->GetInputDevice().GetControlerVec()[0];
		//プレイヤーの動き
		NormalMove();
	}

	bool Player::IsJampPressed()
	{
		auto& app = App::GetApp();
		auto pad = app->GetInputDevice().GetControlerVec()[0];

		return (pad.wPressedButtons & XINPUT_GAMEPAD_A);

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
		Vec3 pos = Vec3(0.0f, 5.0f, 0.0f);
		Vec3 rot;

		m_prevY = pos.y;

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

		return v;
	}

	// m_velocityXZの更新
	void Player::VelocityXZUpdate(const Vec3& input)
	{
		float delta = App::GetApp()->GetElapsedTime();
		//入力がないときにVelocityを減少させる
		if (input.length() < 0.1f)
		{
			//空中では慣性を残す(地上は今まで通り)
			float drag = m_isGround ? 0.9f : 0.99f;
			m_velocityXZ.x *= drag;
			m_velocityXZ.z *= drag;
		}
		else
		{
			float Acceleration = 10.0f;//加速度(最高速に達するまでの時間(秒)の逆数)
			//空中では加速度を下げる
			if (!m_isGround)
			{
				Acceleration *= m_airControl;
			}
			//入力をもとに速度を加算
			m_velocityXZ.x += (input.x * cosf(m_cameraAngleY) - input.z * sinf(m_cameraAngleY)) * delta * Acceleration * m_speed;
			m_velocityXZ.z += (input.x * sinf(m_cameraAngleY) + input.z * cosf(m_cameraAngleY)) * delta * Acceleration * m_speed;
			//最大速度の設定(地上は入力の大きさに合わせる。空中は入力が弱くても急に減速させない)
			float maxSpeed = m_isGround ? m_speed * input.length() : m_speed;
			if (m_velocityXZ.length() > maxSpeed)
			{
				m_velocityXZ = m_velocityXZ.normalize() * maxSpeed;
			}
		}
	}
	// m_velocityYの更新
	void Player::VelocityYUpdate(bool isJump)
	{
		auto delta = App::GetApp()->GetElapsedTime();

		//接地中は落下速度を溜めない
		if (m_isGround)
		{
			m_velocityY = 0.0f;
		}
		//常に重力加速度がかかっている
		m_velocityY += -9.8f * delta;

		//ジャンプは接地中だけ
		if (isJump && m_isGround)
		{
			m_velocityY = 7.5f;
		}
	}
	

	bool Player::NormalMove()
	{
		float delta = App::GetApp()->GetElapsedTime();

		auto camera = GetStage()->GetView()->GetTargetCamera();
		Vec3 camForward = camera->GetAt() - camera->GetEye();
		camForward.y = 0.0f;   // 水平方向だけ見る
		if (camForward.length() > 0.001f)
		{
			m_cameraAngleY = atan2f(-camForward.x, camForward.z);
		}

		Vec3 pos = m_transComp->GetPosition();

		m_isGround = (m_velocityY <= 0.0f) && (pos.y - m_prevY > 0.00001f);

		//移動の処理
		auto inputVec = InputVec();
		VelocityXZUpdate(inputVec);
		VelocityYUpdate(IsJampPressed());

		pos.x += m_velocityXZ.x * delta;
		pos.z += m_velocityXZ.z * delta;
		pos.y += m_velocityY * delta;

		m_transComp->SetPosition(pos);
		m_prevY = pos.y;

		// プレイヤーの方向
		float playerAngle = m_transComp->GetQuaternion().toRotVec().y;
		if (inputVec.length() > 0.1f)playerAngle = atan2f(-(inputVec.x * sinf(m_cameraAngleY) + inputVec.z * cosf(m_cameraAngleY)), (inputVec.x * cosf(m_cameraAngleY) - inputVec.z * sinf(m_cameraAngleY))) - XM_PIDIV2;

		//プレイヤーの向きを変更
		m_transComp->SetRotation(Vec3(0.0f, playerAngle, 0.0f));

		return false;
	}
}
//end basecross

