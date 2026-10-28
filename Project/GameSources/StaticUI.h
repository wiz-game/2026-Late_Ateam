/*!
@file StaticUI.h
@brief 静的なUI
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class StaticUI : public Sprite;
	//--------------------------------------------------------------------------------------
	class StaticUI : public Sprite
	{
	public:
		// 構築と破棄
		StaticUI(const shared_ptr<Stage>& StagePtr, const wstring& TextureKey, bool Trace,
			const Vec2& StartScale, const Vec2& StartPos);
		virtual ~StaticUI()
		{}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

	};

}
//end basecross
