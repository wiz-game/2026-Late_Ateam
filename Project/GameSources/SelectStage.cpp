/*!
@file SelectStage.cpp
@brief セレクトステージ実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	セレクトステージクラス実体
	//--------------------------------------------------------------------------------------

	//ビューとライトの作成
	void SelectStage::CreateViewLight() {
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

	void SelectStage::OnCreate() {
		try {
			//ビューとライトの作成
			CreateViewLight();

		}
		catch (...) {
			throw;
		}
	}

	void SelectStage::OnUpdate()
	{
		auto& app = App::GetApp();
		auto scene = app->GetScene<Scene>();

		// デバイスの確保
		auto device = App::GetApp()->GetInputDevice();
		auto keyState = device.GetKeyState();

		wstringstream ss;

		int num = scene->GetLoadStageNumber();

		// キーボードで入力した数字を読み込むステージにする
		for (int i = 0; i <= 9; ++i)
		{
			if (keyState.m_bPressedKeyTbl['0' + i])
			{
				// 桁溢れを防ぐ
				if (num < 100000)
				{
					num = num * 10 + i;
				}
			}
		}

		// BackSpaseで一文字消す
		if (keyState.m_bPressedKeyTbl[VK_BACK])
		{
			num /= 10;
		}

		ss << L"Stage::" << num << endl;
		scene->SetDebugString(ss.str());

		scene->SetLoadStageNumber(num);

		if (keyState.m_bPressedKeyTbl[VK_RETURN])
		{
			PostEvent(0.0f, GetThis<SelectStage>(), scene, L"ToGameStage");
		}

	}
}
//end basecross
