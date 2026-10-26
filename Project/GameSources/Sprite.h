/*!
@file Sprite.h
@brief スプライト
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class Sprite : public GameObject
	{
		bool m_Trace;
		Vec2 m_StartScale;
		Vec2 m_StartPos;
		wstring m_TextureKey;

	public:
		// 構築と破棄
		Sprite(const shared_ptr<Stage>& StagePtr, const wstring& TextureKey, bool Trace,
			const Vec2& StartScale, const Vec2& StartPos);
		virtual ~Sprite()
		{}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画
		float m_TotalTime;
		bool m_Selected;
		bool IsSelect() const {
			return m_Selected;
		}
		void SetSelect(bool b) {
			m_Selected = b;
			m_TotalTime = 0.0f;
		}

	};

}
//end basecross
