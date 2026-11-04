/*!
@file CharacterController.cpp
@brief Jolt PhysicsのCharacterVirtualを使ったキャラクターコントローラー実体
*/

#include "stdafx.h"
#include "CharacterController.h"
#include "JoltManager.h"

#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

namespace basecross
{
	CharacterController::CharacterController(const shared_ptr<GameObject>& owner) :
		Component(owner)
	{}

	CharacterController::~CharacterController()
	{
		m_character = nullptr;
	}

	bool CharacterController::Initialize(const Settings& settings)
	{
		m_settings = settings;

		m_pPhysicsSystem = JoltManager::GetActiveSystem();
		if (!m_pPhysicsSystem)
		{
			return false;
		}

		auto transComp = GetGameObject()->GetComponent<Transform>();
		Vec3 pos = transComp->GetPosition();

		// カプセル形状(Joltのカプセルは「円筒部分の長さの半分」と「半径」で作る)
		float halfCylinder = (std::max)(m_settings.length * 0.5f - m_settings.radius, 0.01f);
		JPH::RefConst<JPH::Shape> capsule = new JPH::CapsuleShape(halfCylinder, m_settings.radius);

		// 床として扱う面の高さ(原点=カプセルの中心から見て、この高さより下の接触だけ床として扱う)
		float supportHeight = 0.0f;

		JPH::RefConst<JPH::Shape> shape = capsule;
		if (m_settings.isLying)
		{
			// カプセルは標準でY軸方向に伸びているので、X軸まわりに90度倒してZ軸方向(体の前後)に向ける
			shape = JPH::RotatedTranslatedShapeSettings(
				JPH::Vec3::sZero(),
				JPH::Quat::sRotation(JPH::Vec3::sAxisX(), JPH::DegreesToRadians(90.0f)),
				capsule.GetPtr()
			).Create().Get();
		}
		else
		{
			// 立っているときは、下の丸い部分の中心より下を床として扱う
			supportHeight = -halfCylinder;
		}

		JPH::CharacterVirtualSettings characterSettings;
		characterSettings.mShape = shape;
		characterSettings.mUp = JPH::Vec3::sAxisY();
		characterSettings.mMass = m_settings.mass;
		characterSettings.mMaxSlopeAngle = JPH::DegreesToRadians(m_settings.maxSlopeAngle);
		characterSettings.mMaxStrength = m_settings.maxStrength;
		characterSettings.mPredictiveContactDistance = m_settings.predictiveContactDistance;
		characterSettings.mPenetrationRecoverySpeed = m_settings.penetrationRecoverySpeed;
		characterSettings.mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), -supportHeight);

		m_character = new JPH::CharacterVirtual(
			&characterSettings,
			JPH::RVec3(pos.x, pos.y, pos.z),
			JPH::Quat::sRotation(JPH::Vec3::sAxisY(), m_angleY),
			m_pPhysicsSystem
		);

		return true;
	}

	void CharacterController::SetLinearVelocity(const Vec3& velocity)
	{
		m_velocity = velocity;
	}

	void CharacterController::SetAngleY(float angleY)
	{
		m_angleY = angleY;
	}

	bool CharacterController::SyncFromTransform()
	{
		if (!m_character)
		{
			return false;
		}

		Vec3 transPos = GetGameObject()->GetComponent<Transform>()->GetPosition();
		Vec3 charPos = GetPosition();

		// 1mm以上ずれていたら、Transformが外から動かされたとみなす
		if ((transPos - charPos).lengthSqr() < 1.0e-6f)
		{
			return false;
		}

		Teleport(transPos);
		return true;
	}

	void CharacterController::Move(float deltaTime)
	{
		if (!m_character || !m_pPhysicsSystem || deltaTime <= 0.0f)
		{
			return;
		}

		m_character->SetRotation(JPH::Quat::sRotation(JPH::Vec3::sAxisY(), m_angleY));
		m_character->SetLinearVelocity(JPH::Vec3(m_velocity.x, m_velocity.y, m_velocity.z));

		// 衝突フィルター(このキャラクターのレイヤーが当たる相手だけを調べる)
		JPH::DefaultBroadPhaseLayerFilter broadPhaseFilter(
			m_pPhysicsSystem->GetObjectVsBroadPhaseLayerFilter(), m_settings.objectLayer);
		JPH::DefaultObjectLayerFilter objectFilter(
			m_pPhysicsSystem->GetObjectLayerPairFilter(), m_settings.objectLayer);
		JPH::BodyFilter bodyFilter;
		JPH::ShapeFilter shapeFilter;

		// 段差を登る・下り坂で床に張り付く、までまとめて行う更新
		JPH::CharacterVirtual::ExtendedUpdateSettings updateSettings;
		updateSettings.mStickToFloorStepDown = JPH::Vec3(0.0f, -m_settings.stickToFloorDistance, 0.0f);
		updateSettings.mWalkStairsStepUp = JPH::Vec3(0.0f, m_settings.stepUpHeight, 0.0f);

		m_character->ExtendedUpdate(
			deltaTime,
			m_pPhysicsSystem->GetGravity(),
			updateSettings,
			broadPhaseFilter,
			objectFilter,
			bodyFilter,
			shapeFilter,
			m_tempAllocator
		);

		// 結果をTransformへ
		GetGameObject()->GetComponent<Transform>()->SetPosition(GetPosition());
	}

	void CharacterController::Teleport(const Vec3& position)
	{
		if (!m_character)
		{
			return;
		}

		m_character->SetPosition(JPH::RVec3(position.x, position.y, position.z));
		m_character->SetLinearVelocity(JPH::Vec3::sZero());
		m_velocity = Vec3(0.0f);

		GetGameObject()->GetComponent<Transform>()->SetPosition(position);

		// 新しい場所のまわりの接触を取り直す
		RefreshContacts();
	}

	void CharacterController::RefreshContacts()
	{
		if (!m_character || !m_pPhysicsSystem)
		{
			return;
		}

		JPH::DefaultBroadPhaseLayerFilter broadPhaseFilter(
			m_pPhysicsSystem->GetObjectVsBroadPhaseLayerFilter(), m_settings.objectLayer);
		JPH::DefaultObjectLayerFilter objectFilter(
			m_pPhysicsSystem->GetObjectLayerPairFilter(), m_settings.objectLayer);
		JPH::BodyFilter bodyFilter;
		JPH::ShapeFilter shapeFilter;

		m_character->RefreshContacts(broadPhaseFilter, objectFilter, bodyFilter, shapeFilter, m_tempAllocator);
	}

	bool CharacterController::GetIsOnGround() const
	{
		if (!m_character)
		{
			return false;
		}
		return m_character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround;
	}

	bool CharacterController::GetIsSupported() const
	{
		if (!m_character)
		{
			return false;
		}
		return m_character->IsSupported();
	}

	Vec3 CharacterController::GetGroundNormal() const
	{
		if (!m_character)
		{
			return Vec3(0.0f, 1.0f, 0.0f);
		}
		JPH::Vec3 normal = m_character->GetGroundNormal();
		return Vec3(normal.GetX(), normal.GetY(), normal.GetZ());
	}

	Vec3 CharacterController::GetPosition() const
	{
		if (!m_character)
		{
			return Vec3(0.0f);
		}
		JPH::RVec3 pos = m_character->GetPosition();
		return Vec3(
			static_cast<float>(pos.GetX()),
			static_cast<float>(pos.GetY()),
			static_cast<float>(pos.GetZ())
		);
	}

	Vec3 CharacterController::GetLinearVelocity() const
	{
		if (!m_character)
		{
			return Vec3(0.0f);
		}
		JPH::Vec3 vel = m_character->GetLinearVelocity();
		return Vec3(vel.GetX(), vel.GetY(), vel.GetZ());
	}
}
//end basecross
