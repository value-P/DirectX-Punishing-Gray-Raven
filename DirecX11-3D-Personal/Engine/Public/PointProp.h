#pragma once
#include "VIBuffer.h"

namespace Engine
{
    class CPointProp final: public CVIBuffer
    {
	public:
		CPointProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CPointProp() = default;

	public:
		virtual HRESULT Initialize() override;

	public:
		static shared_ptr<CPointProp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

    };
}


