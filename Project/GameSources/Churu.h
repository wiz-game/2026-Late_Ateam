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

		bool m_isFlying = false;       // 空中にあるかどうか
		bool m_isThrown = false;       // 投げられたかどうか
		Vec3 m_velocity = Vec3(0.0f);  // 飛んでいる間の速度

		static constexpr float KILL_HEIGHT = -30.0f; // これより下に落ちたら消す
		static constexpr float SKIN = 0.005f;        // 壁・床との間に空けるすき間(めり込み・張り付き防止)

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
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

		// 投げる(velocityで飛び始め、床に着くと止まる)
		void Throw(const Vec3& velocity);

		// 飛んでいる最中か
		bool GetIsFlying() const { return m_isFlying; }

		// プレイヤーに投げられたチュールか(数の上限を数えるのに使う)
		bool GetIsThrown() const { return m_isThrown; }
	};

}
//end basecross
