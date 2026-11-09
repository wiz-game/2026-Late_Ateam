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
	class AnimUI : public GameObject
	{
		static constexpr float DEFAULT_Z_POS = 0.0f; // デフォルトのZ位置
		static constexpr float DEFAULT_Z_SCALE = 1.0f; // デフォルトのZスケール

		wstring m_TextureKey; // テクスチャキー
		bool m_AlphaActive; // アルファを有効にするかどうか
		Vec2 m_StartScale; // 初期スケール
		Vec2 m_StartPos; // 初期位置


	public:
		// 構築と破棄
		AnimUI(const shared_ptr<Stage>& StagePtr,
			const wstring& TextureKey,
			bool AlphaActive,
			const Vec2& StartScale,
			const Vec2& StartPos);
		virtual ~AnimUI()
		{}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

		float m_UVMin = 0.0f; // UVの最小値
		float m_UVMax = 1.0f; // UVの最大値
		Col4 m_Color = Col4(1.0f); // 色の情報
	};

}
//end basecross
