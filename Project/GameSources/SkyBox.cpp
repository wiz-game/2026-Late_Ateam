/*!
@file SkyBox.cpp
@brief スカイボックス実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross
{
	SkyBox::SkyBox(const shared_ptr<Stage>& stage)
		: GameObject(stage)
	{}

	SkyBox::~SkyBox()
	{}

	const map<wstring, Vec3> SkyBox::pairs = {
		{L"000_000_000", Vec3(0   ,  0,   +0.5f)},
		{L"000_090_000", Vec3(+0.5f, 0,    0)},
		{L"000_180_000", Vec3(0   ,  0,   -0.5f)},
		{L"000_270_000", Vec3(-0.5f, 0,    0)},
		{L"090_000_000", Vec3(0   , -0.5f, 0)},
		{L"270_000_000", Vec3(0   , +0.5f, 0)},
	};

	void SkyBox::OnCreate()
	{
		for (const auto& pair : pairs)
		{
			auto plane = ObjectFactory::Create<Plane>(GetStage());

			// ライティングを受けないようにするため、PTStaticDrawコンポーネントを追加
			auto planeDrawComp = plane->AddComponent<PTStaticDraw>();
			// テクスチャを設定
			planeDrawComp->SetTextureResource(pair.first);
			// Zバッファを受けないように
			planeDrawComp->SetDepthStencilState(DepthStencilState::None);
			// テクスチャの切れ目を目立たなくするため、リニアフィルタリングでクランプする
			planeDrawComp->SetSamplerState(SamplerState::LinearClamp);

			auto planeTransComp = plane->GetComponent<Transform>();
			// テクスチャの文字列を角度に変換
			// 0文字目から3文字取り出す
			float rotX = XMConvertToRadians(stof(pair.first.substr(0, 3)));
			// 4文字目から3文字取り出す
			float rotY = XMConvertToRadians(stof(pair.first.substr(4, 3)));
			// 8文字目から3文字取り出す
			float rotZ = XMConvertToRadians(stof(pair.first.substr(8, 3)));
			// 視野範囲の限界まで拡大する
			const float scale = 707.0f;
			planeTransComp->SetPosition(pair.second * scale);
			planeTransComp->SetRotation(rotX, rotY, rotZ);
			planeTransComp->SetScale(Vec3(scale));
			// スカイキューブを構成するプレーンをSkyBoxオブジェクトの子にすることで座標の管理の簡易化
			planeTransComp->SetParent(GetThis<GameObject>());

			m_planes.push_back(plane);
		}
	}

	void SkyBox::OnUpdate()
	{
		// カメラの位置にスカイボックスを移動させる
		auto transComp = GetComponent<Transform>();
		transComp->SetPosition(GetStage()->GetView()->GetTargetCamera()->GetEye());
	}

	void SkyBox::OnDraw()
	{
		// 管理しているすべての面を描画する
		for (auto& plane : m_planes)
		{
			plane->OnDraw();
		}
	}


	Plane::Plane(const shared_ptr<Stage>& stage)
		: GameObject(stage)
	{}

	Plane::~Plane()
	{}

	void Plane::OnCreate()
	{
		// 板ポリを形成するための頂点データ
		vector<VertexPositionTexture> vertices = {
			{Vec3(-0.5f, +0.5f, 0.0f), Vec2(0.0f, 0.0f)},
			{Vec3(+0.5f, +0.5f, 0.0f), Vec2(1.0f, 0.0f)},
			{Vec3(-0.5f, -0.5f, 0.0f), Vec2(0.0f, 1.0f)},
			{Vec3(+0.5f, -0.5f, 0.0f), Vec2(1.0f, 1.0f)}
		};

		// 頂点インデックス
		vector<uint16_t> indices = {
			0, 1, 2, 2, 1, 3
		};

		// 上記データを適用
		auto drawComp = AddComponent<PTStaticDraw>(); // PNTだとライティングがかかるので向かない
		drawComp->SetOriginalMeshUse(true);
		drawComp->CreateOriginalMesh(vertices, indices);
	}

}