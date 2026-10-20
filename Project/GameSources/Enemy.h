/*!
@file Enemy.h
@brief 敵キャラクターなど
*/

#pragma once
#include "Project.h"

namespace basecross {
	class Enemy : public GameObject
	{
		std::shared_ptr<Transform> m_transComp; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<PNTStaticDraw> m_drawComp; // ドローコンポーネント
		std::weak_ptr<Player> m_player;	//Player

		Vec3 m_pos = Vec3(10.0f, 0.0f, 0.0f);	//位置
		Vec3 m_scale = Vec3(1.0f, 1.0f, 1.0f);	//スケール
		Vec3 m_rot = Vec3(0.0f, 0.0f, 0.0f);	//回転
		Vec3 m_forward = Vec3(0.0f, 0.0f, 1.0f);//向き

		float m_speed = 5.0f;		// 移動の速さ

		float m_EyeRenge = 10.0f;	// 視野範囲
		float m_EyeAngle = 60.0f;	// 視野角度

		float m_CatchSpeed = 8.0f;	// 追跡時の速さ
		float m_CatchRenge = 1.0f;	// 捕獲範囲

		std::vector<Vec3> m_patrolPositions; //巡回ポイント
		size_t m_currentIndex = 0;		//今向かっているポイントの番号
		float m_arriveDistance = 0.5f;	//到着したとみなす距離

		void InitDrawComp(); // Drawコンポーネントの初期化
		void InitTransComp(); // Transformコンポーネントの初期化

		enum class EnemyState
		{
			Patol, // 巡回
			Alert, // 警戒
			Chase, // 追跡
			Catch, // 捕獲
		};

		EnemyState m_state = EnemyState::Patol; // 敵キャラクターの状態

	public:
		Enemy(const std::shared_ptr<Stage>& stage) :
			GameObject(stage),
			m_speed(5.0f)
		{}
		virtual ~Enemy()
		{}
		virtual void OnCreate() override;
		virtual void OnUpdate() override;

		Vec3 RotateY(const Vec3& v, float angle);//視界の描画
		bool IsPlayerInSight(const Vec3& playerPos) const;//視界の判定

		void UpdatePatol(float delta);//巡回関数
		void UpdateChase(float delta, const Vec3& playerPos);//追跡関数

		void SetEnemyPoint(const std::vector<Vec3>& points)
		{
			m_patrolPositions = points;
			m_currentIndex = 0;
			//if (!points.empty()) m_pos = points[0];
			//m_transComp->SetPosition(m_pos);
		}
	};
}
