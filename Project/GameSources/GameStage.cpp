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

		// ビューにカメラを設定
		auto view = CreateView<SingleView>();
		view->SetCamera(camera);

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


		}
		catch (...) {
			throw;
		}
	}

	void GameStage::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();

	}

	void GameStage::OnUpdate2()
	{
		m_jphManger.Update(1.0f / 60.0f);
	}

	void GameStage::OnDraw()
	{
	}
}
//end basecross
