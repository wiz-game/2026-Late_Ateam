/*!
@file RayHitCheck.cpp
@brief レイのあたり判定の実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	bool RayHitCheck::HitTest(const shared_ptr<GameObject>& obj, const Vec3& start, const Vec3& end, Vec3& hit, bool isIgnore)
	{
		if (isIgnore)
		{
			//判定をとる必要がないほど遠いオブジェクトは無視する
			if (!CheckRange(obj, start, end))
			{
				return false;
			}

			// 判定を取らなくてよい方向のオブジェクトは無視する
			if (!CheckDirection(obj, start, end))
			{
				return false;
			}
		}
		//ステージからオブジェクトの配列を受け取りそれぞれの判定をする
		TRIANGLE triangle;
		size_t triangleIndex;
		shared_ptr<DrawComponent> baseDraw = obj->GetComponent<DrawComponent>(false);
		auto BcDrawComp = dynamic_pointer_cast<BcBaseDraw>(baseDraw);
		auto SmDrawComp = dynamic_pointer_cast<SmBaseDraw>(baseDraw);
		bool isHit = false;
		if (BcDrawComp)
		{
			//ポリゴンとの判定をとる
			isHit = BcDrawComp->HitTestStaticMeshSegmentTriangles(start, end, hit, triangle, triangleIndex);
		}
		if (SmDrawComp)
		{
			//ポリゴンとの判定をとる
			isHit = SmDrawComp->HitTestStaticMeshSegmentTriangles(start, end, hit, triangle, triangleIndex);
		}
		return isHit;
		SmDrawComp->SetMeshResource(L"");
	}

	bool RayHitCheck::CheckDirection(const shared_ptr<GameObject>& obj, const Vec3& start, const Vec3& end)
	{
		// 判定を取らなくてよい方向のオブジェクトは無視する
		auto objTrans = obj->GetComponent<Transform>();
		auto objPos = objTrans->GetPosition();
		auto objScale = objTrans->GetScale();
		Vec3 SEdis = end - start;
		bool Xp = SEdis.x <= 0 && (objPos.x - (objScale.x / 2.0f)) < start.x;
		bool Xm = SEdis.x > 0 && (objPos.x + (objScale.x / 2.0f)) > start.x;
		bool Yp = SEdis.y <= 0 && (objPos.y - (objScale.y / 2.0f)) < start.y;
		bool Ym = SEdis.y > 0 && (objPos.y + (objScale.y / 2.0f)) > start.y;
		bool Zp = SEdis.z <= 0 && (objPos.z - (objScale.z / 2.0f)) < start.z;
		bool Zm = SEdis.z > 0 && (objPos.z + (objScale.z / 2.0f)) > start.z;

		if ((!Xp && !Xm) || (!Yp && !Ym) || (!Zp && !Zm))
		{
			return false;
		}
		return true;
	}

	bool RayHitCheck::CheckRange(const shared_ptr<GameObject>& obj, const Vec3& start, const Vec3& end)
	{
		//判定をとる必要がないほど遠いオブジェクトは無視する
		auto objTrans = obj->GetComponent<Transform>();
		auto objPos = objTrans->GetPosition();
		auto objScale = objTrans->GetScale();
		Vec3 disVec = objPos - start;
		float dis = disVec.length() - objScale.length();
		if (dis > Vec3(end - start).length())
		{
			return false;
		}
		return true;
	}

	bool RayHitCheck::CheckDirection(const Vec3& pos, const Vec3& scale, const Vec3& start, const Vec3& end)
	{
		// 判定を取らなくてよい方向のオブジェクトは無視する
		Vec3 SEdis = end - start;
		bool Xp = SEdis.x <= 0 && (pos.x - (scale.x / 2.0f)) < start.x;
		bool Xm = SEdis.x > 0 && (pos.x + (scale.x / 2.0f)) > start.x;
		bool Yp = SEdis.y <= 0 && (pos.y - (scale.y / 2.0f)) < start.y;
		bool Ym = SEdis.y > 0 && (pos.y + (scale.y / 2.0f)) > start.y;
		bool Zp = SEdis.z <= 0 && (pos.z - (scale.z / 2.0f)) < start.z;
		bool Zm = SEdis.z > 0 && (pos.z + (scale.z / 2.0f)) > start.z;

		if ((!Xp && !Xm) || (!Yp && !Ym) || (!Zp && !Zm))
		{
			return false;
		}
		return true;
	}

	bool RayHitCheck::CheckRange(const Vec3& pos, const Vec3& scale, const Vec3& start, const Vec3& end)
	{
		//判定をとる必要がないほど遠いオブジェクトは無視する
		Vec3 disVec = pos - start;
		float dis = disVec.length() - scale.length();
		if (dis > Vec3(end - start).length())
		{
			return false;
		}
		return true;
	}

	bool RayHitCheck::HitTestVirtualSphere(const shared_ptr<GameObject>& obj, const Vec3& start, const Vec3& end, Vec3& hit, float& distance, float radius, bool isIgnore)
	{
		if (isIgnore)
		{
			//判定をとる必要がないほど遠いオブジェクトは無視する
			if (!CheckRange(obj, start, end))
			{
				return false;
			}

			// 判定を取らなくてよい方向のオブジェクトは無視する
			if (!CheckDirection(obj, start, end))
			{
				return false;
			}
		}

		// 仮の球体との判定をとる
		float length = 0;
		auto objPos = obj->GetComponent<Transform>()->GetPosition();
		auto objScale = obj->GetComponent<Transform>()->GetScale().x;
		float a = end.x - start.x;
		float b = end.y - start.y;
		float c = end.z - start.z;
		float x1 = start.x - objPos.x;
		float y1 = start.y - objPos.y;
		float z1 = start.z - objPos.z;

		float t = -((a * x1) + (b * y1) + (c * z1));
		float abc = (a * a) + (b * b) + (c * c);
		if (t < 0)
		{
			length = (x1 * x1) + (y1 * y1) + (z1 * z1);
			length = sqrtf(length);
		}
		else if (t > abc)
		{
			length = Vec3(end - objPos).length();
		}
		else
		{
			float l1 = (a * y1) - (b * x1);
			float l2 = (b * z1) - (c * y1);
			float l3 = (a * z1) - (c * x1);
			length = ((l1 * l1) + (l2 * l2) + (l3 * l3)) / abc;
			length = sqrtf(length);
		}

		distance = length;
		if (length < radius)
		{
			hit = objPos;
			return true;
		}
		return false;
	}

	bool RayHitCheck::HitTestVirtualSphere(const Vec3& pos, const Vec3& start, const Vec3& end, Vec3& hit, float& distance, float radius, bool isIgnore)
	{
		if (isIgnore)
		{
			//判定をとる必要がないほど遠いオブジェクトは無視する
			if (!CheckRange(pos, Vec3(radius), start, end))
			{
				return false;
			}

			// 判定を取らなくてよい方向のオブジェクトは無視する
			if (!CheckDirection(pos, Vec3(radius), start, end))
			{
				return false;
			}
		}

		// 仮の球体との判定をとる
		float length = 0;
		float a = end.x - start.x;
		float b = end.y - start.y;
		float c = end.z - start.z;
		float x1 = start.x - pos.x;
		float y1 = start.y - pos.y;
		float z1 = start.z - pos.z;

		float t = -((a * x1) + (b * y1) + (c * z1));
		float abc = (a * a) + (b * b) + (c * c);
		if (t < 0)
		{
			length = (x1 * x1) + (y1 * y1) + (z1 * z1);
			length = sqrtf(length);
		}
		else if (t > abc)
		{
			length = Vec3(end - pos).length();
		}
		else
		{
			float l1 = (a * y1) - (b * x1);
			float l2 = (b * z1) - (c * y1);
			float l3 = (a * z1) - (c * x1);
			length = ((l1 * l1) + (l2 * l2) + (l3 * l3)) / abc;
			length = sqrtf(length);
		}

		distance = length;
		if (length < radius)
		{
			hit = pos;
			return true;
		}
		return false;
	}

	bool RayHitCheck::HitTestVirtualCube(const Vec3& pos, const Vec3& start, const Vec3& end, Vec3& hit, float& distance, const Vec3& scale, bool isIgnore)
	{
		if (isIgnore)
		{
			//判定をとる必要がないほど遠いオブジェクトは無視する
			if (!CheckRange(pos, scale, start, end))
			{
				return false;
			}

			// 判定を取らなくてよい方向のオブジェクトは無視する
			if (!CheckDirection(pos, scale, start, end))
			{
				return false;
			}
		}

		auto scaleh = scale / 2.0f;
		int startInX = pos.x + scaleh.x >= start.x ? (pos.x - scaleh.x <= start.x ? 0 : -1) : 1;
		int startInY = pos.y + scaleh.y >= start.y ? (pos.y - scaleh.y <= start.y ? 0 : -1) : 1;
		int startInZ = pos.z + scaleh.z >= start.z ? (pos.z - scaleh.z <= start.z ? 0 : -1) : 1;
		int endInX = pos.x + scaleh.x >= end.x ? (pos.x - scaleh.x <= end.x ? 0 : -1) : 1;
		int endInY = pos.y + scaleh.y >= end.y ? (pos.y - scaleh.y <= end.y ? 0 : -1) : 1;
		int endInZ = pos.z + scaleh.z >= end.z ? (pos.z - scaleh.z <= end.z ? 0 : -1) : 1;
		bool startIn = (abs(startInX) + abs(startInY) + abs(startInZ)) == 0;
		bool endIn = (abs(endInX) + abs(endInY) + abs(endInZ)) == 0;
		if (!startIn && endIn)
		{
			if (startInY == 0 && startInZ == 0)
			{
				switch (startInX)
				{
				case -1:
					hit.x = pos.x - scaleh.x;
					break;
				case 1:
					hit.x = pos.x + scaleh.x;
					break;
				default:
					break;
				}
				float per = abs(hit.x - start.x / end.x - start.x);
			}
			if (startInX == 0 && startInZ == 0)
			{
				switch (startInY)
				{
				case -1:
					hit.y = pos.y - scaleh.y;
					break;
				case 1:
					hit.y = pos.y + scaleh.y;
					break;
				default:
					break;
				}
			}
			if (startInY == 0 && startInX == 0)
			{
				switch (startInZ)
				{
				case -1:
					hit.z = pos.z - scaleh.z;
					break;
				case 1:
					hit.z = pos.z + scaleh.z;
					break;
				default:
					break;
				}
			}
			return true;
		}

		if (startIn && !endIn)
		{
			if (endInY == 0 && endInZ == 0)
			{
				switch (endInX)
				{
				case -1:
					hit.x = pos.x - scaleh.x;
					break;
				case 1:
					hit.x = pos.x + scaleh.x;
					break;
				default:
					break;
				}
			}
			if (endInX == 0 && endInZ == 0)
			{
				switch (endInY)
				{
				case -1:
					hit.y = pos.y - scaleh.y;
					break;
				case 1:
					hit.y = pos.y + scaleh.y;
					break;
				default:
					break;
				}
			}
			if (endInY == 0 && endInX == 0)
			{
				switch (endInZ)
				{
				case -1:
					hit.z = pos.z - scaleh.z;
					break;
				case 1:
					hit.z = pos.z + scaleh.z;
					break;
				default:
					break;
				}
			}
			return true;
		}
		return false;
	}
}
//end basecross
