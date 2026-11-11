/*!
@file Churu.h
@brief チュールなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class Churu : public GameObject;
	//--------------------------------------------------------------------------------------
	class Churu : public GameObject
	{
	private:
		Vec3 m_Scale;
		Vec3 m_Rotation;
		Vec3 m_Position;

	public:
		// 構築と破棄
		Churu(const shared_ptr<Stage>& stage,
			const Vec3& Scale,
			const Vec3& Rotation,
			const Vec3& Position
		) :
			GameObject(stage),
			m_Scale(Scale),
			m_Rotation(Rotation),
			m_Position(Position)
		{}
		virtual ~Churu()
		{}


		virtual void OnCreate() override; // 初期化
		//virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画
	};

}
//end basecross
