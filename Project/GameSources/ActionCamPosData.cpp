/*!
@file ActionCamPosData.cpp
@brief
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	std::vector<ActionCamPosData>ActionCamInfo::Create()
	{
		std::vector<ActionCamPosData> postion;

		postion.push_back(
			{
				Vec3(10.0f, 0.0f,10.0f),
				10.0f,
				Vec3(10.0f,0.0f,0.0f)
			}
		);

		return postion;
	}
}
//end basecross
