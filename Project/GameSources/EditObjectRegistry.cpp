#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void EditObjectRegistry::Register(const EditObjectInfo& info)
	{
		// ID が重複していたら登録しない
		if (FindByID(info.id))
		{
			MessageBox(nullptr, L"EditObjectRegistry: ID が重複しています", L"Error", MB_OK);
			return;
		}
		m_infos.push_back(info);
		// 切り替え順を ID の昇順に固定する
		std::sort(m_infos.begin(), m_infos.end(),
			[](const EditObjectInfo& a, const EditObjectInfo& b) { return a.id < b.id; });
	}

	const EditObjectInfo* EditObjectRegistry::FindByID(uint16_t id) const
	{
		for (const auto& info : m_infos)
		{
			if (info.id == id) return &info;
		}
		return nullptr;
	}

	const EditObjectInfo* EditObjectRegistry::FindByObject(const shared_ptr<GameObject>& obj) const
	{
		if (!obj) return nullptr;
		const type_index t(typeid(*obj));   // 実際の型（派生クラス）で比べる
		for (const auto& info : m_infos)
		{
			if (info.type == t) return &info;
		}
		return nullptr;
	}

}
//end basecross