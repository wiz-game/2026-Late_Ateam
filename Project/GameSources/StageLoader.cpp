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
		uint32_t num = 0;
		ifs.read((char*)&num, sizeof(num)); // はじめの4バイトとして、オブジェクト数を書き込む

		// 処理のためのオブジェクトグループの作成
		auto stageObj = GetStage()->CreateSharedObjectGroup(L"StageObjects");

		vector<shared_ptr<GameObjectForEdit>> created;   // ファイル内の順番どおり読めなかったものはnullptr

		for (uint32_t i = 0; i < num; i++)
		{
			// 必要になる変数の初期化（switvh文内で宣言できなかったので）
			uint16_t id = 0;
			Vec3 objScale;
			Vec3 objRot;
			Vec3 objPos;
			shared_ptr<GameObjectForEdit> object;


			// データを書き込む
			ifs.read((char*)&id, sizeof(id)); // オブジェクトID
			ifs.read((char*)&objScale, sizeof(objScale)); // スケール
			ifs.read((char*)&objRot, sizeof(objRot)); // 回転
			ifs.read((char*)&objPos, sizeof(objPos)); // 座標
			auto info = EditObjectRegistry::Get().FindByID(id);
			if (info)
			{
				object = info->create(GetStage(), objScale, objRot, objPos);
				if (object)
				{
					object->SetMemberRotation(objRot);
					stageObj->IntoGroup(object);
				}
			}

			created.push_back(object);

		}

		// EnemyとWayPoint関係の追加データ
		bool hasExtra = false;
		uint32_t extraNum = 0;
		if (ifs.read((char*)&extraNum, sizeof(extraNum)))
		{
			hasExtra = true;
			for (uint32_t k = 0; k < extraNum; k++)
			{
				uint32_t index = 0;
				int32_t a = 0, b = 0;
				ifs.read((char*)&index, sizeof(index));
				ifs.read((char*)&a, sizeof(a));
				ifs.read((char*)&b, sizeof(b));
				if (!ifs || index >= created.size() || !created[index]) continue;

				if (auto enemy = dynamic_pointer_cast<Enemy>(created[index]))
				{
					enemy->SetGroupID(a);
				}
				else if (auto wp = dynamic_pointer_cast<WayPoint>(created[index]))
				{
					wp->SetID(a);
					wp->SetOrder(b);
				}
			}
		}

		// Enemyのグループが重ならないようにする
		if (!hasExtra)
		{
			int g = 0;
			for (auto& o : created)
				if (auto enemy = dynamic_pointer_cast<Enemy>(o)) enemy->SetGroupID(g++);
		}

		// ファイルを閉じる
		ifs.close();

	}


}
//end basecross
