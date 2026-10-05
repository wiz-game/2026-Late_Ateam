/*!
@file MainCamera.h
@brief カメラなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	class MainCamera : public Camera
	{
	private:
		std::weak_ptr<GameObject> m_targetObj; // 注視するオブジェクト
		float m_distance; // 注視対象からカメラの距離
		float m_maxDis; // カメラの最大距離
		float m_minDis; // カメラの最短距離
		float m_angleY; // y軸中心のカメラの回り込み
		float m_hight; // カメラの高さ
		float m_angleX; //カメラの縦方向の回り込み
		float m_upperLimit; // カメラの方向の制限上
		float m_lowerLimit; // カメラの下方向の制限
		float m_closerdelta; // 近づくまでの猶予時間
		float m_leavedelta; // 遠ざかる時の移動に使う変数

		Point2D<int> m_pastPoint = { 0,0 };

		std::unique_ptr<StateMachine<MainCamera>> m_stateMachine; // カメラのステート

	public:
		MainCamera():
			m_distance(6),
			m_maxDis(6),
			m_minDis(2),
			m_angleY(XM_PIDIV2),
			m_hight(5),
			m_angleX(XM_PIDIV2),
			m_upperLimit(XM_PI / 10.0f),
			m_lowerLimit(XM_PI / 6.0f),
			m_closerdelta(0.0f),
			m_leavedelta(10.0f)
		{}

		virtual ~MainCamera()
		{}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新

		//通常状態の動き
		void NormalMove(bool isEditor);

		//通常状態になった時の初期化
		void InitParameter() { m_closerdelta = 0; };

		//注視するオブジェクトの指定
		void SetTargetObject(const shared_ptr<GameObject>& obj) { m_targetObj = obj; }
		
		//ステートマシンのゲッター
		const unique_ptr<StateMachine<MainCamera>>& GetStateMachine() { return m_stateMachine; }


	};
}
//end basecross

