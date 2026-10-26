/*!
@file WayPointManager.cpp
@brief WayPointManagerの実体
*/

#include "stdafx.h"
#include "WayPointManager.h"
#include "WayPoint.h"
#include "Enemy.h"

namespace basecross {

	void WayPointManager::OnUpdate()
	{
		if (m_dirty) 
		{ 
			Rebuild();
			m_dirty = false;
		}
	}

	void WayPointManager::Rebuild()
	{
		map<int, vector<pair<int, Vec3>>> tmp;
		for (auto& obj : GetStage()->GetGameObjectVec())
		{
			if (auto wp = dynamic_pointer_cast<WayPoint>(obj))
			{
				//groupIDごとに、orderと座標のペアを集める
				tmp[wp->GetID()].push_back(
					std::make_pair(wp->GetOrder(), wp->GetTrans()->GetPosition()));
			}
		}

		m_routes.clear();
		for (auto& kv : tmp)
		{
			auto& v = kv.second;

			//配列を指定したルールで並べ替える
			stable_sort(v.begin(), v.end(),	//最初から最後まで
				[](const std::pair<int, Vec3>& a, const std::pair<int, Vec3>& b)
				{
					return a.first < b.first; //orderの小さい順に並べる
				});

			//座標だけを取り出してm_routesに入れる
			for (auto& p : v)
			{
				m_routes[kv.first].push_back(p.second);
			}
		}
	}

	//グループごとの点の数を返す
	size_t WayPointManager::GetCount(int groupId)const
	{
		auto it = m_routes.find(groupId);
		return it == m_routes.end() ? 0 : it->second.size();
	}

	//表から座標を取り出しす関数
	Vec3 WayPointManager::GetPoint(int groupId, size_t index) const
	{
		auto it = m_routes.find(groupId);
		if (it == m_routes.end() || index >= it->second.size())return Vec3(0.0f);
		return it->second[index];
	}

	//groupIdごとに表示非表示できるようにする関数
	void WayPointManager::ShowOnlyGroup(int groupId)
	{
		for (auto& obj : GetStage()->GetGameObjectVec())
		{
			if (auto wp = dynamic_pointer_cast<WayPoint>(obj))
			{
				wp->SetDrawActive(groupId >= 0 && wp->GetID() == groupId);
			}
		}
	}

	//Enemyに空いているgroupIDの最小値を返す（Enemyが使用中のIDは除外）
	int WayPointManager::FindFreeGroupId() const
	{
		std::set<int> used;
		for (auto& obj : GetStage()->GetGameObjectVec())
		{
			if (auto en = dynamic_pointer_cast<Enemy>(obj))
			{
				used.insert(en->GetGroupID());//EnemyのgroupIdを登録
			}
		}
		int id = 0;
		while (used.count(id))id++;	//空いている最小の番号を探す
		return id;
	}

	//そのグループで使うorder(現在の最大値＋１)
	int WayPointManager::GetNextOrder(int groupId) const
	{
		int maxOrder = -1;
		for (auto& obj : GetStage()->GetGameObjectVec())
		{
			if (auto wp = dynamic_pointer_cast<WayPoint>(obj))
			{
				if (wp->GetID() == groupId)
					maxOrder = (std::max)(maxOrder, wp->GetOrder());
			}
		}
		return maxOrder + 1;
	}
};