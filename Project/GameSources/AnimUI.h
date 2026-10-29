/*!
@file 動的UI.h
@brief 動的なUI
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class AnimUI : public Sprite;
	//--------------------------------------------------------------------------------------
	class AnimUI : public Sprite
	{
	public:
		// 構築と破棄
		AnimUI(const shared_ptr<Stage>& StagePtr, const wstring& TextureKey, bool Trace,
			const Vec2& StartScale, const Vec2& StartPos);
		virtual ~AnimUI()
		{}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

	};

}
//end basecross
