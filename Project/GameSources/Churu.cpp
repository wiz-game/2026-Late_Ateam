/*!
@file Churu.cpp
@brief チュールなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void Churu::OnCreate()
	{
		AddTag(L"Churu");

		auto ptrTransform = GetComponent<Transform>();
		ptrTransform->SetScale(m_Scale);
		ptrTransform->SetRotation(m_Rotation);
		ptrTransform->SetPosition(m_Position);

		auto ptrDraw = AddComponent<PNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		ptrDraw->SetEmissive(Col4(1.0f));
		//ptrDraw->SetBlendState(BlendState::Additive);

		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetFixed(true);
		//ptrColl->SetDrawActive(true);

	}


	// 投げられている間の動き
	void Churu::OnUpdate()
	{
		// 置かれているだけのときは何もしない
		if (!m_isFlying)
		{
			return;
		}

		float delta = App::GetApp()->GetElapsedTime();
		auto ptrTransform = GetComponent<Transform>();
		Vec3 pos = ptrTransform->GetPosition();

		// 立方体の内側にぴったり入る球で、壁・床との当たりを調べる
		float radius = m_Scale.x * 0.5f;

		// 重力
		m_velocity.y += -9.8f * delta;

		// 縦(落下・上昇)を先に動かす
		float moveY = m_velocity.y * delta;
		if (fabsf(moveY) > 0.00001f)
		{
			Vec3 dirY(0.0f, moveY > 0.0f ? 1.0f : -1.0f, 0.0f);
			float length = fabsf(moveY);
			float hit = JoltStatic::GetSphereCastDistance(pos, dirY, length + SKIN, radius);
			if (hit < length + SKIN)
			{
				// 当たったところまで進める
				pos += dirY * (std::max)(hit - SKIN, 0.0f);
				if (moveY < 0.0f)
				{
					// 下に当たった = 着地。ここで止まる
					m_isFlying = false;
					m_velocity = Vec3(0.0f);
				}
				else
				{
					// 上に当たった = 天井。上向きの速度だけ消して落ちてくる
					m_velocity.y = 0.0f;
				}
			}
			else
			{
				pos += dirY * length;
			}
		}

		// 横を動かす(壁に当たったら横の速度を消して、そのまま落ちる)
		if (m_isFlying)
		{
			Vec3 moveXZ(m_velocity.x * delta, 0.0f, m_velocity.z * delta);
			float length = moveXZ.length();
			if (length > 0.00001f)
			{
				Vec3 dirXZ = moveXZ.normalize();
				float hit = JoltStatic::GetSphereCastDistance(pos, dirXZ, length + SKIN, radius);
				if (hit < length + SKIN)
				{
					pos += dirXZ * (std::max)(hit - SKIN, 0.0f);
					m_velocity.x = 0.0f;
					m_velocity.z = 0.0f;
				}
				else
				{
					pos += dirXZ * length;
				}
			}
		}

		ptrTransform->SetPosition(pos);

		// 床の外に落ちたら消す
		if (pos.y < KILL_HEIGHT)
		{
			m_isFlying = false;
			GetStage()->RemoveGameObject<Churu>(GetThis<Churu>());
		}
	}

	// 投げる
	void Churu::Throw(const Vec3& velocity)
	{
		m_velocity = velocity;
		m_isFlying = true;
	}

}
//end basecross
