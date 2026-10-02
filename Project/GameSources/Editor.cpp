/*!
@file Editor.cpp
@brief ステージのエディター
*/

#include "stdafx.h"
#include "Project.h"
#include "GameStage.h"
#include <iomanip>

namespace basecross {

	//初期化
	void Editor::OnCreate()
	{
		m_arrow = GetStage()->AddGameObject<Arrow>();
		auto arrow = m_arrow.lock();
		if (arrow)
		{
			arrow->SetDrawActive(false);
		}
		m_mark = GetStage()->AddGameObject<GameObject>();
		auto mark = m_mark.lock();
		if (mark)
		{
			mark->AddComponent<PCTStaticDraw>()->SetMeshResource(L"DEFAULT_SPHERE");
			mark->SetDrawActive(false);
		}
	}

	void Editor::OnUpdate()
	{
		// 必要な物の取得と初期化
		auto& app = App::GetApp();
		auto scene = app->GetScene<Scene>();
		auto device = app->GetInputDevice();
		auto keyState = device.GetKeyState();
		auto path = App::GetApp()->GetDataDirWString();
		auto stage = GetStage();
		auto delta = app->GetElapsedTime();
		wstringstream ss;

		// ウィークポインタのロック
		auto obj = m_arrow.lock();
		if (!obj) return;
		auto arrow = dynamic_pointer_cast<Arrow>(obj);
		if (!arrow) return;
		auto mark = m_mark.lock();
		if (!mark) return;


		//Bを押したときにエディターモードに変更
		if (keyState.m_bPressedKeyTbl['B'])
		{
			// プレイヤーやカメラの取得
			auto player = stage->GetSharedGameObject<Player>(L"Player");
			auto camera = stage->GetView()->GetTargetCamera();
			auto mainCamera = dynamic_pointer_cast<MainCamera>(camera);
			// ステージ内のオブジェクトの更新フラグを変更する
			auto objs = GetStage()->GetGameObjectVec();
			for (auto& obj : objs)
			{
				obj->SetUpdateActive(m_isEditorMode);
			}
			// 更新フラグの変更時の影響を戻す
			SetUpdateActive(true);
			stage->GetSharedObject(L"DebugString")->SetUpdateActive(true);
			stage->GetSharedObject(L"DebugString")->SetDrawLayer(5);
			arrow->SetUpdateActive(true);
			// 現在エディターモードでないなら
			if (!m_isEditorMode)
			{
				// エディターモードに変更しカメラをエディターに追従させる
				m_isEditorMode = true;
				mainCamera->SetTargetObject(GetThis<GameObject>());
				mainCamera->GetStateMachine()->ChangeState(EditorCameraState::Instance());

				m_stageNum = scene->GetLoadStageNumber();
			}
			else // エディターモードなら
			{
				// 通常の状態に戻し、カメラのターゲットをプレイヤーに戻す
				m_isEditorMode = false;
				//選択されていたオブジェクトのリセット
				ResetPickUpObject();
				mainCamera->SetTargetObject(player);
				mainCamera->GetStateMachine()->ChangeState(CameraNormalMoveState::Instance());
			}
		}
		// これ以降はエディターモードの時に動かす
		if (m_isEditorMode)
		{
			//Ctrl+Sでステージの保存
			if (keyState.m_bPressedKeyTbl['S'] && keyState.m_bPushKeyTbl[VK_CONTROL])
			{
				wstring name = L"stage_" + to_wstring(m_stageNum) + L".stg";
				SaveBinaryFile(path + L"Stages\\" + name);
			}

			if (keyState.m_bPressedKeyTbl['O'] && keyState.m_bPushKeyTbl[VK_CONTROL])
			{
				return;
				ChangeStage();
			}

			//　座標、スケール、回転のどれを変更するかのデバッグ文字列を設定
			switch (m_changeTransform)
			{
			case 0:
				ss << L"Position" << endl;
				break;
			case 1:
				ss << L"Scale" << endl;
				break;
			case 2:
				ss << L"Rotation" << endl;
				break;
			}

			//右クリックでカメラの方向に移動する
			if (keyState.m_bPushKeyTbl[VK_RBUTTON])
			{
				auto camera = stage->GetView()->GetTargetCamera();
				auto velocity = Vec3(camera->GetAt() - camera->GetEye()).normalize(); // 注視点からカメラ位置の単位ベクトルを移動ベクトルとする
				auto inputVec = GetInputVec(); // 操作に関する入力

				// カメラの向きが前になるように進む
				// 前方向の移動の処理
				auto pos = GetComponent<Transform>()->GetPosition();
				pos += inputVec.z * velocity * delta * 30;

				//左右方向への移動の処理
				auto upperVec = Vec3(0.0f, 1.0f, 0.0f); // 真上方向の単位ベクトル
				auto cross = Vec3(velocity.y * upperVec.z - velocity.z * upperVec.y, velocity.z * upperVec.x - velocity.x * upperVec.z, velocity.x * upperVec.y - velocity.y * upperVec.x); // upperVecと移動ベクトルとの外積
				pos += -cross * inputVec.x * delta * 30;
				// 上下方向の移動
				pos.y += inputVec.y * delta * 30;
				GetComponent<Transform>()->SetPosition(pos);
			}
			//マウスの左クリックでオブジェクトを選択する
			if (keyState.m_bPressedKeyTbl[VK_LBUTTON])
			{
				auto pickObj = PickUpGameObject();
				if (pickObj)
				{
					//選択されていたオブジェクトのリセット
					ResetPickUpObject();
					// オブジェクトの選択
					m_pickObj = pickObj;
					// 選択されたオブジェクトのロック
					auto obj = m_pickObj.lock();
					auto object = dynamic_pointer_cast<GameObjectForEdit>(obj);
					if (object)
					{
						// 選択されたオブジェクトの色の変更、座標などの保存、マウスの保存、XYZの矢印の描画のONにする
						SetObjectEmissive(obj, Col4(1.0f, 1.0f, 0.0f, 1.0f), true);
						m_objPos = object->GetComponent<Transform>()->GetPosition();
						m_objScale = object->GetComponent<Transform>()->GetScale();
						m_objRot = object->GetMemberRotation();
						m_objRotBase = m_objRot;
						GetMouseRay(m_pastMauseRay[0], m_pastMauseRay[1]);
						arrow->SetDrawActive(true);
						arrow->SetArrowPosition(m_objPos);
					}
				}
			}

			// マウス中ボタンでプレイヤーの移動
			if (keyState.m_bPressedKeyTbl[VK_MBUTTON])
			{
				auto player = GetStage()->GetSharedGameObject<Player>(L"Player");
				auto camera = GetStage()->GetView()->GetTargetCamera();
				auto at = camera->GetAt();
				at.x = (float)((int)at.x);
				at.y = (float)((int)at.y);
				at.z = (float)((int)at.z);

				player->GetComponent<Transform>()->SetPosition(at);
			}

			//	Lキーでオブジェクトの選択を解除
			if (keyState.m_bPressedKeyTbl['L'])
			{
				ResetPickUpObject();
			}
			//W,E,RでTransformの変更項目を変える
			if (!(keyState.m_bPushKeyTbl[VK_LBUTTON] || keyState.m_bPushKeyTbl[VK_RBUTTON]))
			{
				if (keyState.m_bPressedKeyTbl['W'])m_changeTransform = 0;
				if (keyState.m_bPressedKeyTbl['E'])m_changeTransform = 1;
				if (keyState.m_bPressedKeyTbl['R'])m_changeTransform = 2;
			}
			//生成するオブジェクトを変更
			if (keyState.m_bPressedKeyTbl[VK_LEFT])
			{
				m_addObjNum--;
			}
			if (keyState.m_bPressedKeyTbl[VK_RIGHT])
			{
				m_addObjNum++;
			}
			//移動、スケールのスナップサイズを変更
			if (keyState.m_bPressedKeyTbl[VK_UP])
			{
				m_snapSize += 0.1f;
			}
			if (keyState.m_bPressedKeyTbl[VK_DOWN])
			{
				m_snapSize -= 0.1f;
			}

			// 編集するステージ番号を変更
			if (keyState.m_bPressedKeyTbl['1'])
			{
				m_stageNum--;
			}
			if (keyState.m_bPressedKeyTbl['2'])
			{
				m_stageNum++;
			}
			ss << L"Stage : " << m_stageNum << endl;

			// 選択状態のオブジェクトの複製
			CopyObject();

			// 選択状態のオブジェクトの数値の変更
			MovePickUpObject(ss);

			// Xでオブジェクトの追加
			if (keyState.m_bPressedKeyTbl['X'])
			{
				AddNewObject();
			}
			ss << L"NUM:" << stage->GetGameObjectVec().size() << endl;
			auto stl = L"";
			switch (m_addObjNum)
			{
			case 1:
				stl = L"Ground";
				break;

			default:
				stl = L"None";
				break;
			}
			ss << L"ObjectName:" << m_addObjNum << L" " << stl << endl;
			ss << L"SnapSize: " << m_snapSize << endl;

			scene->SetDebugString(ss.str());
		}

	}

