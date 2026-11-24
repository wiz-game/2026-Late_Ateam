/*!
@file Player.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include <deque>
#include "PNTDXModelDraw.h"

namespace basecross {

	class CharacterController; 
	class Churu;

	// 使えるアイテムの種類
	enum class ItemType
	{
		None,   // 何も持っていない
		Churu,  // チュール
	};

	class Player : public GameObjectForEdit
	{
		std::shared_ptr<Transform> m_transComp; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<PNTStaticDraw> m_drawComp; // ドローコンポーネント
		//std::shared_ptr<PNTDXModelDraw> m_drawComp; // ドローコンポーネント
		std::shared_ptr<CharacterController> m_controller; // Joltで床・壁にぶつかりながら動かす

		float m_speed; // 移動の速さ
		float m_scale = 0.5f; // プレイヤーのスケール
		float m_dashRate = 2.0f; // ダッシュ時の速度倍率
		bool m_isDash = false;   // ダッシュ中か(空中では離陸したときの状態を保つ)

		bool m_isGround = false;
		bool m_canClear = false;
		bool IsJampPressed();
		bool GetIsDashPressed();

		void UseItem();
		bool GetIsItemPressed();   // アイテム使用の操作がされたか
		bool ThrowChuru();         // チュールを投げる(投げられたらtrue。数の上限などで投げられなければfalse)
		std::deque<std::weak_ptr<Churu>> m_thrownChurus; // 投げたチュールを、古い順に覚えておく

		ItemType m_currentItem = ItemType::Churu; // いま選んでいるアイテム(今はチュールだけ)
		float m_throwSpeed = 8.0f;    // チュールを投げる水平方向の速さ
		float m_throwUpSpeed = 5.0f;  // チュールを投げ上げる速さ

		static constexpr int MAX_CHURU = 5;            // 世界に同時に存在できる、投げたチュールの数
		static constexpr float ITEM_COOL_TIME = 2.0f;  // アイテムを使ったあとのクールタイム(秒)
		float m_itemCoolTime = 0.0f;                   // クールタイムの残り(秒)。0なら使える

		float m_airControl = 0.1f;

		static constexpr float COLL_RADIUS = 0.6f;  // カプセルの半径(幅は1.2)
		static constexpr float COLL_LENGTH = 1.2f;  // 円筒部分の長さ(全長は 1.2 + 0.6×2 = 2.4)
		float m_angleY = 0.0f;                      // プレイヤーの向き(下の理由で自前で保持)

		void InitDrawComp(); // Drawコンポーネントの初期化
		void InitTransComp(); // Transformコンポーネントの初期化
		void InitController(); // Jolt側のキャラクターコントローラーの初期化

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

		float GetPlayerSpeed() const { return m_velocityXZ.length(); }

		void SetPlayerUpdateActive(bool isActive);

		bool GetIsInShadow();

		// 選んでいるアイテムの切り替えと取得
		void SetCurrentItem(ItemType item) { m_currentItem = item; }
		ItemType GetCurrentItem() const { return m_currentItem; }

		// クールタイムの残り(秒)
		float GetItemCoolTime() const { return m_itemCoolTime; }

		// プレイヤーの動き
		bool NormalMove();
	};
}
//end basecross

