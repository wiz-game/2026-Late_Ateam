/*!
@file StageLoader.h
@brief ステージのエディター
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class StageLoader : public GameObject;
	//--------------------------------------------------------------------------------------
	class StageLoader : public GameObject
	{
	public:
		// 構築と破棄
		StageLoader(const shared_ptr<Stage>& stage) :
			GameObject(stage)
		{}
		virtual ~StageLoader()
		{}

		//virtual void OnCreate() override; // 初期化
		//virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

		//ステージのデータをバイナリファイルで保存する
		void LoadStageFile(const wstring& filename);
	};

}
//end basecross
