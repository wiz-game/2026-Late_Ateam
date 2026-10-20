/*!
@file ClearStage.h
@brief クリアステージ
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	クリアステージクラス
	//--------------------------------------------------------------------------------------
	class ClearStage : public Stage
	{
		void CreateViewLight(); //ビューの作成

	public:
		//構築と破棄
		ClearStage() :Stage() {}
		virtual ~ClearStage() {}

		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新

	};
}
//end basecross

