/*!
@file StaticUI.cpp
@brief 静的なUI実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	StaticUI::StaticUI(
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
	void StaticUI::OnCreate()
	{
		vector<VertexPositionColorTexture> vertices =
		{
			{
				VertexPositionColorTexture(
					Vec3(-SPRITE_HALF_SIZE, SPRITE_HALF_SIZE, DEFAULT_Z_POS),
					Col4(COLOR_WHITE),
					Vec2(UV_MIN, UV_MIN)
				)
			},
			{
				VertexPositionColorTexture(
					Vec3(SPRITE_HALF_SIZE, SPRITE_HALF_SIZE, DEFAULT_Z_POS),
					Col4(COLOR_WHITE),
					Vec2(UV_MAX, UV_MIN)
				)
			},
			{
				VertexPositionColorTexture(
					Vec3(-SPRITE_HALF_SIZE, -SPRITE_HALF_SIZE, DEFAULT_Z_POS),
					Col4(COLOR_WHITE),
					Vec2(UV_MIN, UV_MAX)
				)
			},
			{
				VertexPositionColorTexture(
					Vec3(SPRITE_HALF_SIZE, -SPRITE_HALF_SIZE, DEFAULT_Z_POS),
					Col4(COLOR_WHITE),
					Vec2(UV_MAX, UV_MAX)
				)
			}
		};

		vector<unsigned short> indices =
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

}
//end basecross
