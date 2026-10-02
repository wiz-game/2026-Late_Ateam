/*!
@file RayHitCheck.h
@brief レイの当たり判定を確認
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class RayHitCheck 
	//--------------------------------------------------------------------------------------
	class RayHitCheck
	{
		static bool CheckDirection(const shared_ptr<GameObject>& obj, const Vec3& start, const Vec3& end);
		static bool CheckRange(const shared_ptr<GameObject>& obj, const Vec3& start, const Vec3& end);
		static bool CheckDirection(const Vec3& pos, const Vec3& scale, const Vec3& start, const Vec3& end);
		static bool CheckRange(const Vec3& pos, const Vec3& scale, const Vec3& start, const Vec3& end);

	public:
		static bool HitTest(const shared_ptr<GameObject>& obj, const Vec3& start, const Vec3& end, Vec3& hit, bool isIgnore = true);
		static bool HitTestVirtualSphere(const shared_ptr<GameObject>& obj, const Vec3& start, const Vec3& end, Vec3& hit, float& distance, float radius = 1.0f, bool isIgnore = true);
		static bool HitTestVirtualSphere(const Vec3& pos, const Vec3& start, const Vec3& end, Vec3& hit, float& distance, float radius = 1.0f, bool isIgnore = true);
		static bool HitTestVirtualCube(const Vec3& pos, const Vec3& start, const Vec3& end, Vec3& hit, float& distance, const Vec3& scale = Vec3(1.0f), bool isIgnore = true);

	};

}
//end basecross
