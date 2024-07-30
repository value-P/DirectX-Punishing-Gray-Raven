#include "MathUtils.h"

Vector3 CMathUtils::Slerp(const Vector3& vStartDir, const Vector3& vEndDir, _float t)
{
	_float dot = vStartDir.Dot(vEndDir);

	dot = clamp(dot, -1.f, 1.f);

	// 계산을 하고자 하는 두 방향벡터 사이의 각도롤 t만큼 보간
	_float theta = acosf(dot) * t;

	Vector3 relativeVec = vEndDir - vStartDir * dot;
	relativeVec.Normalize();

	return ((vStartDir * cosf(theta)) + (relativeVec * sinf(theta)));
}

_float CMathUtils::Dot(const Vector3& vSrc, const Vector3& vDst)
{
	Vector3 SrcDir = vSrc;
	Vector3 DstDir = vDst;

	SrcDir.Normalize();
	DstDir.Normalize();
	
	return SrcDir.Dot(DstDir);
}

_float CMathUtils::RadFromDot(const Vector3& vSrc, const Vector3& vDst)
{
	return acosf(Dot(vSrc,vDst));
}

_float CMathUtils::GetDistance(const Vector3& vTargetPos, const Vector3& vMyPos)
{
	Vector3 vDir = vTargetPos - vMyPos;

	return vDir.Length();
}

Vector4 CMathUtils::HSVToRGB(const Vector4& vHSV)
{
	Vector4 vRGB(0.f, 0.f, 0.f, vHSV.w);

	if (vHSV.y == 0.f)
	{
		vRGB.x = vRGB.y = vRGB.z = vHSV.z;
		return vRGB;
	}

	_float h = fmodf(vHSV.x, 1.f) / (60.f / 360.f);
	_int i = (_int)h;
	_float f = h - (_float)i;
	_float p = vHSV.z * (1.f - vHSV.y);
	_float q = vHSV.z * (1.f - vHSV.y * f);
	_float t = vHSV.z * (1.f - vHSV.y * (1.f - f));

	switch (i)
	{
	case 0: vRGB.x = vHSV.z; vRGB.y = t; vRGB.z = p; break;
	case 1: vRGB.x = q; vRGB.y = vHSV.z; vRGB.z = p; break;
	case 2: vRGB.x = p; vRGB.y = vHSV.z; vRGB.z = t; break;
	case 3: vRGB.x = p; vRGB.y = q; vRGB.z = vHSV.z; break;
	case 4: vRGB.x = t; vRGB.y = p; vRGB.z = vHSV.z; break;
	case 5: default: vRGB.x = vHSV.z; vRGB.y = p; vRGB.z = q; break;
	}

	return vRGB;
}

_float CMathUtils::lerp(const _float& a, const _float& b, const _float& t)
{
	return a * (1.f - t) + b * t;
}
