/*!
@file CameraState.h
@brief カメラのステートなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	// 通常時のカメラのステート
	class CameraNormalMoveState : public ObjState<MainCamera>
	{
		CameraNormalMoveState() {}
	public:
		static shared_ptr<CameraNormalMoveState> Instance();
		virtual void Enter(const shared_ptr<MainCamera>& obj) override;
		virtual void Execute(const shared_ptr<MainCamera>& obj) override;
		virtual void Exit(const shared_ptr<MainCamera>& obj) override;
	};

	// エディターモード時のカメラのステート
	class EditorCameraState : public ObjState<MainCamera>
	{
		EditorCameraState() {}
	public:
		static shared_ptr<EditorCameraState> Instance();
		virtual void Enter(const shared_ptr<MainCamera>& obj) override;
		virtual void Execute(const shared_ptr<MainCamera>& obj) override;
		virtual void Exit(const shared_ptr<MainCamera>& obj) override;
	};

}
//end basecross

