/*!
@file ActionCamera.h
@brief アクションカメラ
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class ActionCamera : public GameObject
	{
	public:
		// 構築と破棄
		ActionCamera(const shared_ptr<Stage>& stage) :
			GameObject(stage)
		{}
		virtual ~ActionCamera()
		{}

		virtual void OnCreate() override; // 初期化
		//virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画
	};

}
//end basecross
