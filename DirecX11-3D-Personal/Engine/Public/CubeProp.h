#pragma once
#include "VIBuffer.h"

namespace Engine
{
	class CCubeProp final : public CVIBuffer
	{
	public:
		CCubeProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CCubeProp() = default;

	public:
		virtual HRESULT Initialize() override;

	public:
		static shared_ptr<CCubeProp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	};
}


