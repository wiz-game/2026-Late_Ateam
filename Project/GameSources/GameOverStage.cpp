/*!
@file GameOverStage.cpp
@brief ゲームオーバーステージ実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゲームオーバーステージクラス実体
	//--------------------------------------------------------------------------------------

	//ビューとライトの作成
	void GameOverStage::CreateViewLight() {
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

	void GameOverStage::OnCreate() {
		try {
			//ビューとライトの作成
			CreateViewLight();
			CreateGameOverSprite();

		}
		catch (...) {
			throw;
		}
	}

	void GameOverStage::OnUpdate()
	{
		auto& app = App::GetApp();
		auto scene = app->GetScene<Scene>();

		auto pad = app->GetInputDevice().GetControlerVec()[0];


		if (pad.wPressedButtons & XINPUT_GAMEPAD_A || App::GetApp()->GetInputDevice().GetKeyState().m_bPressedKeyTbl[VK_SPACE])
		{
			PostEvent(0.0f, GetThis<GameOverStage>(), scene, L"ToTitleStage");
		}

	}
	void GameOverStage::CreateGameOverSprite() {
		AddGameObject<Sprite>(L"GameOver", false,
			Vec2(900, 350), Vec2(0.0f, 150.0f));

		AddGameObject<Sprite>(L"TitleBack1", false,
			Vec2(500, 150), Vec2(0.0f, -200.0f));

		//auto Score = Player::DrawStrings();
	}

}
//end basecross
