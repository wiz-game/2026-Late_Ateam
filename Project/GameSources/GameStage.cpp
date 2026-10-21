/*!
@file GameStage.cpp
@brief ゲームステージ実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------

	//ビューとライトの作成
	void GameStage::CreateViewLight() {
		// カメラの設定
		auto camera = ObjectFactory::Create<MainCamera>();
		camera->SetEye(Vec3(0.0f, 0.0f, -5.0f));
		camera->SetAt(Vec3(0.0f, 0.0f, 0.0f));
		m_camera = camera;

		auto actionCamera = ObjectFactory::Create<ActionCamera>();
		m_actionCamera = actionCamera;

		// ビューにカメラを設定
		m_view = CreateView<SingleView>();
		m_view->SetCamera(camera);

		//マルチライトの作成
		auto light = CreateLight<MultiLight>();
		light->SetDefaultLighting(); //デフォルトのライティングを指定
	}

	void GameStage::CreatePlayer() 
	{
		// プレイヤーの作成
		auto player = AddGameObject<Player>();

		// カメラをプレイヤーに紐づけ
		m_camera->SetTargetObject(player);

		m_actionCamera->SetPleyer(player);

		// プレイヤーを設定
		SetSharedGameObject(L"Player", player);
	}

	void GameStage::LoadStage()
	{
		// バイナリファイルの取得
		auto path = App::GetApp()->GetDataDirWString();

		// ステージローダーの作成
		auto loader = AddGameObject<StageLoader>();

		// ステージの番号を取得
		int loadNum = App::GetApp()->GetScene<Scene>()->GetLoadStageNumber();
		
		// ステージのロード
		loader->LoadStageFile(path + L"Stages\\stage_" + to_wstring(loadNum) + L".stg");
	}

	void GameStage::OnCreate() {
		try {
			auto& app = App::GetApp();

			// JoltPhysicsを初期化する
			m_jphManger.Initialize();

			// ビューとライトの作成
			CreateViewLight();

//----<Editor>------------------------------------------------------------------------------
			LoadStage();			// ステージのロード	
			CreatePlayer();			// プレイヤーの作成	
			AddGameObject<Editor>();// エディターの作成	
//------------------------------------------------------------------------------------------
			auto enemy = AddGameObject<Enemy>();
			enemy->SetEnemyPoint(
				{
					Vec3(0.0f,1.0f,0.0f),
					Vec3(0.0f,1.0f,20.0f),
					Vec3(20.0f,1.0f,0.0f),
					Vec3(20.0f,1.0f,-20.0f),
				}
				);
		}
		catch (...) {
			throw;
		}
	}

	void GameStage::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();

		TopActionCamera();
	}

	void GameStage::OnUpdate2()
	{
		m_jphManger.Update(1.0f / 60.0f);

	}

	void GameStage::OnDraw()
	{
	}

	void GameStage::TopActionCamera()
	{
		static bool isActionCamera = false;
		static bool oldXState = false;

		XINPUT_STATE state{};

		if (XInputGetState(0, &state) != ERROR_SUCCESS)
		{
			return;
		}

		bool xPressed =
			(state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;

		if (xPressed && !oldXState)
		{
			isActionCamera = !isActionCamera;

			if (isActionCamera)
			{
				OutputDebugString(L"Action Camera\n");
				m_view->SetCamera(m_actionCamera);
			}
			else
			{
				OutputDebugString(L"Normal Camera\n");
				m_view->SetCamera(m_camera);
			}
		}

		oldXState = xPressed;
	}
}
//end basecross
