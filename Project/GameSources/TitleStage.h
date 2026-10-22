/*!
@file TitleStage.h
@brief ゲームステージ
*/

#pragma once
#include "stdafx.h"

namespace basecross {	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class TitleStage : public Stage
	{

		void CreateViewLight(); //ビューの作成

		void CreateTitleSprite();


	public:
		//構築と破棄
		TitleStage() :Stage() {}
		virtual ~TitleStage() {}

		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新

	};

}
//end basecross
