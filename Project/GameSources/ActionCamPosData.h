/*!
@file ActionCamPosMark.h
@brief
*/

#pragma once
#include "stdafx.h"
#include "ActionCamPos.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	class ActionCamPosMark 
	//--------------------------------------------------------------------------------------
	class ActionCamPosMark : public GameObject
	{
		std::shared_ptr<PNTStaticDraw> m_drawComp;
		std::shared_ptr<Transform> m_transComp;

	private:
		std::weak_ptr<ActionCamPos> m_awner;

	public:
		ActionCamPosMark(const shared_ptr<Stage>& stage):
			GameObject(stage)
		{}

		void SetOwner(const std::shared_ptr<ActionCamPos>& awner)
		{
			m_awner = awner;
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

		void SetDrawActive(bool active)
		{
			if (m_drawComp)
			{
				m_drawComp->SetDrawActive(active);
			}
		}
	};
}
//end basecross
