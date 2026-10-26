/*!
@file Scene.cpp
@brief シーン実装
*/

#include "stdafx.h"
#include "Project.h"
#include "DXModelResource.h"
#include "JoltManager.h"
#include "Scene.h"

namespace basecross{

	//--------------------------------------------------------------------------------------
	///	ゲームシーン
	//--------------------------------------------------------------------------------------
	void Scene::CreateResourses() {

		// リソース作成
		LoadMedia loadMedia;
		loadMedia.RegisterMediaFiles(App::GetApp()->GetDataDirWString());

		// エディターにオブジェクトを登録
		REGISTER(Ground, 1, L"Ground");
		REGISTER(Wall, 2, L"Wall");
		REGISTER(Floor, 3, L"Floor");
		REGISTER(InvisibleWall, 4, L"InvisibleWall");
		REGISTER(Koban, 10, L"Koban");
		REGISTER(Enemy, 11, L"Enemy");
		REGISTER(ClearObject, 12, L"ClearObject");
		REGISTER(ActionCamPos, 13, L"ActionCamPos");
	}

	void Scene::OnCreate(){
		try {
			JoltManager::StaticInitialize();

			// 背景色を設定
			SetClearColor(Col4(0.0f, 0.11328125f, 0.2578125, 1.0f)); // ミッドナイトブルー
			
			//リソース作成
			CreateResourses();

			//自分自身にイベントを送る
			//これによりゲームステージのオブジェクトがCreate時にシーンにアクセスできる
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToTitleStage");
		}
		catch (...) {
			throw;
		}
	}

	Scene::~Scene() {
		JoltManager::StaticTerminate();
	}

	void Scene::OnEvent(const shared_ptr<Event>& event) {
		if (event->m_MsgStr == L"ToGameStage") {
			//ゲームステージの設定
			ResetActiveStage<GameStage>();
		}
		if (event->m_MsgStr == L"ToSelectStage") {
			ResetActiveStage<SelectStage>();
		}
		if (event->m_MsgStr == L"ToTitleStage") {
			ResetActiveStage<TitleStage>();
		}
		if (event->m_MsgStr == L"ToClearStage") {
			ResetActiveStage<ClearStage>();
		}
		if (event->m_MsgStr == L"ToGameOverStage") {
			ResetActiveStage<GameOverStage>();
		}

	}
}
//end basecross
