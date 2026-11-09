/*!
@file JoltStatic.cpp
@brief ステージ上の動かない物(床・壁)をJoltの静的ボディとして管理する実体
*/

#include "stdafx.h"
#include "Project.h"
#include "JoltLayers.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyFilter.h>
#include "JoltManager.h"

namespace basecross {

	namespace
	{
		// Joltの静的ボディにする対象かどうか。対象ならtrueを返し、種類をkindに入れる
		bool GetStaticBodyKind(const shared_ptr<GameObject>& obj, JoltStatic::BodyKind& kind)
		{
			if (dynamic_pointer_cast<Ground>(obj) || dynamic_pointer_cast<Floor>(obj) || dynamic_pointer_cast<Wall>(obj))
			{
				kind = JoltStatic::Normal;
				return true;
			}
			if (dynamic_pointer_cast<InvisibleWall>(obj))
			{
				kind = JoltStatic::Invisible;
				return true;
			}
			return false;
		}

		// 見えない壁(Invisible)だけを無視するボディフィルター(カメラなどが見えない壁を通り抜けるため)
		class IgnoreInvisibleBodyFilter : public JPH::BodyFilter
		{
		public:
			virtual bool ShouldCollideLocked(const JPH::Body& inBody) const override
			{
				return inBody.GetUserData() != (uint64_t)JoltStatic::Invisible;
			}
		};
	}

	// 登録済みのボディを全部消して、objsの中の床・壁から作り直す
	void JoltStatic::Rebuild(const vector<shared_ptr<GameObject>>& objs, JPH::PhysicsSystem& system)
	{
		// 前回のボディを消す
		Clear(system);

		JPH::BodyInterface& bodyInterface = system.GetBodyInterface();

		for (auto& obj : objs)
		{
			BodyKind kind = Normal;
			if (!GetStaticBodyKind(obj, kind))
			{
				continue;
			}

			// Transformから位置・向き・大きさを取る
			auto trans = obj->GetComponent<Transform>();
			Vec3 pos = trans->GetPosition();
			Quat q = trans->GetQuaternion();
			Vec3 scale = trans->GetScale();

			// DEFAULT_CUBEは1辺が1なので、スケールの半分が箱の半径になる(0だとJoltが受け付けないので最小値を設ける)
			const float kMinHalf = 0.005f;
			JPH::Vec3 half(
				(std::max)(fabsf(scale.x) * 0.5f, kMinHalf),
				(std::max)(fabsf(scale.y) * 0.5f, kMinHalf),
				(std::max)(fabsf(scale.z) * 0.5f, kMinHalf));
			JPH::Quat rot = JPH::Quat(q.x, q.y, q.z, q.w).Normalized();

			JPH::BodyCreationSettings settings(
				new JPH::BoxShape(half),
				JPH::RVec3(pos.x, pos.y, pos.z),
				rot,
				JPH::EMotionType::Static,
				Layers::NON_MOVING);
			settings.mUserData = (uint64_t)kind;

			JPH::BodyID id = bodyInterface.CreateAndAddBody(settings, JPH::EActivation::DontActivate);
			if (!id.IsInvalid())
			{
				m_bodyIDs.push_back(id);
			}
		}

		// まとめて追加したあとに呼ぶと、判定の検索が速くなる
		if (!m_bodyIDs.empty())
		{
			system.OptimizeBroadPhase();
		}
	}

	// 登録済みのボディを全部消す
	void JoltStatic::Clear(JPH::PhysicsSystem& system)
	{
		if (m_bodyIDs.empty())
		{
			return;
		}

		JPH::BodyInterface& bodyInterface = system.GetBodyInterface();
		bodyInterface.RemoveBodies(m_bodyIDs.data(), (int)m_bodyIDs.size());
		bodyInterface.DestroyBodies(m_bodyIDs.data(), (int)m_bodyIDs.size());
		m_bodyIDs.clear();
	}

	// 点startから向きdirへ半径radiusの球を飛ばし、壁・床にぶつかるまでに進める距離を返す
	// (maxDistanceまで何にも当たらなければmaxDistanceを返す。dirは長さ1にしておくこと。見えない壁は無視する)
	float JoltStatic::GetSphereCastDistance(const Vec3& start, const Vec3& dir, float maxDistance, float radius)
	{
		JPH::PhysicsSystem* system = JoltManager::GetActiveSystem();
		if (!system || maxDistance <= 0.0f)
		{
			return maxDistance;
		}

		JPH::RefConst<JPH::Shape> sphere = new JPH::SphereShape(radius);
		JPH::RShapeCast shapeCast = JPH::RShapeCast::sFromWorldTransform(
			sphere,
			JPH::Vec3::sReplicate(1.0f),
			JPH::RMat44::sTranslation(JPH::RVec3(start.x, start.y, start.z)),
			JPH::Vec3(dir.x, dir.y, dir.z) * maxDistance
		);

		JPH::ShapeCastSettings settings;

		// 動くもの(Layers::MOVING)が当たる相手、という条件で調べる
		JPH::DefaultBroadPhaseLayerFilter broadPhaseFilter(system->GetObjectVsBroadPhaseLayerFilter(), Layers::MOVING);
		JPH::DefaultObjectLayerFilter objectFilter(system->GetObjectLayerPairFilter(), Layers::MOVING);
		IgnoreInvisibleBodyFilter bodyFilter;

		// いちばん近くでぶつかった結果だけ受け取る
		JPH::ClosestHitCollisionCollector<JPH::CastShapeCollector> collector;
		system->GetNarrowPhaseQuery().CastShape(shapeCast, settings, JPH::RVec3::sZero(), collector, broadPhaseFilter, objectFilter, bodyFilter);

		if (!collector.HadHit())
		{
			return maxDistance;
		}
		return collector.mHit.mFraction * maxDistance;
	}

}
//end basecross