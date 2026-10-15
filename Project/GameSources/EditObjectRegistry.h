/*!
@file Scene.h
@brief シーン
*/
#pragma once

#include "stdafx.h"
#include "Project.h"

namespace basecross {


    struct EditObjectInfo
    {
        uint16_t id;                 // ファイルに書く ID（一度決めたら変えない）
        wstring  name;               // 画面表示用の名前
        type_index type;             // クラスの型（オブジェクトから ID を引く用）
        function<shared_ptr<GameObjectForEdit>(const shared_ptr<Stage>&,
            const Vec3& scale, const Vec3& rot, const Vec3& pos)> create; // 生成関数
    };

    class EditObjectRegistry 
    {
        vector<EditObjectInfo> m_infos;   // 登録順 = ←→キーで切り替える順
    public:
        // 最初に使われたときに生成（静的初期化の順序の問題を避ける）
        static EditObjectRegistry& Get() { static EditObjectRegistry inst; return inst; }
        void Register(const EditObjectInfo& info);              // ID の重複もここで検出
        const EditObjectInfo* FindByID(uint16_t id) const;
        const EditObjectInfo* FindByObject(const shared_ptr<GameObject>& obj) const; // typeid で検索
        const vector<EditObjectInfo>& GetAll() const { return m_infos; }
    };

    template<typename T>
    struct EditObjectRegistrar
    {
        EditObjectRegistrar(uint16_t id, const wstring& name)
        {
            EditObjectRegistry::Get().Register(
            {
                id,
                name,
                type_index(typeid(T)),
                [](const shared_ptr<Stage>& s, const Vec3& sc, const Vec3& r, const Vec3& p)->shared_ptr<GameObjectForEdit>
                {
                    return s->AddGameObject<T>(sc, r, p);
                }
            });
        }
    };

#define REGISTER(Type, ID, Name) \
    static EditObjectRegistrar<Type> s_editRegistrar_##Type(ID, Name)
}