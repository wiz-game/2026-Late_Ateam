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

	void GameStage::RebuildStatic()
	{
		m_static.Rebuild(GetGameObjectVec(), m_jphManger.GetSystem());
	}

	void GameStage::OnCreate() {
		try {
			auto& app = App::GetApp();

			// JoltPhysicsを初期化する
			m_jphManger.Initialize();

			// ビューとライトの作成
			CreateViewLight();
			AddGameObject<StaticUI>(L"ButtanGuide", true, Vec2(300.0f, 150.0f), Vec2(500.0f, -300.0f));
			AddGameObject<AnimUI>(L"MissionList", true, Vec2(100.0f, 35.0f), Vec2(-500.0f, 300.0f));
//----<Editor>------------------------------------------------------------------------------
			CreatePlayer();			// プレイヤーの作成	
			auto mgr = AddGameObject<WayPointManager>();
			SetSharedGameObject(L"WayPointManager", mgr);
			LoadStage();			// ステージのロード	
			RebuildStatic();		// スタティックオブジェクトの判定作成
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

		auto& objList= GetGameObjectVec();

		for (auto& obj : objList)
		{
			auto trigger = std::dynamic_pointer_cast<ActionCamPos>(obj);
			
			if (!trigger)
			{
				continue;
			}

			auto trans = trigger->GetComponent<Transform>();
			if (!trans)
			{
				continue;
			}

			if (trigger->IsInRange(playMove->GetComponent<Transform>()->GetPosition()))
			{
				currentTrigger = trigger.get();
				canUseActionCamera = true;
				break;
			}
		}

		auto pad = App::GetApp()->GetInputDevice().GetControlerVec()[0];
			
		bool actionPressed = 
			(pad.wPressedButtons & XINPUT_GAMEPAD_B) != 0 || App::GetApp()->GetInputDevice().GetKeyState().m_bPressedKeyTbl[VK_RETURN];

		if (canUseActionCamera && actionPressed)
		{
			isActionCamera = !isActionCamera;

			if (currentTrigger)
			{
				m_actionCamera->ApplyTrigger(currentTrigger);
			}

			if (isActionCamera)
			{
				playMove->SetPlayerUpdateActive(false);
				m_view->SetCamera(m_actionCamera);
			}
			else
			{
				playMove->SetPlayerUpdateActive(true);
				m_view->SetCamera(m_camera);
			}
		}

	}
}
//end basecross
