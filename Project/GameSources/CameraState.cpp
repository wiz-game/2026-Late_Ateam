/*!
@file Cable.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	// 通常時のカメラのステート-------------------------------------------------------------------------
	shared_ptr<CameraNormalMoveState> CameraNormalMoveState::Instance()
	{
		static shared_ptr<CameraNormalMoveState> instance(new CameraNormalMoveState);
		return instance;
	}

	void CameraNormalMoveState::Enter(const shared_ptr<MainCamera>& obj)
	{
		obj->InitParameter();
	}
	void CameraNormalMoveState::Execute(const shared_ptr<MainCamera>& obj)
	{
		obj->NormalMove(false);
	}
	void CameraNormalMoveState::Exit(const shared_ptr<MainCamera>& obj)
	{

	}

	// エディターモード時のカメラのステート-----------------------------------------------------------------
	shared_ptr<EditorCameraState> EditorCameraState::Instance()
	{
		static shared_ptr<EditorCameraState> instance(new EditorCameraState);
		return instance;
	}

	void EditorCameraState::Enter(const shared_ptr<MainCamera>& obj)
	{}
	void EditorCameraState::Execute(const shared_ptr<MainCamera>& obj)
	{
		obj->NormalMove(true);
	}
	void EditorCameraState::Exit(const shared_ptr<MainCamera>& obj)
	{

	}

}
//end basecross
