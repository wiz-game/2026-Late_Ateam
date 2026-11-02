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
	class StaticUI : public GameObject
	{
	private:
		static constexpr float SPRITE_HALF_SIZE = 0.5f; // スプライトの半分のサイズ
		static constexpr float DEFAULT_Z_POS = 0.0f; // デフォルトのZ位置
		static constexpr float DEFAULT_Z_SCALE = 1.0f; // デフォルトのZスケール
		static constexpr float UV_MIN = 0.0f; // UVの最小値
		static constexpr float UV_MAX = 1.0f; // UVの最大値
		static constexpr float COLOR_WHITE = 1.0f; // 白色の色情報	


		wstring m_TextureKey; // テクスチャキー
		bool m_AlphaActive; // アルファを有効にするかどうか
		Vec2 m_StartScale; // 初期スケール
		Vec2 m_StartPos; // 初期位置

	public:
		// 構築と破棄
		StaticUI(const shared_ptr<Stage>& StagePtr, 
			const wstring& TextureKey, 
			bool AlphaActive,
			const Vec2& StartScale, 
			const Vec2& StartPos);
		virtual ~StaticUI()
		{}

		virtual void OnCreate() override; // 初期化
		//virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

	};

}
//end basecross