	void Editor::SaveBinaryFile(const wstring& filename)
	{
		//保存するかどうかの確認
		auto result = MessageBox(NULL, L"ステージを保存しますか", L"Save", MB_YESNO);
		if (result == IDNO)
		{
			return;
		}

		// 保存用ファイルを開く
		ofstream ofs(filename, std::ios::binary);
		//ファイルが開かなかったらその旨を伝えるメッセージを出す
		if (ofs.fail())
		{
			MessageBox(0, L"保存用ファイルを開けませんでした", L"failed", 0);
			return;
		}
		vector<shared_ptr<GameObjectForEdit>> objs;
		for (auto obj : GetStage()->GetGameObjectVec())
		{
			auto object = dynamic_pointer_cast<GameObjectForEdit>(obj);

			if (object)
			{
				objs.push_back(object);
			}
		}
		// オブジェクトの数を書き込む
		uint32_t num = objs.size();
		ofs.write((char*)&num, sizeof(num)); // はじめの4バイトとして、オブジェクト数を書き込む
		for (int i = 0; i < num; i++)
		{
			uint16_t id = CheckObjectID(objs[i]);
			if (id == 0) continue;

			auto objTrans = objs[i]->GetComponent<Transform>();
			auto objScale = objTrans->GetScale();
			auto objRot = objs[i]->GetMemberRotation();
			auto objPos = objTrans->GetPosition();

			// データを書き込む
			ofs.write((char*)&id, sizeof(id)); // オブジェクトID
			ofs.write((char*)&objScale, sizeof(objScale)); // スケール
			ofs.write((char*)&objRot, sizeof(objRot)); // 回転
			ofs.write((char*)&objPos, sizeof(objPos)); // 座標

		}
		// 成功のメッセージを表示してファイルを閉じる
		MessageBox(0, L"ステージ配置を保存しました", L"Binary file save succeeded", 0);
		ofs.close();


	}

