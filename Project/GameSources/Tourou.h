/*!
@file Tourou.h
@brief 灯籠など
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class Tourou : public GameObjectForEdit;
	//--------------------------------------------------------------------------------------
	class Tourou : public GameObjectForEdit
	{
	private:
		Vec3 m_Scale;
		Vec3 m_Rotation;
		Vec3 m_Position;

	public:
		// 構築と破棄
		Tourou(const shared_ptr<Stage>& stage,
			const Vec3& Scale,
			const Vec3& Rotation,
			const Vec3& Position
		) :
			GameObjectForEdit(stage),
			m_Scale(Scale),
			m_Rotation(Rotation),
			m_Position(Position)
		{}
		virtual ~Tourou()
		{}


		virtual void OnCreate() override; // 初期化
		//virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画
	};

}
//end basecross
