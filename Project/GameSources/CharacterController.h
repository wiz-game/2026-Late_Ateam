/*!
@file CharacterController.h
@brief Jolt PhysicsのCharacterVirtualを使ったキャラクターコントローラー
*/

#pragma once
#include "stdafx.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

#include "JoltLayers.h"

namespace basecross
{
	/**
	 * プレイヤーなど「入力で動くキャラクター」を、Jolt上の壁・床にぶつかりながら動かすコンポーネント。
	 *
	 * 使い方(毎フレーム):
	 *   1. SyncFromTransform()  ... Transformが外から動かされていたら、キャラクターもそこへ移す
	 *   2. GetIsOnGround()      ... 前フレームの結果として、床に立っているか
	 *   3. SetLinearVelocity()  ... 速度(重力・ジャンプ込み)を渡す
	 *   4. SetAngleY()          ... 向きを渡す
	 *   5. Move(deltaTime)      ... Joltで動かし、結果をTransformの位置に入れる
	 *
	 * キャラクターの原点はカプセルの中心(= Transformの位置)です。
	 */
	class CharacterController : public Component
	{
	public:
		struct Settings
		{
			float radius = 0.3f;                    // カプセルの半径
			float length = 1.2f;                    // カプセルの全長(両端の丸い部分を含む)
			bool isLying = true;                    // true:横に寝かせる(体の前後方向がカプセルの軸) false:立たせる
			float mass = 70.0f;                     // 質量(乗っている物を押し下げる力に使う)
			float maxSlopeAngle = 45.0f;            // 歩ける最大の坂の角度(度)
			float maxStrength = 100.0f;             // 他の物を押す力の最大値
			float predictiveContactDistance = 0.1f; // 先読みする接触距離
			float penetrationRecoverySpeed = 1.0f;  // めり込みを戻す速さ(1で1回の更新で全部戻す)
			float stepUpHeight = 0.3f;              // 登れる段差の高さ(0で段差を登らない)
			float stickToFloorDistance = 0.3f;      // 下り坂・段差で床に張り付く距離(0で張り付かない)
			JPH::ObjectLayer objectLayer = Layers::MOVING;
		};

		explicit CharacterController(const std::shared_ptr<GameObject>& owner);
		virtual ~CharacterController();

		// コピー禁止
		CharacterController(const CharacterController&) = delete;
		CharacterController& operator=(const CharacterController&) = delete;

		// キャラクターを作る(AddComponentの直後に呼ぶ。いまのTransformの位置から始まる)
		bool Initialize(const Settings& settings);

		virtual void OnCreate() override {}
		virtual void OnUpdate() override {}
		virtual void OnDraw() override {}

		// 次のMoveで使う速度(m/s)。重力やジャンプのY成分も含めて渡す
		void SetLinearVelocity(const Vec3& velocity);

		// 向き(Y軸まわりの回転・ラジアン)
		void SetAngleY(float angleY);

		// Transformの位置が外から動かされていたら、キャラクターもそこへ移す。移したらtrue
		bool SyncFromTransform();

		// 1フレーム分動かし、結果の位置をTransformに入れる
		void Move(float deltaTime);

		// 衝突を無視して瞬間移動する(速度も0に戻す)。Transformの位置も同じ場所にする
		void Teleport(const Vec3& position);

		// 状態の取得
		bool GetIsOnGround() const;   // 歩ける床の上に立っている
		bool GetIsSupported() const;  // 何かの上に乗っている(急な坂も含む)
		Vec3 GetGroundNormal() const; // 立っている面の法線
		Vec3 GetPosition() const;     // キャラクターの位置
		Vec3 GetLinearVelocity() const; // 直近の更新で使った速度

	private:
		// いまの位置のまわりの接触を取り直す(瞬間移動のあとなどに使う)
		void RefreshContacts();

		JPH::Ref<JPH::CharacterVirtual> m_character;
		Settings m_settings;
		JPH::PhysicsSystem* m_pPhysicsSystem = nullptr;
		JPH::TempAllocatorMalloc m_tempAllocator;

		Vec3 m_velocity = Vec3(0.0f);
		float m_angleY = 0.0f;
	};
}
//end basecross
