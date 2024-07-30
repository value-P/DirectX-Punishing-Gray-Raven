#pragma once
#include "VIBuffer.h"

namespace Engine
{
    class CCellBufferProp : public CVIBuffer
    {
	public:
		CCellBufferProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CCellBufferProp() = default;

	public:
		virtual HRESULT Initialize(const Vector3& vPoint0, const Vector3& vPoint1, const Vector3& vPoint2);

	public:
		static shared_ptr<CCellBufferProp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const Vector3& vPoint0, const Vector3& vPoint1, const Vector3& vPoint2);
    };
}


