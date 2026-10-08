/*!
@file Arrow.h
@brief エディター中の矢印
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class Arrow : public GameObject;
	//--------------------------------------------------------------------------------------
	class Arrow : public GameObject
	{
		shared_ptr<GameObject> m_arrows[3]; // 0 = x, 1 = y, 2 = z,
		int m_hitArrowNum;
	public:
		// 構築と破棄
		Arrow(const shared_ptr<Stage>& stage) :
			GameObject(stage)
		{}
		virtual ~Arrow()
		{}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		virtual void OnDraw() override; // 描画
		void CheckHitAnyArrow(const Vec3& start, const Vec3& end);
		int GetPickNum() { return m_hitArrowNum; }
		void SetArrowPosition(const Vec3& pos);

		void CreateScaleArrowMesh(const shared_ptr<GameObject>& obj);
	};

}
//end basecross
