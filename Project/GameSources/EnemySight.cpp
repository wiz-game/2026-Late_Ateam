#include "stdafx.h"
#include "EnemySight.h"

namespace basecross 
{
	void EnemySight::OnCreate()
	{
		m_transComp = AddComponent<Transform>();

		//頂点（座標と色のデータ）と、インデックス（度の頂点をつないで三角形にするかの番号）
		std::vector<VertexPositionColor> vertices;
		std::vector<uint16_t> indices;

		Col4 deepRed(1.0f, 0.0f, 0.0f, 0.55f);//両眼視界：濃い赤
		Col4 lightRed(1.0f, 0.3f, 0.3f, 0.25f);//単眼視界：薄い赤

		float halfInner = m_innerAngle * 0.5f;//両眼の半分
		float halfOuter = (std::min)(m_outerAngle * 0.5f, 180.0f);//単眼の半分

		//扇形を作るラムダ関数
		auto addFan = [&](float startDeg, float endDeg, int div, const Col4& color)
			{
				//扇形の中心の頂点を追加
				uint16_t centerIdx = (uint16_t)vertices.size();
				vertices.push_back(VertexPositionColor(Vec3(0.0f, 0.0f, 0.0f), color));

				//扇形の外周の頂点を追加
				for (int i = 0; i <= div; ++i)
				{
					float t = (float)i / (float)div;// 0.0f～1.0fの範囲で割合を計算
					float deg = startDeg + (endDeg - startDeg) * t;//割合に応じて角度を計算
					float rad = XMConvertToRadians(deg);//角度をラジアンに変換

					//扇形の半径を1.0fにする
					Vec3 pos(sinf(rad), 0.0f, cosf(rad));
					vertices.push_back(VertexPositionColor(pos, color));
				}

				//扇形のインデックスを追加
				for (int i = 0; i < div; ++i)
				{
					indices.push_back(centerIdx);//扇形の中心の頂点
					indices.push_back((uint16_t)(centerIdx + i + 1));//扇形の外周の頂点(i番目)
					indices.push_back((uint16_t)(centerIdx + i + 2));//扇形の外周の頂点(i+1番目)
				}
			};

		//addFan（開始角度、終了角度、分割数、色）
		addFan(-halfOuter, -halfInner, 10, lightRed);//左の単眼視界（薄い赤）
		addFan(-halfInner,  halfInner, 20, deepRed);//中央の両眼視界（濃い赤）
		addFan( halfInner,  halfOuter, 10, lightRed);//右の単眼視界（薄い赤）

		//頂点とインデックスからメッシュを作成
		auto mesh = MeshResource::CreateMeshResource(vertices, indices, false);
		m_drawComp = AddComponent<PCStaticDraw>();
		m_drawComp->SetMeshResource(mesh);

		m_drawComp->SetRasterizerState(RasterizerState::CullNone);
		m_drawComp->SetBlendState(BlendState::AlphaBlend);
		//m_drawComp->SetDepthStencilState(DepthStencilState::Read);
		//m_drawComp->SetDiffuse(deepRed);

		//透過を有効にする
		SetAlphaActive(true);
	}

	void EnemySight::OnUpdate()
	{
		auto enemy = m_enemy.lock();
		if (!enemy)
		{
			//Enemyが削除されていたら、EnemySightも削除する
			GetStage()->RemoveGameObject<EnemySight>(GetThis<EnemySight>());
			return;
		}

		//Enemyの位置と向きに合わせる
		auto enemyTrans = enemy->GetComponent<Transform>();
		Vec3 pos = enemyTrans->GetPosition();
		Vec3 forward = enemyTrans->GetForward();
		Vec3 rot = enemyTrans->GetRotation();

		Vec3 sightPos = pos + forward * 0.5f;//Enemyの頭の位置に合わせる
		sightPos.y = pos.y + 1.0f;

		m_transComp->SetPosition(sightPos);
		m_transComp->SetRotation(rot);

		//Enemyの視界の範囲に合わせてスケールを変更する
		float range = enemy->GetEyeRange();
		m_transComp->SetScale(Vec3(range, 1.0f, range));
	}
}