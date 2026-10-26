/*!
@file Scene.h
@brief シーン
*/
#pragma once

#include "stdafx.h"

namespace basecross{

	//--------------------------------------------------------------------------------------
	///	ゲームシーン
	//--------------------------------------------------------------------------------------
	class Scene : public SceneBase
	{
		//--------------------------------------------------------------------------------------
		/*!
		@brief リソースの作成
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
	private:
		int m_StageNum; // ステージの番号
		int m_loadStageNumber; // 読み込むステージの番号
		bool m_isStartFirst;
		Vec3 m_checkpointPos;

		void CreateResourses();
		int m_SrageNum;


	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief コンストラクタ
		*/
		//--------------------------------------------------------------------------------------
		Scene() :SceneBase(),
			m_SrageNum(1),
			m_isStartFirst(true)
		{}
		int GetStageNum() const {
			return m_SrageNum;
		}
		void SetStageNum(int i) {
			m_SrageNum = i;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~Scene();
		//--------------------------------------------------------------------------------------
		/*!
		@brief 初期化
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate() override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief イベント取得
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnEvent(const shared_ptr<Event>& event) override;

		// ロードするステージのナンバーを取得
		int GetLoadStageNumber() { return m_loadStageNumber; }

		// ロードするステージのナンバーを設定
		void SetLoadStageNumber(int stagenumber) { m_loadStageNumber = stagenumber; }
		void SetCheckpointPos(const Vec3& pos) { m_checkpointPos = pos; }
		void SetIsFirstStart(bool b) { m_isStartFirst = b; }


	};

}

//end basecross
