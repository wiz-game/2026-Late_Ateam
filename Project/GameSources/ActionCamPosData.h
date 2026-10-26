/*!
@file ActionCamPosData.h
@brief
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	class ActionCamPosData 
	//--------------------------------------------------------------------------------------

	struct ActionCamPosData
	{
		Vec3 ActionPos;
		float Radius;

		Vec3 RoomCenter;
	};

	class ActionCamInfo
	{
	public:
		static std::vector<ActionCamPosData> Create();

	};
}
//end basecross
