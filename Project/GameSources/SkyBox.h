/*!
@file SkyBox.h
@brief スカイボックス
*/

#pragma once
#include "stdafx.h"

namespace basecross
{
	class SkyBox : public GameObject
	{
	public:
		// テクスチャキー(アセット名)と配置座標を関連図ける連想配列
		static const map<wstring, Vec3> pairs;

	private:
		// スカイキューブの６面を管理する
		vector<shared_ptr<GameObject>> m_planes;

	public:
		SkyBox(const shared_ptr<Stage>& stage);
		virtual ~SkyBox();

		virtual void OnCreate() override;
		virtual void OnUpdate() override;
		virtual void OnDraw() override;
	};

	class Plane : public GameObject
	{
	public:
		Plane(const shared_ptr<Stage>& stage);
		~Plane();

		void OnCreate() override;
	};
}