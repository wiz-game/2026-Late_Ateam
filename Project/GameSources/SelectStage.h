/*!
@file SelectStage.h
@brief セレクトステージ
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	セレクトステージクラス
	//--------------------------------------------------------------------------------------
	class SelectStage : public Stage
	{
		void CreateViewLight(); //ビューの作成
		
	public:
		//構築と破棄
		SelectStage() :Stage() {}
		virtual ~SelectStage() {}

		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新

	};
}
//end basecross

