/*!
@file TitleStage.cpp
@brief タイトルステージ実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------

	//ビューとライトの作成
	void TitleStage::CreateViewLight() {
		// カメラの設定
		auto camera = ObjectFactory::Create<Camera>();
		camera->SetEye(Vec3(0.0f, 10.0f, -12.0f));
		camera->SetAt(Vec3(0.0f, 0.0f, 0.0f));

		// ビューにカメラを設定
		auto view = CreateView<SingleView>();
		view->SetCamera(camera);

		//マルチライトの作成
		auto light = CreateLight<MultiLight>();
		light->SetDefaultLighting(); //デフォルトのライティングを指定
	}

	void TitleStage::OnCreate() {
		try {
			auto& app = App::GetApp();
			CreateViewLight();
			CreateTitleSprite();

		}
		catch (...) {
			throw;
		}
	}

	void TitleStage::OnUpdate()
	{
		auto& app = App::GetApp();
		auto scene = app->GetScene<Scene>();

		auto pad = app->GetInputDevice().GetControlerVec()[0];

		if (pad.wPressedButtons & XINPUT_GAMEPAD_A || App::GetApp()->GetInputDevice().GetKeyState().m_bPressedKeyTbl[VK_SPACE])
		{
			PostEvent(0.0f, GetThis<TitleStage>(), scene, L"ToGameStage");
		}

	}
	void TitleStage::CreateTitleSprite() {
		AddGameObject<StaticUI>(L"TitleBuak", true, Vec2(1300.0f, 800.0f), Vec2(0.0f, 0.0f));
		AddGameObject<StaticUI>(L"TitleLogo", true, Vec2(550.0f, 350.0f), Vec2(350.0f, 100.0f));
		//auto Score = Player::DrawStrings();
	}

}


