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
		// ステージを引数にしたコンストラクタ【必須】
		Player(const std::shared_ptr<Stage>& stage) :
			GameObjectForEdit(stage),
			m_speed(1.0f),
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

		// プレイヤーの動き
		bool NormalMove();
	};
}
//end basecross

