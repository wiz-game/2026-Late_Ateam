/*!
@file Enemy.cpp
@brief 敵キャラクターの実体
*/
#include "stdafx.h"
#include "Enemy.h"
#include "EnemySight.h"

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
		m_prevY = m_pos.y;//前フレームのY座標を初期化
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

		GetStage()->AddGameObject<EnemySight>(GetThis<Enemy>(), m_EyeAngle, m_AlertEyeAngle);
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

		//地面にいるかどうかの判定
		if ((m_velocityY <= 0.0f) && (m_pos.y - m_prevY > 0.00001f))
		{
			m_isGround = true;

		}
		else
		{
			m_isGround = false;
		}
		m_wantJump = false;	//ジャンプしたいかどうかのフラグをリセット

		bool isInShadow = m_player.lock()->GetIsInShadow();
		if (isInShadow)
		{
			m_EyeRenge = 10.0f;
		}
		else
		{
			m_EyeRenge = 20.0f;
		}

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
			//チュールを見つけたらチュールへ向かう
			else if (FindChuru())
			{
				m_state = EnemyState::Churu;
			}
			//単眼視界に入っていたら警戒モード
			else if (AlertEye(playerPos))
			{
				m_state = EnemyState::Alert;
			}
			else if (HearPlayer(playerPos))//聴覚でPlayerを見つけたら捜索モードに移行
			{
				m_state = EnemyState::Search;
				m_searchTime = 10.0f;
				m_searchChangeTime = 2.0f;
				m_wanderTarget = playerPos;//最初の目的地は「最後に見た位置」に設定
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
			//警戒中でもチュールがあれば釣られる
			else if (FindChuru())
			{
				m_state = EnemyState::Churu;
			}
			else if (AlertEye(playerPos))
			{
				//単眼視界に入っていたらEnemyの向きをPlayer方向にする
				TurnPlayer(playerPos, delta);
				m_lastPlayerPos = playerPos;//Playerを見失ったときの座標を更新
			}
			else
			{
				//-SetNextWayPoint();//一番近いウェイポイントを探して設定する
				//見失ったら捜索モードに移行
				m_state = EnemyState::Search;
				m_searchTime = 10.0f;
				m_searchChangeTime = 2.0f;
				m_wanderTarget = m_lastPlayerPos;//最初の目的地は「最後に見た位置」に設定
			}
			break;


		case EnemyState::Chase:
			UpdateChase(delta, playerPos);
			wss << L"Chase";

			//追跡中の座標履歴を記録
			m_breadcrumbTime -= delta;
			if (m_breadcrumbTime <= 0.0f)
			{
				m_breadcrumbTime = 1.0f;//1.0fごとに履歴を記録
				m_breadcrumb.push_back(m_pos);
			}

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
				//見失ったら捜索モードに移行
				m_state = EnemyState::Search;
				m_searchTime = 10.0f;
				m_searchChangeTime = 2.0f;
				m_wanderTarget = m_lastPlayerPos;//最初の目的地は「最後に見た位置」に設定
			}
			break;

		case EnemyState::Search:
			//聴覚でPlayerを見つけたら捜索モードに移行
			if (HearPlayer(playerPos))
			{
				m_state = EnemyState::Search;
				m_searchTime = 10.0f;
				m_searchChangeTime = 2.0f;
				m_wanderTarget = playerPos;//最初の目的地は「最後に見た位置」に設定
			}

			UpdateSearch(delta);
			wss << L"Search:" << m_searchTime;

			//両眼視界に入っていたら捕獲モード
			if (IsPlayerInSight(playerPos))
			{
				m_state = EnemyState::Chase;
			}
			//探索中にチュールを見つけたらチュールへ向かう
			else if (FindChuru())
			{
				m_state = EnemyState::Churu;
			}
			//単眼視界に入っていたら警戒モード
			else if (AlertEye(playerPos))
			{
				m_state = EnemyState::Alert;
			}
			else if (m_searchTime <= 0.0f)
			{
				//一番近いウェイポイントを探して設定する
				if (SetNextWayPoint() || m_breadcrumb.empty())
				{
					//直接見えるWayPointが見つかったら足跡を消す
					m_breadcrumb.clear();
					//捜索時間がなくなったら巡回モードに戻る
					m_state = EnemyState::Patol;
				}
			}
			break;
		case EnemyState::Churu:
			UpdateChuru(delta);
			wss << L"Churu:";

			//両眼視界に入っていたら捕獲モード
			if (IsPlayerInSight(playerPos))
			{
				m_GetChuru = false;
				m_churuWaitTime = 0.0f;
				m_state = EnemyState::Chase;
			}
			//単眼視界に入っていたら警戒モード
			else if (AlertEye(playerPos))
			{
				m_GetChuru = false;
				m_churuWaitTime = 0.0f;
				m_state = EnemyState::Alert;
			}

			break;

		case EnemyState::Catch:
			break;
		}

		//重力とジャンプの処理
		VelocityYUpdate(m_wantJump);
		m_pos.y += m_velocityY * delta;
		
		m_transComp->SetPosition(m_pos);
		m_prevY = m_pos.y;//前フレームのY座標を更新

		app->GetScene<Scene>()->SetDebugString(wss.str());
	}

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
		float dist = toPlayer.length();//Yを０にする前に距離を計算する
		
		//Playerが視界範囲より遠いなら、見えていない（移行の処理はしない）
		if (dist > m_EyeRenge) return false;

		toPlayer.y = 0.0f;//向きの計算のために平行にする

		//ほぼ同じ位置にいるなら見えている
		if (dist < 0.01f) return true;

		toPlayer.normalize();
		float dot = toPlayer.dot(m_forward);//内積:1に近いほど正面 -1は真後ろ

		//両眼視界：角度 < 40°⇔ dot >= cos( 40°) = 0.5
		if (dot >= m_innerCos)
		{
			return !IsBlockedByWall(m_pos, playerPos);
		}
		return false;
	}

	//単眼視界（警戒）の判定
	bool Enemy::AlertEye(const Vec3& playerPos) const
	{
		//距離
		Vec3 toPlayer = playerPos - m_pos;
		float dist = toPlayer.length();

		//Playerが視界範囲より遠いなら、見えていない（移行の処理はしない）
		if (dist > m_EyeRenge) return false;

		toPlayer.y = 0.0f;//向きの計算のために平行にする

		//ほぼ同じ位置にいるなら見えている
		if (dist < 0.01f) return true;

		toPlayer.normalize();
		float dot = toPlayer.dot(m_forward);

		//単眼視界：40°< 角度 <= 90°⇔ cos( 90°) <= dot < cos( 40°)
		if (dot < m_innerCos && dot >= m_outerCos)
		{
			return !IsBlockedByWall(m_pos, playerPos);
		}

		return false;
	}

	//Y方向の速度更新
	void Enemy::VelocityYUpdate(bool isJump)
	{
		auto delta = App::GetApp()->GetElapsedTime();
		if (m_isGround)
		{
			m_velocityY = 0.0f;
		}
		else
		{
			m_velocityY -= 9.8f * delta;
		}

		if (isJump && m_isGround)
		{
			m_velocityY = 7.5f;
		}
	}


	//巡回モード関数
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
		
		float heightDiff = toTarget.y;	//目的地のWaiPointの高さと現在の高さの差
		toTarget.y = 0.0f;				//水平方向だけで判定
		float dist = toTarget.length();

		//ある程度目的地に到着したら次のポイントへ
		if (dist < m_arriveDistance && abs(heightDiff) < 1.0f)
		{
			//最後まで行ったら最初のWayPointに戻る
			m_currentIndex = (m_currentIndex + 1) % count;
			return;
		}

		//次のWayPointが高い位置にある場合、かつ水平距離が近い場合ジャンプする
		if (heightDiff > 1.0f && dist < 3.0f)
		{
			m_wantJump = true;
		}

		//目的地に向かって移動
		if (dist > 0.01f)
		{
			toTarget.normalize();
			Vec3 moveDir = AvoidWall(toTarget);//壁を避ける移動ベクトルを計算

			m_forward = moveDir;	//向きを更新
			m_pos.x = pos.x + moveDir.x * m_speed * delta;
			m_pos.z = pos.z + moveDir.z * m_speed * delta;
			m_transComp->SetPosition(m_pos);
		}
		//見た目も進行方向へ
		ApplyRotation();
	}

	//追跡モード関数
	void Enemy::UpdateChase(float delta, const Vec3& playerPos)
	{
		float distToPlayer = (playerPos - m_pos).length();

		//ステートの切り替え
		if (distToPlayer < m_CatchRenge)
		{
			m_state = EnemyState::Catch;
			return;
		}

		//Enemy同士の反発するベクトル
		Vec3 separation(0.0f);
		float separateRadius = 2.0f;//2.0f以内に他のEnemyがいたら離れる
		
		bool passedSelf = false;
		int chaseIndex = 0;

		//役割分担コード＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

		for (auto& obj : GetStage()->GetGameObjectVec())
		{
			if (obj.get() == this)
			{
				passedSelf = true;
				continue;//自分自身はスキップ
			}

			//Playerを追いかけているEnemyを調べて番号を付ける
			auto otherEnemy = std::dynamic_pointer_cast<Enemy>(obj);
			if (otherEnemy)
			{
				//反発は「全Enemy」から受け取る
				Vec3 diff = m_pos - otherEnemy->GetComponent<Transform>()->GetPosition();
				diff.y = 0.0f;
				float d = diff.length();

				//自分と他のEnemyの距離が2.0fいないだったら藩閥させる
				if (d > 0.001f && d < separateRadius)
				{
					diff.normalize();
					//近いほど強く押し出す
					separation += diff * ((separateRadius - d) / separateRadius);
				}

				//自分より前の番号のEnemyかつ「現在Chase中のEnemy」だったらカウントする
				if (!passedSelf && otherEnemy->GetState() == EnemyState::Chase)
				{
					chaseIndex++;
				}
			}
		}

		//目的地を決定する
		Vec3 targetPos = playerPos;

		//距離が離れていて、追いかけているEnemyが1体以上いたら回り込む
		if (distToPlayer > 3.5f && chaseIndex > 1)
		{
			Vec3 forward = m_player.lock()->GetComponent<Transform>()->GetForward();
			forward.y = 0.0f;

			if (forward.length() > 0.01f)
			{
				forward.normalize();
			}

			Vec3 right(forward.z, 0.0f, -forward.x);//進行方向に対して右向きのベクトル

			//一番目は右前方、二番目は左前方、三番目はさらに大きく回り込む
			float sideSign = (chaseIndex % 2 == 1) ? 1.0f : -1.0f;
			float forwardOffset = 4.0f;//Playerの4.0f先を狙う
			float sideOffset = 3.0f * ((chaseIndex + 1) / 2);//横に3.0fずつずらす

			Vec3 flankPos = playerPos + forward * forwardOffset + right * (sideSign * sideOffset);

			//壁にさえぎられていない時だけ先回りし、壁があるときは素直にPlayerを追う
			if (!IsBlockedByWall(m_pos, flankPos))
			{
				targetPos = flankPos;
			}
		}

		//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
		
		//追跡コード
		Vec3 toTarget = targetPos - m_pos;
		float heightDiff = toTarget.y;	//目的地の高さと現在の高さの差
		toTarget.y = 0.0f;

		if (heightDiff > 1.0f && distToPlayer < 3.5f)
		{
			m_wantJump = true;
		}

		if (distToPlayer > 0.01f)
		{
			toTarget.normalize();//「目標へ向かうベクトル」を正規化
			toTarget += separation * 0.8f;//正規化した移動方向に反発を合成

			if (distToPlayer > 0.01f)
			{
				//もう一度正規化
				toTarget.normalize();//反発が加わった瞬間に移動スピードまで早くなってしまうため
				Vec3 moveDir = AvoidWall(toTarget);//壁を避ける移動ベクトルを計算

				//見た目も進行方向へ
				m_pos.x += moveDir.x * m_CatchSpeed * delta;
				m_pos.z += moveDir.z * m_CatchSpeed * delta;

				m_transComp->SetPosition(m_pos);
			}
		}
		TurnPlayer(playerPos, delta);
		m_lastPlayerPos = playerPos;//Playerを見失ったときの座標を更新

	}

	//捜索モード関数
	void Enemy::UpdateSearch(float delta)
	{
		m_searchTime -= delta;

		if (m_searchTime <= 0.0f)
		{
			if (!m_breadcrumb.empty())
			{
				Vec3 returnPos = m_breadcrumb.back();
				Vec3 toReturn = returnPos - m_pos;
				toReturn.y = 0.0f;

				if (toReturn.length() < m_arriveDistance)
				{
					//足跡の座標に到着したら、履歴を消す
					m_breadcrumb.pop_back();
				}
				else
				{
					toReturn.normalize();
					Vec3 moveDir = AvoidWall(toReturn);//壁を避ける移動ベクトルを計算
					m_pos.x += moveDir.x * m_speed * m_speedRate * delta;
					m_pos.z += moveDir.z * m_speed * m_speedRate * delta;
					m_transComp->SetPosition(m_pos);
					TurnPlayer(m_pos + moveDir, delta);
				}
			}
			return;
		}

		m_searchChangeTime -= delta;

		Vec3 toTarget = m_wanderTarget - m_pos;
		toTarget.y = 0.0f;
		float dist = toTarget.length();

		//仮の目的地に到着した、または目的地変更時間が来たら新しい目的地を設定
		if (dist < m_arriveDistance || m_searchChangeTime <= 0.0f || IsBlockedByWall(m_pos, m_wanderTarget))
		{
			m_searchChangeTime = 3.0f;//２秒ごとに目的地を変更する
			bool found = false;

			for (int tryCount = 0; tryCount < 5; tryCount++)
			{
				//最後に見失った位を中心に、半径4.0fの範囲でランダムな目的地を設定する
				float randAngle = (float)(rand() % 360) * XM_PI / 180.0f;
				float randDist = (float)(rand() % 4) + 1.0f; // 1.0f～4.0fの範囲でランダムな距離
				
				//ランダムに目的地を設定
				Vec3 newTarget = m_lastPlayerPos + Vec3(sinf(randAngle), 0.0f, cosf(randAngle)) * randDist;

				//壁に遮られていないか確認
				if (!IsBlockedByWall(m_pos, newTarget))
				{
					m_wanderTarget = newTarget;
					found = true;
					break;
				}
			}

			//もし5回試しても壁に遮られている場合は、目的地を真後ろの設定
			if(!found)
			{
				m_wanderTarget = m_pos - m_forward * 1.5f;
			}

			toTarget = m_wanderTarget - m_pos;
			toTarget.y = 0.0f;
			dist = toTarget.length();
		}

		//仮の目的地に向かってゆっくり移動
		if (dist > 0.01f)
		{
			toTarget.normalize();
			Vec3 moveDir = AvoidWall(toTarget);//壁を避ける移動ベクトルを計算

			//巡回よりゆっくり移動する
			m_pos.x += moveDir.x * m_speed * m_speedRate * delta;
			m_pos.z += moveDir.z * m_speed * m_speedRate * delta;
			m_transComp->SetPosition(m_pos);

			//見た目も進行方向へ
			TurnPlayer(m_pos + moveDir, delta);
		}
	}

	//聴覚の判定
	bool Enemy::HearPlayer(const Vec3& playerPos)
	{
		float dist = (m_pos - playerPos).length();
		if (dist < m_hearRenge)
		{
			float playerSpeed = m_player.lock()->GetPlayerSpeed();

			//Playerが走っている場合は、Enemyの向きをPlayer方向にする
			if (playerSpeed > 15.0f)
			{
				TurnPlayer(playerPos, App::GetApp()->GetElapsedTime());
				m_lastPlayerPos = playerPos;//Playerを見失ったときの座標を更新
				return true;
			}
		}
		return false;
	}

	//範囲内のチュールを探す
	bool Enemy::FindChuru()
	{
		for (auto obj : GetStage()->GetGameObjectVec())
		{
			if (obj->FindTag(L"Churu"))
			{
				Vec3 churuPos = obj->GetComponent<Transform>()->GetPosition();
				Vec3 toTarget = churuPos - m_pos;
				float dist = toTarget.length();

				if (dist < m_churuRange)
				{
					m_churuObj = obj;
					return true;
				}
			}
		}

		return false;
	}

	//チュールへ向かう関数
	void Enemy::UpdateChuru(float delta)
	{
		//到着して待機中の処理
		if (m_GetChuru)
		{
			m_churuWaitTime -= delta;
			if (m_churuWaitTime <= 0.0f)
			{
				m_GetChuru = false;

				SetNextWayPoint();
				m_state = EnemyState::Patol;
			}
			return;
		}

		//--------チュールに向かって移動中の処理----------

		auto Churu = m_churuObj.lock();
		if (!Churu)
		{
			//チュールが消えていたら巡回に戻る
			SetNextWayPoint();
			m_state = EnemyState::Patol;
			return;
		}

		Vec3 churuPos = Churu->GetComponent<Transform>()->GetPosition();
		Vec3 toTarget = churuPos - m_pos;
		float heighDiff = toTarget.y;
		toTarget.y = 0.0;
		float dist = toTarget.length();

		//チュールが一定の高さまでの場所に合ったらジャンプする
		if (heighDiff > 1.0f && heighDiff < 3.0f)
		{
			m_wantJump = true;
		}

		//到着するまでチュールに向かって移動する
		if (dist > m_arriveDistance)
		{
			toTarget.normalize();

			//チュールが目の前（壁にさえぎられていない＆2.5f以内）なら、奥の壁を避けずに直進
			Vec3 moveDir = (!IsBlockedByWall(m_pos, churuPos) && dist < 2.5f) 
				? toTarget : AvoidWall(toTarget);

			m_pos.x += moveDir.x * m_speed * delta;
			m_pos.z += moveDir.z * m_speed * delta;
			TurnPlayer(m_pos + moveDir, delta);
		}
		else
		{
			//到着したらチュールの方向を向く
			TurnPlayer(churuPos, delta);
			GetStage()->RemoveGameObject<GameObject>(Churu);
			m_churuObj.reset();

			//消去フラグと停止タイマーをセット
			m_GetChuru = true;
			m_churuWaitTime = 3.0f;
		}

	}


	//壁に遮られているか判定
	bool Enemy::IsBlockedByWall(const Vec3& startPos, const Vec3& targetPos) const
	{
		//床との誤判定を防ぐために、少し上にあげる
		Vec3 start = startPos + Vec3(0.0f, 0.5f, 0.0f);
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

		float bodyRadius = 0.5f;//体の幅（半分）
	
		//正面のチェック（中央 ＋ 左右の肩幅ぶん）
		Vec3 rightVec(moveDir.z, 0.0f, -moveDir.x);
		bool hitCenter = IsBlockedByWall(m_pos, m_pos + moveDir * feelerLength);
		bool hitLeftShoulder = IsBlockedByWall(m_pos - rightVec * bodyRadius, m_pos + moveDir * feelerLength);
		bool hitRightShoulder = IsBlockedByWall(m_pos + rightVec * bodyRadius, m_pos + moveDir * feelerLength);

		//三本とも壁に当たらなければそのまま直進
		if (!hitCenter && !hitLeftShoulder && !hitRightShoulder)
		{
			return moveDir;
		}

		//壁がない方向を探す
		float angle = atan2(moveDir.x, moveDir.z);
		float angles[] = {45.0f,90.0f,135.0f};
		for (float deg : angles)
		{
			float rad = XMConvertToRadians(deg);//30°ずつずらす
			// 左方向を計算
			Vec3 leftDir(sinf(angle - rad), 0.0f, cosf(angle - rad));
			if (!IsBlockedByWall(m_pos + leftDir * 0.55f, start + leftDir * feelerLength))
			{
				return leftDir;//左が空いていたら左に進む
			}

			// 右方向を計算
			Vec3 rightDir(sinf(angle + rad), 0.0f, cosf(angle + rad));
			if (!IsBlockedByWall(m_pos + rightDir * 0.55f, start + rightDir * feelerLength))
			{
				return rightDir;//右が空いていたら右に進む
			}
		}

		// どの方向も壁に遮られている場合は、後ろに下がる
		return -moveDir;
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

	//一番近いウェイポイントを探して設定する
	bool Enemy::SetNextWayPoint()
	{
		auto mgr = m_wpManager.lock();
		if (!mgr)
		{
			//m_wpManagerが空だったら再取得する
			mgr = GetStage()->GetSharedGameObject<WayPointManager>(L"WayPointManager", false);
			m_wpManager = mgr;
			if (!mgr)return false;
		}

		size_t count = mgr->GetCount(m_groupId);
		if (count == 0) return false;

		float minDist = FLT_MAX;
		bool found = false;	//ウェイポイントが見つかったかどうかのフラグ

		for (size_t i = 0; i < count; i++)
		{
			Vec3 wpPos = mgr->GetPoint(m_groupId, i);
			if (wpPos.y - m_pos.y > 2.0f) continue; // 高すぎるウェイポイントは無視する
			
			// 壁に遮られている場合は無視する
			if (IsBlockedByWall(m_pos, wpPos)) continue; 

			float dist = (wpPos - m_pos).length();

			//壁に遮られていないウェイポイントを探す
			if (dist < minDist)
			{
				//直接見えるWayPointが見つかったら足跡を消す
				minDist = dist;
				m_currentIndex = i;
				found = true;
			}
		}

		//見つかった場合はm_currentIndexが更新される
		return found;
	}

	// 当たり判定
	void Enemy::OnCollisionEnter(shared_ptr<GameObject>& Other)
	{
		if (Other->FindTag(L"Floor") || Other->FindTag(L"Wall"))
		{
			if(m_velocityY <= 0.0f)
			{
				m_isGround = true;
			}
		}
	}

	void Enemy::OnCollisionExit(shared_ptr<GameObject>& Other)
	{
		if (Other->FindTag(L"Floor"))
		{
			if (m_velocityY <= 0.0f)
			{
				m_isGround = true;
			}
		}
	}

};