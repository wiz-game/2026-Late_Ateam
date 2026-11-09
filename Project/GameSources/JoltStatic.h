/*!
@file JoltStatic.h
@brief ステージ上の動かない物(床・壁)をJoltの静的ボディとして管理する
*/

#pragma once
#include "stdafx.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace basecross {

	class JoltStatic
	{
	public:
		// ボディに持たせる種類の目印
		enum BodyKind : uint64_t
		{
			Normal = 0,    // 通常の床・壁
			Invisible = 1  // 見えない壁
		};

		// 登録済みのボディを全部消して、objsの中の床・壁から作り直す
		void Rebuild(const vector<shared_ptr<GameObject>>& objs, JPH::PhysicsSystem& system);

		// 登録済みのボディを全部消す
		void Clear(JPH::PhysicsSystem& system);

		// 登録されているボディの数
		size_t GetBodyCount() const
		{
			return m_bodyIDs.size();
		}

	private:
		std::vector<JPH::BodyID> m_bodyIDs;
	};

}
//end basecross