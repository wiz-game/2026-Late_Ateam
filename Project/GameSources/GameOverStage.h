/*!
@file GameOverStage.h
@brief ゲームオーバーステージ
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゲームオーバーステージクラス
	//--------------------------------------------------------------------------------------
	class Sprite;
	
	class GameOverStage : public Stage
	{
		void CreateViewLight(); //ビューの作成

		void CreateGameOverSprite();
		bool m_CntrolLock;
		int m_SelectStage;
		//トータル時間
		float m_TotalTime;
		//選択されているかどうか
		bool m_Selected;

		//スプライトのweak_ptrの配列
		vector<weak_ptr<Sprite>> m_SpVec;
		//選択がチェンジされた
		void ChangeSelect(int num);


	public:
		//構築と破棄
		GameOverStage() :Stage(), m_CntrolLock(false),
			m_SpVec(2)
		{}
		virtual ~GameOverStage() {}

		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新

	};
}
//end basecross