	shared_ptr<GameObject> Editor::PickUpGameObject()
	{
		//カメラの向いている方向と開始地点から判定を確認
		auto camera = GetStage()->GetView()->GetTargetCamera();
		Vec3 eye = camera->GetEye();
		Vec3 startPos = eye;
		Vec3 distance = Vec3(camera->GetAt() - eye).normalize();
		Vec3 endPos = eye + distance * 100;
		GetMouseRay(startPos, endPos);
		Vec3 hitPos; // 当たった時の座標を受け取る用の変数
		vector<shared_ptr<GameObject>> hitObj; // Hitしたオブジェクトを保存しておく配列
		vector<Vec3> hitPositions; // 当たった座標を保存しておく配列

		// 移動用の矢印に当たっていたら処理を飛ばす
		auto obj = m_arrow.lock();
		if (obj)
		{
			auto arrow = dynamic_pointer_cast<Arrow>(obj);
			if (arrow)
			{
				arrow->CheckHitAnyArrow(startPos, endPos);
				if (arrow->GetPickNum() >= 0)
				{
					return nullptr;
				}
			}
		}
		for (auto& v : GetStage()->GetGameObjectVec())
		{
			auto player = dynamic_pointer_cast<Player>(v);

			// 当たり判定の確認
			bool isHit = false;

			// マルチメッシュを使用しているオブジェクトかどうかのブール
			bool useMulti = false;
			if (!useMulti)
			{
				isHit = RayHitCheck::HitTest(v, startPos, endPos, hitPos);
			}

			// マルチメッシュを使用しているオブジェクトの当たり判定
			if (useMulti)
			{
				float dis;
				isHit = RayHitCheck::HitTestVirtualSphere(v, startPos, endPos, hitPos, dis);
			}


			//当たっていたら配列に保存
			if (isHit)
			{
				hitObj.push_back(v);
				hitPositions.push_back(hitPos);
			}

		}
		//判定がとられた座標から最も開始地点から近い場所をHitPositionにする
		if (hitPositions.size() > 0)
		{
			auto hitPosition = hitPositions[0];
			shared_ptr<GameObject> selectObj = hitObj[0];

			for (int i = 0; i < hitPositions.size(); i++)
			{
				auto newDis = startPos - hitPositions[i];
				auto nowDis = startPos - hitPosition;
				if (newDis.length() < nowDis.length())
				{
					hitPosition = hitPositions[i];
					selectObj = hitObj[i];
				}
			}

			if (selectObj)
			{
				if (selectObj.get() == m_pickObj.lock().get())
				{
					return false;
				}
				//m_pickObj = selectObj;
				return selectObj;
			}

		}
		else
		{
			ResetPickUpObject();
			return nullptr;
		}

		return nullptr;
	}

