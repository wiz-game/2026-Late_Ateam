/*!
@file Enemy.cpp
@brief 敵キャラクターの実体
*/
#include "stdafx.h"
#include "Enemy.h"

namespace basecross {
	void Enemy::InitDrawComp()
	{
		m_drawComp = AddComponent<PNTStaticDraw>();
		m_drawComp->SetMeshResource(L"DEFAULT_CUBE");
		m_drawComp->SetDiffuse(Col4(1.0f, 0.0f, 0.0f, 1.0f));

		// 影付け
		auto shadowComp = AddComponent<Shadowmap>();
		shadowComp->SetMeshResource(L"DEFAULT_CUBE");
	}

	void Enemy::InitTransComp()
	{
		m_transComp = AddComponent<Transform>();
		m_transComp->SetPosition(m_pos);
		m_transComp->SetScale(m_scale);
		m_transComp->SetRotation(m_rot);
	}

	void Enemy::OnCreate()
	{
		AddTag(L"Enemy");

		//各コンポーネントの初期化
		InitDrawComp();
		InitTransComp();

		// 初期位置の初期化
		m_spawnPos = m_pos;

		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(true);

		//プレイヤーの取得
		m_player = GetStage()->GetSharedGameObject<Player>(L"Player");
		m_wpManager = GetStage()->GetSharedGameObject<WayPointManager>(L"WayPointManager");

		//角度は変わらないためOnCreateで実装
		float inner = m_EyeAngle * 0.5f; //両眼の端
		float outer = (std::min)(inner + m_AlertEyeAngle, 180.0f);//単眼の外端

		//角度をcosにする（内積を使用するため）
		m_innerCos = cosf(XMConvertToRadians(inner));
		m_outerCos = cosf(XMConvertToRadians(outer));
	}

	void Enemy::OnUpdate()
	{
		auto& app = App::GetApp();
		auto delta = app->GetElapsedTime();
		auto scene = app->GetScene<Scene>();
		std::wstringstream wss(L"");

		//Playerの位置を取得
		auto player = m_player.lock();
		if (!player) return;
		Vec3 playerPos = player->GetComponent<Transform>()->GetPosition();

		//最新座標をm_posに反映する
		m_pos = m_transComp->GetPosition();

		switch (m_state)
		{
		case EnemyState::Patol:
			UpdatePatol(delta);//巡回
			wss << L"Patl";
			//両眼視界に入っていたら捕獲モード
			if (IsPlayerInSight(playerPos))
			{
				m_state = EnemyState::Chase;
			}
			//単眼視界に入っていたら警戒モード
			else if (AlertEye(playerPos))
			{
				m_state = EnemyState::Alert;
			}
			else
			{
				m_state = EnemyState::Patol;
			}
			break;

		case EnemyState::Alert:
			wss << L"Alert";
			if (IsPlayerInSight(playerPos))
			{
				m_state = EnemyState::Chase;
			}
			else if (AlertEye(playerPos))
			{
				//単眼視界に入っていたらEnemyの向きをPlayer方向にする
				TurnPlayer(playerPos, delta);
			}
			else
			{
				//何もなければ巡回に戻る
				m_state = EnemyState::Patol;
			}
			break;


		case EnemyState::Chase:
			UpdateChase(delta, playerPos);
			wss << L"Chase";

			//両眼視界に入っていたら捕獲モード
			if (IsPlayerInSight(playerPos))
			{
				m_state = EnemyState::Chase;
			}
			//単眼視界に入っていたら警戒モード
			else if (AlertEye(playerPos))
			{
				m_state = EnemyState::Alert;
			}
			else
			{
				m_state = EnemyState::Patol;
			}

			break;

		case EnemyState::Catch:
			break;
		}

		
		app->GetScene<Scene>()->SetDebugString(wss.str());
	}

	////視界の描画用計算
	//Vec3 Enemy::RotateY(const Vec3& v, float angle)
	//{
	//	float c = cosf(angle);
	//	float s = sinf(angle);

	//	return Vec3(
	//		v.x * c + v.z * s,
	//		v.y,
	//		-v.x * s + v.z * c
	//	);
	//}

	//m_forwardを見た目に反映
	void Enemy::ApplyRotation()
	{
		float angleY = atan2(m_forward.x, m_forward.z);
		m_transComp->SetRotation(Vec3(0.0f, angleY, 0.0f));
	}
	
	//Player方向をゆっくり向く関数
	void Enemy::TurnPlayer(const Vec3& playerPos, float delta)
	{
		//Playerの方向を求める
		Vec3 toPlayer = playerPos - m_pos;
		toPlayer.y = 0.0f;

		if (toPlayer.length() < 0.01f) return;
		toPlayer.normalize();

		//向きを角度に変換
		float current = atan2(m_forward.x, m_forward.z);//現在敵が向いている角度
		float target = atan2(toPlayer.x, toPlayer.z);	//本来向きたい角度

		float diff = target - current;
		//-180°～+180°の範囲で収まるようにする
		while (diff > XM_PI) diff -= XM_2PI;	//例）360°なら2π引く
		while (diff < -XM_PI) diff += XM_2PI;

		float maxStep = m_turnSpeed * delta;
		diff = std::clamp(diff, -maxStep, maxStep);

		float newAngle = current + diff;
		m_forward = Vec3(sinf(newAngle), 0.0f, cosf(newAngle));
		ApplyRotation();
	}

