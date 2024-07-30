#pragma once
#include "Engine_Defines.h"

class ENGINE_DLL CMathUtils
{
public:
	static Vector3 Slerp(const Vector3& vStartDir, const Vector3& vEndDir, _float t);
	static _float Dot(const Vector3& vSrc, const Vector3& vDst);
	// 내적의 사이각을 반환해주는 함수
	static _float RadFromDot(const Vector3& vSrc, const Vector3& vDst);
	static _float GetDistance(const Vector3& vTargetPos, const Vector3& vMyPos);
	static Vector4 HSVToRGB(const Vector4& vHSV);
	static _float lerp(const _float& a, const _float& b, const _float& t);
};