	void Editor::SetObjectEmissive(const shared_ptr<GameObject>& obj, const Col4& col, bool b)
	{
		shared_ptr<DrawComponent> baseDraw = obj->GetComponent<DrawComponent>(false);
		auto bcDraw = dynamic_pointer_cast<BcBaseDraw>(baseDraw);
		auto smDraw = dynamic_pointer_cast<SmBaseDraw>(baseDraw);
		if (bcDraw && b)
		{
			m_picObjCol = bcDraw->GetEmissive();
		}
		if (bcDraw)bcDraw->SetEmissive(col);
		if (smDraw && b)
		{
			m_picObjCol = smDraw->GetEmissive();
		}
		if (smDraw)smDraw->SetEmissive(col);

	}

	void Editor::GetMouseRay(Vec3& start, Vec3& end)
	{
		Mat4x4 world, view, proj;
		world.affineTransformation(
			Vec3(1.0f, 1.0f, 1.0f),
			Vec3(0.0f, 0.0f, 0.0f),
			Vec3(0.0f, 0.0f, 0.0f),
			Vec3(0.0f, 0.0f, 0.0f)
		);
		auto PtrCamera = GetStage()->GetView()->GetTargetCamera();
		view = PtrCamera->GetViewMatrix();
		proj = PtrCamera->GetProjMatrix();
		auto viewport = GetStage()->GetView()->GetTargetViewport();

		//フルスクリーンだった時の対応
		App::GetApp()->GetDeviceResources()->GetD3DDevice();
		auto width = viewport.Width;
		auto height = viewport.Height;
		auto winLong = GetWindowLong(App::GetApp()->GetHWnd(), GWL_STYLE);
		if (!(winLong & WS_OVERLAPPEDWINDOW)) {
			//フルスクリーン
			width = (float)GetSystemMetrics(SM_CXSCREEN);
			height = (float)GetSystemMetrics(SM_CYSCREEN);
		}
		//マウスのポイントの取得
		auto point = App::GetApp()->GetInputDevice().GetKeyState().m_MouseClientPoint;

		start = XMVector3Unproject(
			Vec3((float)point.x, (float)point.y, 0),
			viewport.TopLeftX,
			viewport.TopLeftY,
			width,
			height,
			viewport.MinDepth,
			viewport.MaxDepth,
			proj,
			view,
			world);

		end = XMVector3Unproject(
			Vec3((float)point.x, (float)point.y, 1.0),
			viewport.TopLeftX,
			viewport.TopLeftY,
			width,
			height,
			viewport.MinDepth,
			viewport.MaxDepth,
			proj,
			view,
			world);

	}

	Vec3 Editor::GetInputVec()
	{
		//各種入力機器と経過時間の取得
		auto& app = App::GetApp();
		auto device = app->GetInputDevice();
		auto pad = device.GetControlerVec()[0];
		auto keyState = device.GetKeyState();
		float delta = app->GetElapsedTime();

		Vec3 v = Vec3(0);
		//X方向の操作
		v.x = pad.fThumbLX;
		if (keyState.m_bPushKeyTbl['A'])
		{
			v.x = -1.0f;
		}
		if (keyState.m_bPushKeyTbl['D'])
		{
			v.x = 1.0f;
		}
		//Z方向の操作
		v.z = pad.fThumbLY;
		if (keyState.m_bPushKeyTbl['W'])
		{
			v.z = 1.0f;
		}
		if (keyState.m_bPushKeyTbl['S'])
		{
			v.z = -1.0f;
		}
		//大きさの調整
		if (v.length() > 1.0f)
		{
			v = v.normalize();
		}
		if (keyState.m_bPushKeyTbl['E'])
		{
			v.y = 1.0f;
		}
		if (keyState.m_bPushKeyTbl['Q'])
		{
			v.y = -1.0f;
		}

		return v;

	}

