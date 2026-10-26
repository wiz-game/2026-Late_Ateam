/*!
@file WayPointManager.h
@brief WayPointをまとめるクラスなど
*/

#pragma once
#include "stdafx.h"

namespace basecross
{
	//--------------------------------------------------------------------------------------
	//	class WayPoint : public GameObjectForEdit;
	//--------------------------------------------------------------------------------------
	class WayPointManager : public GameObject
	{
		//全WyaPointを調べて、グループごとに・順番通りに並べた座標表
		map<int, vector<Vec3>> m_routes; //Enemyが参照するキャッシュ

	public:

		WayPointManager(const shared_ptr<Stage>& stage):
			GameObject(stage)
		{}
		virtual ~WayPointManager()
		{}

		//virtual void OnCreate() override; // 初期化
		//virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

		//全WyaPointを調べてm_routesを作り直す
		void Rebuild();

		//Enemyが使う窓口
		size_t GetCount(int groupId) const;//グループごとの点の数を返す
		Vec3 GetPoint(int groupId, size_t index) const;//表から座標を取り出しす関数

		//WayPointが使う窓口
		int FindFreeGroupId() const;//EnemyにID番号を振り分ける
		int GetNextOrder(int groupId) const;//新しいWayPointに付ける番号
		void ShowOnlyGroup(int groupId);//-1で全部非表示
	};
}