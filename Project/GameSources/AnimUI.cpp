/*!
@file AnimUI.cpp
@brief 動的なUI実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	AnimUI::AnimUI(
		const shared_ptr<Stage>& StagePtr,
		const wstring& TextureKey,
		bool AlphaActive,
		const Vec2& StartScale,
		const Vec2& StartPos
	) :
		GameObject(StagePtr),
		m_TextureKey(TextureKey),
		m_AlphaActive(AlphaActive),
		m_StartScale(StartScale),
		m_StartPos(StartPos)
	{}

	//初期化
	void AnimUI::OnCreate()
	{
		vector<VertexPositionColorTexture> vertices =
		{
			{
				VertexPositionColorTexture(
					Vec3(-DEFAULT_Z_SCALE, DEFAULT_Z_SCALE, DEFAULT_Z_POS),
					m_Color,
					Vec2(m_UVMin, m_UVMin)
				)
			},
			{
				VertexPositionColorTexture(
					Vec3(DEFAULT_Z_SCALE, DEFAULT_Z_SCALE, DEFAULT_Z_POS),
					m_Color,
					Vec2(m_UVMax, m_UVMin)
				)
			},
			{
				VertexPositionColorTexture(
					Vec3(-DEFAULT_Z_SCALE, -DEFAULT_Z_SCALE, DEFAULT_Z_POS),
					m_Color,
					Vec2(m_UVMin, m_UVMax)
				)
			},
			{
				VertexPositionColorTexture(
					Vec3(DEFAULT_Z_SCALE, -DEFAULT_Z_SCALE, DEFAULT_Z_POS),
					m_Color,
					Vec2(m_UVMax, m_UVMax)
				)
			}
		};

		vector <unsigned short> indices =
		{
			0, 1, 2,
			1, 3, 2
		};

		SetAlphaActive(m_AlphaActive);

		auto transform = AddComponent<Transform>();

		transform->SetScale(Vec3(m_StartScale.x, m_StartScale.y, DEFAULT_Z_SCALE));
		transform->SetPosition(Vec3(m_StartPos.x, m_StartPos.y, DEFAULT_Z_POS));
		transform->SetRotation(Vec3(0.0f, 0.0f, 0.0f));

		auto draw = AddComponent<PCTSpriteDraw>(vertices, indices);
		draw->SetSamplerState(SamplerState::LinearClamp);
		draw->SetTextureResource(m_TextureKey);

	}

	void AnimUI::OnUpdate()
	{

	}

}
//end basecross
