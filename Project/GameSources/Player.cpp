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

		auto ptrColl = AddComponent<CollisionCapsule>();   // CollisionSphere から変更
		ptrColl->SetMakedRadius(COLL_RADIUS);
		ptrColl->SetMakedHeight(COLL_LENGTH);
		ptrColl->SetDrawActive(false);

		// 壁・床の押し戻しはJolt側で行うので、BaseCross側の判定は「当たった」という通知にだけ使う
		ptrColl->SetAfterCollision(AfterCollision::None);

		// Jolt側のキャラクターコントローラー
		InitController();
	}

	void Player::OnUpdate()
	{

		//プレイヤーの動き
		NormalMove();

	}

	void Player::OnCollisionEnter(shared_ptr<GameObject>& Other)
	{
		auto& app = App::GetApp();
		auto scene = app->GetScene<Scene>();

		if (Other->FindTag(L"Koban"))
		{
			SoundManager::Instance().PlaySE(L"SE_Koban", false);
			m_canClear = true;
		}

		if (Other->FindTag(L"ClearObject"))
		{
			if (m_canClear == true)
			{
				PostEvent(0.0f, GetThis<Player>(), scene, L"ToClearStage");
				m_canClear = false;
			}
		}

		if (Other->FindTag(L"Enemy"))
		{
			PostEvent(0.0f, GetThis<Player>(), scene, L"ToGameOverStage");
		}
	}

	void Player::OnCollisionExecute(shared_ptr<GameObject>&Other)
	{
		
	}

	void Player::OnCollisionExit(shared_ptr<GameObject>&Other)
	{
		
	}

	bool Player::IsJampPressed()
	{
		auto& app = App::GetApp();
		auto pad = app->GetInputDevice().GetControlerVec()[0];
		auto key = app->GetInputDevice().GetKeyState();

		return (pad.wPressedButtons & XINPUT_GAMEPAD_A || key.m_bPressedKeyTbl[VK_SPACE]);
	}

	// Drawコンポーネントの初期化
	void Player::InitDrawComp()
	{
		auto ptrDraw = AddComponent<PNTDXModelDraw>();
		ptrDraw->SetMultiMeshResource(L"Player_NinjaCat");

		// 影付け
		auto shadowComp = AddComponent<Shadowmap>();
		shadowComp->SetMultiMeshResource(L"Player_NinjaCat");

		Mat4x4 span, mT, mR;
		span.scale(Vec3(0.5f));
		mT.translation(Vec3(0.0f, -COLL_RADIUS, 0.0f));
		Quat q; q.rotationX(-XM_PIDIV2);
		mR.rotation(q);
		span *= mT;
		span *= mR;
		ptrDraw->SetMeshToTransformMatrix(span);
		shadowComp->SetMeshToTransformMatrix(span);        // 影にも同じ補正

	}

	// Transformコンポーネントの初期化
	void Player::InitTransComp()
	{
		Vec3 pos = Vec3(0.0f, 0.0f, 0.0f);
		Vec3 rot(XM_PIDIV2, 0.0f, 0.0f);

		m_transComp = GetComponent<Transform>();
		m_transComp->SetPosition(pos);
		m_transComp->SetScale(Vec3(m_scale));
		m_transComp->SetRotation(rot);
	}

	void Player::InitController()
	{
		CharacterController::Settings settings;
		settings.radius = COLL_RADIUS * m_scale;                         // 半径(0.3)
		settings.length = (COLL_LENGTH + COLL_RADIUS * 2.0f) * m_scale;  // 全長(1.2)。両端の丸い部分を含む
		settings.isLying = true;                                         // 見た目に合わせて横に寝かせる

		m_controller = AddComponent<CharacterController>();
		if (!m_controller->Initialize(settings))
		{
			throw BaseException(
				L"JoltのPhysicsSystemがありません。GameStageでJoltManager::Initializeを先に呼んでください",
				L"m_controller->Initialize()",
				L"Player::InitController()"
			);
		}
	}

	// ダッシュ操作がされているか
	bool Player::GetIsDashPressed()
	{
		auto& app = App::GetApp();
		auto pad = app->GetInputDevice().GetControlerVec()[0];
		auto key = app->GetInputDevice().GetKeyState();

		return key.m_bPushKeyTbl[VK_LSHIFT] || (pad.wButtons & XINPUT_GAMEPAD_X) != 0;
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

		// ダッシュ中は、最高速と加速を倍率ぶん上げる(最高速に達するまでの時間は変わらない)
		float speed = m_isDash ? m_speed * m_dashRate : m_speed;

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
			//加速度
			float Acceleration = 10.0f;

			//空中では加速度を下げる
			if (!m_isGround)
			{
				Acceleration *= m_airControl;
			}
			//入力をもとに速度を加算
			m_velocityXZ.x += (input.x * cosf(m_cameraAngleY) - input.z * sinf(m_cameraAngleY)) * delta * Acceleration * speed;
			m_velocityXZ.z += (input.x * sinf(m_cameraAngleY) + input.z * cosf(m_cameraAngleY)) * delta * Acceleration * speed;

			//最大速度の設定
			float maxSpeed = m_isGround ? speed * input.length() : speed;
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
			SoundManager::Instance().PlaySE(L"SE_Jump", false);
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

		// エディターなどでTransformが直接動かされていたら、Joltのキャラクターもそこへ移す
		if (m_controller->SyncFromTransform())
		{
			m_velocityXZ = Vec3(0.0f);
			m_velocityY = 0.0f;
		}

		// 接地しているか(前フレームの移動結果をJoltに聞く)
		m_isGround = m_controller->GetIsOnGround();
		if (m_isGround)
		{
			m_isDash = GetIsDashPressed();
		}

		//移動の処理
		auto inputVec = InputVec();
		VelocityXZUpdate(inputVec);
		VelocityYUpdate(IsJampPressed());

		// プレイヤーの方向
		if (inputVec.length() > 0.1f)
		{
			m_angleY = atan2f(-(inputVec.x * sinf(m_cameraAngleY) + inputVec.z * cosf(m_cameraAngleY)), (inputVec.x * cosf(m_cameraAngleY) - inputVec.z * sinf(m_cameraAngleY))) - XM_PIDIV2;
		}

		// Jolt側で壁・床にぶつかりながら動かす(結果の位置はTransformに入る)
		Vec3 prevPos = m_transComp->GetPosition();
		m_controller->SetLinearVelocity(Vec3(m_velocityXZ.x, m_velocityY, m_velocityXZ.z));
		m_controller->SetAngleY(m_angleY);
		m_controller->Move(delta);

		// 頭を天井にぶつけたら上昇をやめる(止めないと天井に張り付いたまま上昇速度が残る)
		Vec3 movedPos = m_transComp->GetPosition();
		if (m_velocityY > 0.0f && (movedPos.y - prevPos.y) < m_velocityY * delta * 0.5f)
		{
			m_velocityY = 0.0f;
		}

		//プレイヤーの向きを変更
		m_transComp->SetRotation(Vec3(XM_PIDIV2, m_angleY, 0.0f));
		return false;
	}

	// プレイヤーのアップデートを一時的に止める処理
	void Player::SetPlayerUpdateActive(bool isActive)
	{
		SetUpdateActive(isActive); // GameObject標準の関数を呼び出す
	}
}
//end basecross

