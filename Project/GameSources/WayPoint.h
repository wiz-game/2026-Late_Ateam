/*!
@file WayPoint.h
@brief WayPointなど
*/

#pragma once
#include "stdafx.h"
#include "Project.h"
namespace basecross {

	//--------------------------------------------------------------------------------------
	//	class WayPoint : public GameObjectForEdit;
	//--------------------------------------------------------------------------------------
	class WayPoint : public GameObjectForEdit
	{
	private:
		std::shared_ptr<Transform> m_transComp;
		std::shared_ptr<PNTStaticDraw> m_drawComp;

		Vec3 m_Scale;
		Vec3 m_Rotation;
		Vec3 m_Position;

		int m_groupId = 0;	//どのEnemyに付随しているか判定するID
		int m_order = 0;	//ウェイポイントの数

	public:
		// 構築と破棄
		WayPoint(const shared_ptr<Stage>& stage,
			const Vec3& Scale,
			const Vec3& Rotation,
			const Vec3& Position
		) :
			GameObjectForEdit(stage),
			m_Scale(Scale),
			m_Rotation(Rotation),
			m_Position(Position)
		{}
		virtual ~WayPoint()
		{}


		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画


		//ゲッター関数
		int GetID() const
		{
			return m_groupId;
		}
		int GetOrder() const
		{
			return m_order;
		}
		std::shared_ptr<Transform> GetTrans()const
		{
			return m_transComp;
		}
		// WayPointが表示中かどうかを判定
		bool GetIsShown() const
		{
			return m_drawComp && m_drawComp->IsDrawActive();
		}

		//セッター関数
		void SetID(int Id)
		{
			m_groupId = Id;
		}
		void SetOrder(int order)
		{
			m_order = order;
		}
		void SetDrawActive(bool draw)
		{
			m_drawComp->SetDrawActive(draw);
		}
		void SetAlpha(float alpha)
		{
			auto color = m_drawComp->GetDiffuse();
			color.w = alpha;
			m_drawComp->SetDiffuse(color);
		}
	};

}
//end basecross