	void Editor::AddNewObject()
	{
		shared_ptr<GameObject> obj;
		auto camera = GetStage()->GetView()->GetTargetCamera();
		auto at = camera->GetAt();
		at.x = (float)((int)at.x);
		at.y = (float)((int)at.y);
		at.z = (float)((int)at.z);

		switch (m_addObjNum)
		{
		case 1:
			obj = GetStage()->AddGameObject<Ground>(Vec3(1.0f), Vec3(0.0f), at);
			break;

		default:
			break;
		}
		if (obj)
		{
			obj->SetUpdateActive(false);
		}

	}

	void Editor::ResetPickUpObject()
	{
		// オブジェクトのロック
		auto pobj = m_pickObj.lock();
		auto arrow = m_arrow.lock();
		auto mark = m_mark.lock();
		if (pobj && arrow)
		{
			// 選択されていたオブジェクトの色を元に戻す
			SetObjectEmissive(pobj, m_picObjCol, false);
			arrow->SetDrawActive(false);
			mark->SetDrawActive(false);
			m_pickObj.reset();
		}

	}

	void Editor::MovePickUpObject(wstringstream& ss)
	{
		// オブジェクトのロック
		auto objA = m_arrow.lock();
		auto mark = m_mark.lock();
		if (!objA)return;
		auto arrow = dynamic_pointer_cast<Arrow>(objA);
		if (!arrow || !mark)return;

		// デバイスの確保
		auto device = App::GetApp()->GetInputDevice();
		auto keyState = device.GetKeyState();



		//選択状態のオブジェクトを移動
		auto obj = m_pickObj.lock();
		auto object = dynamic_pointer_cast<GameObjectForEdit>(obj);
		if (object)
		{
			// 選択状態のオブジェクトの削除
			if (keyState.m_bPressedKeyTbl[VK_DELETE])
			{
				GetStage()->RemoveGameObject<GameObject>(obj);
				arrow->SetDrawActive(false);
				mark->SetDrawActive(false);
			}

			// オブジェクトの情報を保存
			auto objTrans = object->GetComponent<Transform>();
			auto pos = objTrans->GetPosition();
			auto rot = m_objRotBase;
			auto scale = objTrans->GetScale();

			// マウスのレイの始点、終点を確保する
			Vec3 start, end;
			GetMouseRay(start, end);

			// 左クリック時に移動用の矢印とのヒット判定と、過去のマウスのレイの更新を行う
			if (keyState.m_bPressedKeyTbl[VK_LBUTTON])
			{
				arrow->CheckHitAnyArrow(start, end);
				GetMouseRay(m_pastMauseRay[0], m_pastMauseRay[1]);
			}

			// 左クリックされているときにモードに応じてオブジェクトを変更する
			if (keyState.m_bPushKeyTbl[VK_LBUTTON])
			{
				Vec3 moveVec = (end - start) - (m_pastMauseRay[1] - m_pastMauseRay[0]);
				moveVec *= (Vec3(pos - start).length() / Vec3(end - start).length()) * 1.5f;
				// スナップサイズに合わせてそれぞれのサイズを調整する
				moveVec.x = (float)((int)(moveVec.x / m_snapSize)) * m_snapSize;
				moveVec.y = (float)((int)(moveVec.y / m_snapSize)) * m_snapSize;
				moveVec.z = (float)((int)(moveVec.z / m_snapSize)) * m_snapSize;
				// どの矢印が選択されているかを確認し、それに応じてmoveVecのxyzの値を変更する
				int num = arrow->GetPickNum();
				switch (num)
				{
				case 0:
					moveVec.y = 0;
					moveVec.z = 0;
					break;
				case 1:
					moveVec.x = 0;
					moveVec.z = 0;
					break;
				case 2:
					moveVec.x = 0;
					moveVec.y = 0;
					break;
				default:
					moveVec = Vec3(0.0f);

				}
				// m_changeTransformより指定された項目を変更する
				switch (m_changeTransform)
				{
				case 0:
					pos = m_objPos + moveVec;
					break;
				case 1:
					scale = m_objScale + moveVec;
					break;
				case 2:
					rot = m_objRot + (moveVec * XMConvertToRadians(10));
					if (rot.x < -XM_2PI)rot.x = -XM_2PI;
					if (rot.y < -XM_2PI)rot.y = -XM_2PI;
					if (rot.z < -XM_2PI)rot.z = -XM_2PI;
					if (rot.x > XM_2PI)rot.x = XM_2PI;
					if (rot.y > XM_2PI)rot.y = XM_2PI;
					if (rot.z > XM_2PI)rot.z = XM_2PI;
					break;
				}

			}
			else
			{
				// 左クリックされていないときはメンバ変数を更新する
				switch (m_changeTransform)
				{
				case 0:
					m_objPos = pos;
					break;
				case 1:
					m_objScale = scale;
					break;
				case 2:
					m_objRot = rot;
					break;
				}

			}
			// オブジェクトを数値をもとに変更
			switch (m_changeTransform)
			{
			case 0:
				objTrans->SetPosition(pos);
				arrow->SetArrowPosition(pos);
				break;
			case 1:
				objTrans->SetScale(scale);
				break;
			case 2:
				objTrans->SetRotation(rot);
				object->SetMemberRotation(rot);
				m_objRotBase = rot;
				break;
			}

			// 確認のためにデバッグ用文字列に数値を追加する
			//ss << std::fixed << std::setprecision(15);
			ss << L"Position: " << pos.x << L" " << pos.y << L" " << pos.z << L" " << endl;
			ss << L"Rotation: " << XMConvertToDegrees(rot.x) << L" " << XMConvertToDegrees(rot.y) << L" " << XMConvertToDegrees(rot.z) << L" " << endl;
			ss << L"Scale: " << scale.x << L" " << scale.y << L" " << scale.z << L" " << endl;
		}
	}

