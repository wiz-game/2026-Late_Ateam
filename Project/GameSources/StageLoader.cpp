/*!
@file StageLoader.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void StageLoader::LoadStageFile(const wstring& filename)
	{
		ifstream ifs(filename, std::ios::binary);
		if (ifs.fail()) {
			MessageBox(0, L"ステージファイルを開けませんでした", L"failed", 0);
			return;
		}

		// オブジェクトの数を書き込む
		uint32_t num;
		ifs.read((char*)&num, sizeof(num)); // はじめの4バイトとして、オブジェクト数を書き込む

		// 処理のためのオブジェクトグループの作成
		auto stageObj = GetStage()->CreateSharedObjectGroup(L"StageObjects");

		for (int i = 0; i < num; i++)
		{
			// 必要になる変数の初期化（switvh文内で宣言できなかったので）
			uint16_t id = 0;
			Vec3 objScale;
			Vec3 objRot;
			Vec3 objPos;
			Vec3 switchScale;
			Vec3 switchRot;
			Vec3 switchPos;
			shared_ptr<GameObjectForEdit> object;


			// データを書き込む
			ifs.read((char*)&id, sizeof(id)); // オブジェクトID
			ifs.read((char*)&objScale, sizeof(objScale)); // スケール
			ifs.read((char*)&objRot, sizeof(objRot)); // 回転
			ifs.read((char*)&objPos, sizeof(objPos)); // 座標
			switch (id)
			{
			case 1:
				object = GetStage()->AddGameObject<Ground>(objScale, objRot, objPos);
				object->SetMemberRotation(objRot);
				stageObj->IntoGroup(object);
				break;
			default:
				break;
			}
		}

		// ファイルを閉じる
		ifs.close();

	}


}
//end basecross
