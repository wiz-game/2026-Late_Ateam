/*!
@file Player.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "PNTDXModelDraw.h"

namespace basecross {
	class Player : public GameObjectForEdit
	{
		std::shared_ptr<Transform> m_transComp; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<PNTStaticDraw> m_drawComp; // ドローコンポーネント
		//std::shared_ptr<PNTDXModelDraw> m_drawComp; // ドローコンポーネント

		float m_speed; // 移動の速さ
		float m_scale = 1.0f; // プレイヤーのスケール

		bool m_isGround = false;
		bool m_canClear = false;
		float m_prevY = 0.0f;
		bool IsJampPressed();

		float m_airControl = 0.1f;

		static constexpr float COLL_RADIUS = 0.6f;  // カプセルの半径(幅は1.2)
		static constexpr float COLL_LENGTH = 1.2f;  // 円筒部分の長さ(全長は 1.2 + 0.6×2 = 2.4)
		float m_angleY = 0.0f;                      // プレイヤーの向き(下の理由で自前で保持)

		void InitDrawComp(); // Drawコンポーネントの初期化
		void InitTransComp(); // Transformコンポーネントの初期化

	//----基本動作(WASD)-------------------------------------------------------

		Vec3 m_velocityXZ; // XZ平面の移動速度
		float m_velocityY; // Y軸方向の移動速度
		float m_cameraAngleY; // カメラのY軸方向の回転の向き
		Vec3 InputVec(); //移動操作のベクトルを返す
		void VelocityXZUpdate(const Vec3& input); // XとZの更新
		void VelocityYUpdate(bool isJump); // Yの更新

	//-------------------------------------------------------------------------

	public :
		// ステージを引数にしたコンストラクタ
		Player(const std::shared_ptr<Stage>& stage) :
			GameObjectForEdit(stage),
			m_speed(10.0f),
			m_velocityXZ(Vec3(0.0f)),
			m_velocityY(0.0f),
			m_cameraAngleY(0.0f)
		{
		}
		virtual ~Player()
		{
		}

		virtual void OnCreate() override;
		virtual void OnUpdate() override;

		// 当たり判定
		void OnCollisionEnter(shared_ptr<GameObject>& Other);	//	当たり判定
		void OnCollisionExecute(shared_ptr<GameObject>& Other);
		void OnCollisionExit(shared_ptr<GameObject>& Other);

		bool GetCanClear() const { return m_canClear; }

		void SetPlayerUpdateActive(bool isActive);

		// プレイヤーの動き
		bool NormalMove();
	};
}
//end basecross

