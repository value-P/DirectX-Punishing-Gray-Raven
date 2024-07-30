#pragma once
#include "VIBuffer.h"

namespace Engine
{
	class CRectProp final: public CVIBuffer
	{
	public:
		CRectProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CRectProp() = default;

	public:
		virtual HRESULT Initialize() override;

	public:
		static shared_ptr<CRectProp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	};
}
