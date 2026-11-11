/*!
@file Enemy.h
@brief 敵キャラクターなど
*/

#pragma once
#include "Project.h"

namespace basecross {
	class WayPointManager;

	class Enemy : public GameObjectForEdit
	{
		std::shared_ptr<Transform> m_transComp; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<PNTStaticDraw> m_drawComp; // ドローコンポーネント
		std::weak_ptr<Player> m_player;	//Player
		std::weak_ptr<WayPointManager> m_wpManager;	//WayPointManager

		Vec3 m_pos = Vec3(10.0f, 0.0f, 0.0f);	//位置
		Vec3 m_scale = Vec3(1.0f, 1.0f, 1.0f);	//スケール
		Vec3 m_rot = Vec3(0.0f, 0.0f, 0.0f);	//回転
		Vec3 m_forward = Vec3(0.0f, 0.0f, 1.0f);//向き

		Vec3 m_spawnPos = Vec3(0.0f);   // ゲーム開始時の位置

		float m_speed = 5.0f;		// 移動の速さ

		//視界関係
		float m_EyeRenge = 20.0f;	// 視野範囲
		float m_EyeAngle = 80.0f;	// 視野角度
		float m_AlertEyeAngle = 50.0f;	//警戒視界の角度
		float m_hearRenge = 30.0f;	// 聴覚範囲

		float m_innerCos = 0.0f;
		float m_outerCos = 0.0f;

		float m_turnSpeed = 2.0f;	//回転速度

		//ジャンプ関係
		bool m_isGround = false;	//地面にいるかどうか
		float m_prevY = 0.0f;		//前フレームのY座標
		float m_velocityY = 0.0f;		//Y方向の速度
		bool m_wantJump = false;		//ジャンプしたいかどうか

		//巡回関係
		size_t m_currentIndex = 0;		//今向かっているポイントの番号
		float m_arriveDistance = 0.5f;	//到着したとみなす距離

		//追跡関係
		float m_CatchSpeed = 8.0f;	// 追跡時の速さ
		float m_CatchRenge = 1.0f;	// 捕獲範囲
		std::vector<Vec3> m_breadcrumb;	//追跡中に通った座標の履歴
		float m_breadcrumbTime = 0.0f;	//履歴を記録する間隔タイマー


		//見失った後の捜索関係
		Vec3 m_lastPlayerPos = Vec3(0.0f);	//Playerを見失ったときの座標
		Vec3 m_wanderTarget = Vec3(0.0f);	//捜索時の仮の目的地
		float m_searchTime = 0.0f;	//捜索時間
		float m_searchChangeTime = 0.0f;	//捜索時の目的地変更時間
		float m_speedRate = 0.8f;	//捜索時の速度倍率

		void InitDrawComp(); // Drawコンポーネントの初期化
		void InitTransComp(); // Transformコンポーネントの初期化

		int m_groupId = 0;	//どのウェイポイントか判定するID

		enum class EnemyState
		{
			Patol, // 巡回
			Alert, // 警戒
			Chase, // 追跡
			Search, // 捜索
			Catch, // 捕獲
		};

		EnemyState m_state = EnemyState::Patol; // 敵キャラクターの状態

	public:
		Enemy(const std::shared_ptr<Stage>& stage,
			const Vec3& scale,
			const Vec3& rot, 
			const Vec3& pos
		) :
			GameObjectForEdit(stage),
			m_pos(pos),
			m_scale(scale),
			m_rot(rot),
			m_speed(5.0f)
		{}
		virtual ~Enemy()
		{}
		virtual void OnCreate() override;
		virtual void OnUpdate() override;

		//Vec3 RotateY(const Vec3& v, float angle);//視界の描画
		bool IsPlayerInSight(const Vec3& playerPos) const;	//両眼視界（発見）の判定
		bool AlertEye(const Vec3& playerPos) const;			//単眼視界（警戒）の判定
		void TurnPlayer(const Vec3& playerPos, float delta);//Player方向を向く関数
		void ApplyRotation();	//m_forwardを見た目に反映

		//Stateごとの更新関数
		void UpdatePatol(float delta);//巡回関数
		void UpdateChase(float delta, const Vec3& playerPos);//追跡関数
		void UpdateSearch(float delta);//捜索関数

		//壁に遮られているか判定
		bool IsBlockedByWall(const Vec3& targetPos) const;
		//壁を避ける移動ベクトルを計算
		Vec3 AvoidWall(const Vec3 moveDir) const;
		//次のウェイポイントを設定する
		bool SetNextWayPoint();	//一番近いウェイポイントを探して設定する

		//聴覚の判定
		bool HearPlayer(const Vec3& playerPos);	//聴覚の判定

		// エディター接続関係
		void ResetToSpawn();               // 置いた位置・向きに戻す(エディターに入るとき)
		void ResetToSpawnTransform();     // エディターで置いた位置を出発位置にする(ゲーム開始時)

		//WayPointManagerの設定
		int GetGroupID() const
		{
			return m_groupId;
		}

		void SetGroupID(int id)
		{
			m_groupId = id;
		}

		void VelocityYUpdate(bool isJump);//Y方向の速度更新


		// 当たり判定
		void OnCollisionEnter(shared_ptr<GameObject>& Other);
		void OnCollisionExit(shared_ptr<GameObject>& Other);

	};
}
