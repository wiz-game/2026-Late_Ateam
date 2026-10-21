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
		//各コンポーネントの初期化
		InitDrawComp();
		InitTransComp();
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(true);

		//プレイヤーの取得
		m_player = GetStage()->GetSharedGameObject<Player>(L"Player");
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

		switch (m_state)
		{
		case EnemyState::Patol:
			UpdatePatol(delta);//巡回
			wss << L"Patl";
			//Enemyの視界にPlayerが入っていたらステートを警戒モードにする
			if (IsPlayerInSight(playerPos))
			{
				m_state = EnemyState::Chase;
			}
			break;

		case EnemyState::Chase:
			UpdateChase(delta, playerPos);
			wss << L"Chase";
			break;

		case EnemyState::Catch:
			PostEvent(0.0f, GetThis<Enemy>(), scene, L"ToGameOverStage");
			break;
		}
		app->GetScene<Scene>()->SetDebugString(wss.str());
	}

	//視界の描画用計算
	Vec3 Enemy::RotateY(const Vec3& v, float angle)
	{
		float c = cosf(angle);
		float s = sinf(angle);

		return Vec3(
			v.x * c + v.z * s,
			v.y,
			-v.x * s + v.z * c
		);
	}

	//視界の判定
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
		//角度をラジアンに変換
		float halfAngle = XMConvertToRadians(m_EyeAngle * 0.5f);//正面を中心に半分ずつ
		
		//Playerへの方向と正面のずれが視界の範囲を超えていたら、見えていない
		if (dot < cosf(halfAngle)) return false;

		//どれにも当てはまらなければ見えている
		return true;
	}

	//巡回関数
	void Enemy::UpdatePatol(float delta)
	{
		if (m_patrolPositions.empty()) return;

		Vec3 pos = m_transComp->GetPosition();
		Vec3 target = m_patrolPositions[m_currentIndex];

		Vec3 toTarget = target - pos;	//現在の位置から目的地まで
		toTarget.y = 0.0f;				//水平方向だけで判定
		float dist = toTarget.length();

		//ある程度目的地に到着したら次のポイントへ
		if (dist < m_arriveDistance)
		{
			m_currentIndex++;

			//最後のポイントまで行ったらポイント０に戻る
			if (m_currentIndex >= m_patrolPositions.size())
			{
				m_currentIndex = 0;
			}
			return;
		}

		//目的地に向かって移動
		toTarget.normalize();
		m_forward = toTarget;	//向きを更新
		m_pos += toTarget * m_speed * delta;
		m_transComp->SetPosition(m_pos);

		//見た目も進行方向へ
		float angleY = atan2(toTarget.x, toTarget.z);
		m_transComp->SetRotation(Vec3(0.0f, angleY, 0.0f));
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
		m_forward = toTarget;
		m_pos += toTarget * m_CatchSpeed * delta;
		m_transComp->SetPosition(m_pos);
	}
}