	void Editor::CopyObject()
	{
		// オブジェクトのロック
		auto objA = m_arrow.lock();
		auto mark = m_mark.lock();
		if (!objA)return;
		auto arrow = dynamic_pointer_cast<Arrow>(objA);
		if (!arrow || !mark)return;

		// デバイスの確保
		auto device = App::GetApp()->GetInputDevice();
		auto keyState = device.GetKeyState();

		// 選択状態のオブジェクトの確認
		auto obj = m_pickObj.lock();
		auto object = dynamic_pointer_cast<GameObjectForEdit>(obj);
		if (object)
		{
			// オブジェクトの情報を保存
			auto objTrans = object->GetComponent<Transform>();
			auto pos = objTrans->GetPosition();
			auto rot = m_objRotBase;
			auto scale = objTrans->GetScale();
			Vec3 velocity = Vec3(0);
			float luncherFire = 0;
			float luncherFade = 0;
			float luncherDelay = 0;

			if (keyState.m_bPressedKeyTbl[VK_LBUTTON] && keyState.m_bPushKeyTbl[VK_MENU])
			{
				int ID = CheckObjectID(obj);
				shared_ptr<GameObject> copy;

				switch (ID)
				{
				case 1:
					copy = GetStage()->AddGameObject<Ground>(scale, rot, pos);
					break;

				default:
					break;
				}

				if (copy)
				{
					copy->SetUpdateActive(false);
					ResetPickUpObject();
					auto copyObj = dynamic_pointer_cast<GameObjectForEdit>(copy);
					if (!copyObj)return;
					// 選択されたオブジェクトの色の変更、座標などの保存、マウスの保存、XYZの矢印の描画のONにする
					SetObjectEmissive(copyObj, Col4(1.0f, 1.0f, 0.0f, 1.0f), true);
					m_objPos = copyObj->GetComponent<Transform>()->GetPosition();
					m_objScale = copyObj->GetComponent<Transform>()->GetScale();
					m_objRot = copyObj->GetMemberRotation();
					m_objRotBase = m_objRot;
					GetMouseRay(m_pastMauseRay[0], m_pastMauseRay[1]);
					arrow->SetDrawActive(true);
					arrow->SetArrowPosition(m_objPos);
					m_pickObj = copyObj;
				}

			}

			if (keyState.m_bPushKeyTbl[VK_LBUTTON])
			{

			}
		}
	}

	// ステージのロードの切り替えをやれええええええ
	void Editor::ChangeStage()
	{
		// バイナリファイルの取得
		auto path = App::GetApp()->GetDataDirWString();

		// ステージの番号を取得
		int loadNum = App::GetApp()->GetScene<Scene>()->GetLoadStageNumber();

		
		auto loader = GetStage()->AddGameObject<StageLoader>();

		// ステージのロード
		loader->LoadStageFile(path + L"Stages\\stage_" + to_wstring(loadNum) + L".stg");

	}

	int Editor::CheckObjectID(const shared_ptr<GameObject> obj)
	{
		int id = 0;
		auto ground = dynamic_pointer_cast<Ground>(obj);

		if (ground) id = 1;

		return id;
	}
}
//end basecross
