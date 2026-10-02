/*!
@file Arrow.cpp
@brief エディター中の矢印の実体
*/

#include "stdafx.h"
#include "Project.h"
#include "Arrow.h"

namespace basecross {

	//初期化
	void Arrow::OnCreate()
	{
		Vec3 pos = GetComponent<Transform>()->GetPosition();
		pos.y += 1.0f;
		m_arrows[0] = ObjectFactory::Create<GameObject>(GetStage());
		CreateScaleArrowMesh(m_arrows[0]);
		auto draw = m_arrows[0]->GetComponent<PCTStaticDraw>();
		draw->SetDiffuse(Col4(1.0f, 0.0f, 0.0f, 1.0f));
		draw->SetDepthStencilState(DepthStencilState::None);
		auto trans = m_arrows[0]->GetComponent<Transform>();
		trans->SetPosition(pos);

		m_arrows[1] = ObjectFactory::Create<GameObject>(GetStage());
		CreateScaleArrowMesh(m_arrows[1]);
		draw = m_arrows[1]->GetComponent<PCTStaticDraw>();
		draw->SetDiffuse(Col4(0.0f, 1.0f, 0.0f, 1.0f));
		draw->SetDepthStencilState(DepthStencilState::None);
		trans = m_arrows[1]->GetComponent<Transform>();
		Vec3 rot = Vec3(0.0f, 0.0f, XM_PIDIV2);
		trans->SetPosition(pos);
		trans->SetRotation(rot);


		m_arrows[2] = ObjectFactory::Create<GameObject>(GetStage());
		CreateScaleArrowMesh(m_arrows[2]);
		draw = m_arrows[2]->GetComponent<PCTStaticDraw>();
		draw->SetDiffuse(Col4(0.0f, 0.0f, 1.0f, 1.0f));
		draw->SetDepthStencilState(DepthStencilState::None);
		trans = m_arrows[2]->GetComponent<Transform>();
		rot = Vec3(0.0f, -XM_PIDIV2, 0.0f);
		trans->SetPosition(pos);
		trans->SetRotation(rot);

		SetDrawLayer(5);
	}

	void Arrow::OnUpdate()
	{
		// 現在のカメラの距離から矢印のサイズを変える
		auto camera = GetStage()->GetView()->GetTargetCamera();
		auto pos = GetComponent<Transform>()->GetPosition();
		auto eye = camera->GetEye();
		auto dis = pos - eye;
		float scale = dis.length() * 0.1f;
		for (int i = 0; i < 3; i++)
		{
			m_arrows[i]->GetComponent<Transform>()->SetScale(Vec3(scale));
		}
	}

	void Arrow::OnDraw()
	{
		for (const auto& obj : m_arrows)
		{
			obj->OnDraw();
		}
	}
	void Arrow::CheckHitAnyArrow(const Vec3& start, const Vec3& end)
	{
		for (int i = 0; i < 3; i++)
		{
			auto drawComp = m_arrows[i]->GetComponent<PCTStaticDraw>();
			drawComp->SetEmissive(Col4(0.5f, 0.5f, 0.5f, 1.0f));
		}

		for (int i = 0; i < 3; i++)
		{
			auto drawComp = m_arrows[i]->GetComponent<PCTStaticDraw>();
			TRIANGLE triangle;
			size_t triangleIndex;
			Vec3 hitPos;
			bool isHit = false;
			isHit = drawComp->HitTestStaticMeshSegmentTriangles(start, end, hitPos, triangle, triangleIndex);
			if (isHit)
			{
				m_hitArrowNum = i;
				drawComp->SetEmissive(Col4(1.0f, 1.0f, 1.0f, 1.0f));
				return;
			}
		}
		m_hitArrowNum = -1;
	}

	void Arrow::SetArrowPosition(const Vec3& pos)
	{
		GetComponent<Transform>()->SetPosition(pos);
		m_arrows[0]->GetComponent<Transform>()->SetPosition(pos);
		m_arrows[1]->GetComponent<Transform>()->SetPosition(pos);
		m_arrows[2]->GetComponent<Transform>()->SetPosition(pos);

	}

	void Arrow::CreateScaleArrowMesh(const shared_ptr<GameObject>& obj)
	{
		std::vector<VertexPositionColorTexture> vertices;
		std::vector<uint16_t> indices;
		Col4 color = Col4(1.0f, 1.0f, 1.0f, 1.0f);
		vertices = {
			{Vec3(1.0f,    0.0f,    0.0f), color, Vec2(0, 0)}, // 0
			{Vec3(0.7f,    0.2f,    0.2f), color, Vec2(0, 0)}, // 1
			{Vec3(0.7f,   -0.2f,    0.2f), color, Vec2(0, 0)}, // 2
			{Vec3(0.7f,   -0.2f,   -0.2f), color, Vec2(0, 0)}, // 3
			{Vec3(0.7f,    0.2f,   -0.2f), color, Vec2(0, 0)}, // 4
			{Vec3(0.7f,    0.1f,    0.1f), color, Vec2(0, 0)}, // 5
			{Vec3(0.7f,   -0.1f,    0.1f), color, Vec2(0, 0)}, // 6
			{Vec3(0.7f,   -0.1f,   -0.1f), color, Vec2(0, 0)}, // 7
			{Vec3(0.7f,    0.1f,   -0.1f), color, Vec2(0, 0)}, // 8
			{Vec3(0.0f,    0.1f,    0.1f), color, Vec2(0, 0)}, // 9
			{Vec3(0.0f,   -0.1f,    0.1f), color, Vec2(0, 0)}, // 10
			{Vec3(0.0f,   -0.1f,   -0.1f), color, Vec2(0, 0)}, // 11
			{Vec3(0.0f,    0.1f,   -0.1f), color, Vec2(0, 0)}	// 12
		};
		indices =
		{
			 0,  1,  2,
			 0,  2,  3,
			 0,  3,  4,
			 0,  4,  1,
			 4,  3,  1,
			 3,  2,  1,
			 5,  9,  6,
			 9, 10,  6,
			 6, 10,  7,
			10, 11,  7,
			 7, 11,  8,
			11, 12,  8,
			 8, 12,  5,
			12,  9,  5
		};

		auto drawComp = obj->AddComponent<PCTStaticDraw>();
		drawComp->SetOriginalMeshUse(true); // プログラム内で自作したメッシュを使用する
		drawComp->CreateOriginalMesh(vertices, indices); // 頂点データとインデックスデータを元にメッシュを自作する
	}
}
//end basecross
