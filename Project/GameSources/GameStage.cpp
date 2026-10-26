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

		// スカイボックスの作成
		AddGameObject<SkyBox>();

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

			//データから地点を生成
			//auto cameraDatta = ActionCamInfo::Create();
			//for (const auto& data : cameraDatta)
			//{
			//	m_acPostionList.emplace_back(
			//		data.ActionPos,
			//		data.Radius,
			//		data.RoomCenter
			//	);
			//}

//----<Editor>------------------------------------------------------------------------------
			CreatePlayer();			// プレイヤーの作成	
			LoadStage();			// ステージのロード	
			AddGameObject<Editor>();// エディターの作成	
//------------------------------------------------------------------------------------------

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

//----------------------------<アクションカメラ関係>----------------------------------------------

	void GameStage::TopActionCamera()
	{
		static bool isActionCamera = false;
		static bool oldXState = false;

		auto triggerList = GetGameObjectVec();
		auto playMove = std::dynamic_pointer_cast<Player>(GetSharedObject(L"Player", false));

		if (!playMove)
		{
			return;

		}

		ActionCamPos* currentTrigger = nullptr;
		bool canUseActionCamera = false;

		for (auto& trigger : m_acPostionList)
		{
			if (trigger.IsInRange(playMove->GetComponent<Transform>()->GetPosition()))
			{
				OutputDebugString(L"In Range!\n");
				currentTrigger = &trigger;
				canUseActionCamera = true;
				break;
			}
		}

		XINPUT_STATE state{};

		if (XInputGetState(0, &state) != ERROR_SUCCESS)
		{
			return;
		}

		bool xPressed =
			(state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;

		if (canUseActionCamera && xPressed && !oldXState)
		{
			isActionCamera = !isActionCamera;

			if (currentTrigger)
			{
				m_actionCamera->SetRoomCrnter(currentTrigger->GetRoomCenter());
			}

			if (isActionCamera)
			{
				playMove->SetPlayerUpdateActive(false);
				OutputDebugString(L"Action Camera\n");
				m_view->SetCamera(m_actionCamera);
			}
			else
			{
				playMove->SetPlayerUpdateActive(true);
				OutputDebugString(L"Normal Camera\n");
				m_view->SetCamera(m_camera);
			}
		}

		oldXState = xPressed;
	}
}
//end basecross
