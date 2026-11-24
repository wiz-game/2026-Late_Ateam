#pragma once
#include "stdafx.h"

#include "Enemy.h"

namespace basecross 
{
	class EnemySight : public GameObject
	{
		std::shared_ptr<Transform> m_transComp;
		std::shared_ptr<PCStaticDraw> m_drawComp;
		std::weak_ptr<Enemy> m_enemy;

		float m_innerAngle;	//両眼視界の角度
		float m_outerAngle;	//単眼を含めた全体の角度

	public:
		EnemySight(const std::shared_ptr<Stage>& stage,
			std::shared_ptr<Enemy>& enemy,
			float innerAngle,
			float outerAngle
		) :
			GameObject(stage),
			m_enemy(enemy),
			m_innerAngle(innerAngle),
			m_outerAngle(innerAngle + outerAngle * 2.0f)
		{}
		virtual ~EnemySight()
		{}

		virtual void OnCreate() override;
		virtual void OnUpdate() override;
	};
}