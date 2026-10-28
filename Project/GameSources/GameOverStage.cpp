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

			auto PtrScene = App::GetApp()->GetScene<Scene>();
			PtrScene->SetStageNum(1);


		}
		catch (...) {
			throw;
		}
	}

	void GameOverStage::OnUpdate()
	{
		auto PtrScene = App::GetApp()->GetScene<Scene>();
		int StageNum = PtrScene->GetStageNum();

		bool isDecide = false; // 決定フラグ
		bool isUp = false;     // 上移動フラグ
		bool isDown = false;   // 下移動フラグ

		//---------------------------------------------------------
		// 1. コントローラの入力チェック
		//---------------------------------------------------------
		auto CntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		if (!CntlVec.empty() && CntlVec[0].bConnected) {
			if (CntlVec[0].wPressedButtons & XINPUT_GAMEPAD_A) {
				isDecide = true;
			}
			if (!m_CntrolLock) {
				if (CntlVec[0].fThumbLY >= 0.8f) {
					isUp = true;
				}
				else if (CntlVec[0].fThumbLY <= -0.8f) {
					isDown = true;
				}
			}
		}

		//---------------------------------------------------------
		// 2. キーボードの入力チェック
		//---------------------------------------------------------
		// 決定 (SPACEキー)
		if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
			isDecide = true;
		}

		// 選択切り替え (Wキーで上、Sキーで下)
		if (!m_CntrolLock) {
			if (GetAsyncKeyState('W') & 0x8000) {
				isUp = true;
			}
			else if (GetAsyncKeyState('S') & 0x8000) {
				isDown = true;
			}
		}

		//---------------------------------------------------------
		// 3. コントロールロックの解除判定
		//---------------------------------------------------------
		if (m_CntrolLock) {
			bool isStickReleased = CntlVec.empty() || !CntlVec[0].bConnected || (CntlVec[0].fThumbLY == 0.0f);
			bool isKeyReleased = !(GetAsyncKeyState('W') & 0x8000) && !(GetAsyncKeyState('S') & 0x8000);

			if (isStickReleased && isKeyReleased) {
				m_CntrolLock = false;
			}
		}

		//---------------------------------------------------------
		// 4. 入力結果の反映とステージ遷移処理
		//---------------------------------------------------------
		if (isDecide) {
			// ゲームステージへ / タイトルへ
			switch (PtrScene->GetStageNum()) {
			case 1:
				PtrScene->SetIsFirstStart(false);
				//CSVファイルの確定
				PostEvent(0.0f, GetThis<GameOverStage>(), PtrScene, L"ToGameStage");
				break;
			case 2:
				PtrScene->SetIsFirstStart(true);
				PtrScene->SetCheckpointPos(Vec3(0));
				//CSVファイルの確定
				PostEvent(0.0f, GetThis<GameOverStage>(), PtrScene, L"ToTitleStage");
				break;
			}
		}

		if (isUp) {
			StageNum--;
			if (StageNum < 1) {
				StageNum = 2;
			}
			m_CntrolLock = true;
			PtrScene->SetStageNum(StageNum);
			ChangeSelect(StageNum);
		}
		else if (isDown) {
			StageNum++;
			if (StageNum > 2) {
				StageNum = 1;
			}
			m_CntrolLock = true;
			PtrScene->SetStageNum(StageNum);
			ChangeSelect(StageNum);
		}
	}

	void GameOverStage::CreateGameOverSprite() {
		AddGameObject<Sprite>(L"GameOver", false,
			Vec2(900, 350), Vec2(0.0f, 250.0f));

		auto PtrSp = AddGameObject<Sprite>(L"Retry", true,
			Vec2(400.0f, 150.0f), Vec2(0.0f, -100.0f));
		PtrSp->SetSelect(true);
		m_SpVec[0] = PtrSp;
		PtrSp = AddGameObject<Sprite>(L"TitleBack1", true,
			Vec2(400.0f, 150.0f), Vec2(0.0f, -300.0f));
		m_SpVec[1] = PtrSp;
	}

	void GameOverStage::ChangeSelect(int num) {
		for (int i = 0; i < 2; i++) {
			shared_ptr<Sprite> shptr = m_SpVec[i].lock();
			if (shptr) {
				if ((i + 1) == num) {
					shptr->SetSelect(true);
				}
				else {
					shptr->SetSelect(false);
				}
			}
		}
	}

}
//end basecross
