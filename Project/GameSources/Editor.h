/*!
@file Editor.h
@brief 汎用ステージエディター（Wall / Floor のみ対応）
*/

#pragma once
#include "stdafx.h"

namespace basecross {

    class Editor : public GameObject
    {
        // 選択中オブジェクト
        weak_ptr<GameObject> m_pickObj;

        // 選択前の Emissive 色
        Col4 m_picObjCol;

        // カメラ
        shared_ptr<Camera> m_prevCamera;
        shared_ptr<Camera> m_editorCamera;

        // エディターモードかどうか
        bool m_isEditorMode;

        // 0=Position, 1=Scale, 2=Rotation
        int m_changeTransform;

        // マウスレイの過去値
        Vec3 m_pastMauseRay[2];

        // 編集対象の Transform 値
        Vec3 m_objPos;
        Vec3 m_objScale;
        Vec3 m_objRot;
        Vec3 m_objRotBase;

        // 追加オブジェクト番号（1=Wall, 2=Floor）
        int m_addObjNum;

        // スナップサイズ
        float m_snapSize;

        // ステージナンバー
        int m_stageNum;

        // XYZ矢印
        weak_ptr<GameObject> m_arrow;

        // マーカー（今回は使わないが残す）
        weak_ptr<GameObject> m_mark;

        // オブジェクト選択処理
        shared_ptr<GameObject> PickUpGameObject();


        // 入力ベクトル取得
        Vec3 GetInputVec();

        // 選択解除
        void ResetPickUpObject();

        // 選択オブジェクト編集
        void MovePickUpObject(wstringstream& ss);

        // 複製処理
        void CopyObject();

        // ステージの切り替え
        void ChangeStage();

        // オブジェクトID判定（Wall / Floor）
        int CheckObjectID(const shared_ptr<GameObject> obj);

    public:
        Editor(const shared_ptr<Stage>& stage)
            : GameObject(stage),
            m_isEditorMode(false),
            m_changeTransform(0),
            m_addObjNum(1),      // 初期は Wall
            m_snapSize(1.0f)
        {}

        virtual ~Editor() {}

        virtual void OnCreate() override;
        virtual void OnUpdate() override;

        // Emissive変更
        void SetObjectEmissive(const shared_ptr<GameObject>& obj, const Col4& col, bool saveOld);

        // マウスレイ取得
        void GetMouseRay(Vec3& start, Vec3& end);

        // 新規オブジェクト追加（Wall / Floor）
        void AddNewObject();

        // ステージ保存（必要なら Editor.cpp に実装）
        void SaveBinaryFile(const std::wstring& filename);
    };

} // namespace basecross