	//両眼視界（発見）の判定
	bool Enemy::IsPlayerInSight(const Vec3& playerPos) const
	{
		//距離
		Vec3 toPlayer = playerPos - m_pos;
		toPlayer.y = 0.0f;	//高さは無視
		float dist = toPlayer.length();

		//Playerが視界範囲より遠いなら、見えていない（移行の処理はしない）
		if (dist > m_EyeRenge) return false;

		//ほぼ同じ位置にいるなら見えている
		if (dist < 0.01f) return true;

		toPlayer.normalize();
		float dot = toPlayer.dot(m_forward);//内積:1に近いほど正面 -1は真後ろ

		//両眼視界：角度 < 40°⇔ dot >= cos( 40°) = 0.5
		if (dot >= m_innerCos)
		{
			return !IsBlockedByWall(playerPos);
		}
		return false;
	}

	//単眼視界（警戒）の判定
	bool Enemy::AlertEye(const Vec3& playerPos) const
	{
		//距離
		Vec3 toPlayer = playerPos - m_pos;
		toPlayer.y = 0.0f;	//高さは無視
		float dist = toPlayer.length();

		//Playerが視界範囲より遠いなら、見えていない（移行の処理はしない）
		if (dist > m_EyeRenge) return false;

		//ほぼ同じ位置にいるなら見えている
		if (dist < 0.01f) return true;

		toPlayer.normalize();
		float dot = toPlayer.dot(m_forward);

		//単眼視界：40°< 角度 <= 90°⇔ cos( 90°) <= dot < cos( 40°)
		if (dot < m_innerCos && dot >= m_outerCos)
		{
			return !IsBlockedByWall(playerPos);
		}

		return false;
	}


	//巡回関数
	void Enemy::UpdatePatol(float delta)
	{
		auto mgr = m_wpManager.lock();
		if (!mgr)
		{
			//m_wpManagerが空だったら再取得する
			mgr = GetStage()->GetSharedGameObject<WayPointManager>(L"WayPointManager", false);
			m_wpManager = mgr;
			if (!mgr)return;
		}

		size_t count = mgr->GetCount(m_groupId);
		if (count == 0)return;

		if (m_currentIndex >= count) m_currentIndex = 0;//WayPointが減った場合の保険

		Vec3 pos = m_transComp->GetPosition();
		Vec3 toTarget = mgr->GetPoint(m_groupId,m_currentIndex) - pos;//現在の位置から目的地まで
		toTarget.y = 0.0f;				//水平方向だけで判定
		float dist = toTarget.length();

		//ある程度目的地に到着したら次のポイントへ
		if (dist < m_arriveDistance)
		{
			//最後まで行ったら最初のWayPointに戻る
			m_currentIndex = (m_currentIndex + 1) % count;
			return;
		}

		//目的地に向かって移動
		toTarget.normalize();
		m_forward = toTarget;	//向きを更新
		m_pos = pos + toTarget * m_speed * delta;
		m_transComp->SetPosition(m_pos);

		//見た目も進行方向へ
		ApplyRotation();
	}

	//追跡関数
	void Enemy::UpdateChase(float delta, const Vec3& playerPos)
	{
		Vec3 toTarget =  playerPos - m_pos;
		toTarget.y = 0.0f;
		float dist = toTarget.length();

		if (dist < m_CatchRenge)
		{
			m_state = EnemyState::Catch;
			return;
		}

		toTarget.normalize();

		Vec3 moveDir = AvoidWall(toTarget);//壁を避ける移動ベクトルを計算

		//見た目も進行方向へ
		m_pos += moveDir * m_CatchSpeed * delta;
		m_transComp->SetPosition(m_pos);
		TurnPlayer(playerPos, delta);
	}

	//壁に遮られているか判定
	bool Enemy::IsBlockedByWall(const Vec3& targetPos) const
	{
		//床との誤判定を防ぐために、少し上にあげる
		Vec3 start = m_pos + Vec3(0.0f, 0.5f, 0.0f);
		Vec3 end = targetPos + Vec3(0.0f, 0.5f, 0.0f);

		for (auto obj : GetStage()->GetGameObjectVec())
		{
			if (obj->FindTag(L"Wall"))
			{
				auto wallObj = obj->GetComponent<CollisionObb>(false);

				if (wallObj)
				{
					//敵からターゲットまでの線分が壁のOBBと交差したら「遮られている(true)」
					if (HitTest::SEGMENT_OBB(start, end, wallObj->GetObb()))
					{
						return true;
					}
				}
			}
		}

		return false;//何もなかったら遮られていない
	}

	Vec3 Enemy::AvoidWall(const Vec3 moveDir) const
	{
		float feelerLength = 2.0f;//壁を検知する距離
		Vec3 start = m_pos;
		Vec3 endCenter = start + moveDir * feelerLength;

		if (!IsBlockedByWall(endCenter))
		{
			return moveDir;
		}

		float angle = atan2(moveDir.x, moveDir.z);
		float offset = XMConvertToRadians(45.0f);//30°ずつずらす

		Vec3 leftDir(sinf(angle - offset), 0.0f, cosf(angle - offset));
		Vec3 rightDir(sinf(angle + offset), 0.0f, cosf(angle + offset));


		Vec3 endLeft = start + leftDir * feelerLength;
		// 左右の方向で壁がないか確認
		if (!IsBlockedByWall(endLeft))
		{
			return leftDir;
		}

		return rightDir;
	}


	void Enemy::ResetToSpawn()
	{
		m_pos = m_spawnPos;
		m_transComp->SetPosition(m_pos);

		Vec3 rot = GetMemberRotation();     // エディターで設定した向き
		m_transComp->SetRotation(rot);
		m_forward = Vec3(sinf(rot.y), 0.0f, cosf(rot.y));   // ApplyRotation と同じ向きの定義

		m_currentIndex = 0;
		m_state = EnemyState::Patol;
	}

	void Enemy::ResetToSpawnTransform()
	{
		m_spawnPos = m_transComp->GetPosition();
		ResetToSpawn();
	}
}