/*!
@file ClearStage.cpp
@brief クリアステージ実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	クリアステージクラス実体
	//--------------------------------------------------------------------------------------

	//ビューとライトの作成
	void ClearStage::CreateViewLight() {
		// カメラの設定
		auto camera = ObjectFactory::Create<Camera>();
		camera->SetEye(Vec3(0.0f, 8.0f, -8.0f));
		camera->SetAt(Vec3(0.0f, 0.0f, 0.0f));

		// ビューにカメラを設定
		auto view = CreateView<SingleView>();
		view->SetCamera(camera);

		//マルチライトの作成
		auto light = CreateLight<MultiLight>();
		light->SetDefaultLighting(); //デフォルトのライティングを指定
	}

	void ClearStage::OnCreate() {
		try {
			//ビューとライトの作成
			CreateViewLight();
			CreateClearSprite();

		}
		catch (...) {
			throw;
		}
	}

	void ClearStage::OnUpdate()
	{

	}

	void ClearStage::CreateClearSprite() {
		AddGameObject<Sprite>(L"TEX_CLEAR", false,
			Vec2(900, 350), Vec2(0.0f, 150.0f));

		AddGameObject<Sprite>(L"TEX_TITLEBACK", false,
			Vec2(500, 150), Vec2(0.0f, -200.0f));

		//auto Score = Player::DrawStrings();
	}

}
//end basecross
