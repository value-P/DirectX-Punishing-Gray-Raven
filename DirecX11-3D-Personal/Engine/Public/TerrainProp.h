#pragma once
#include "VIBuffer.h"

namespace Engine
{
    class ENGINE_DLL CTerrainProp final : public CVIBuffer
    {
	public:
		CTerrainProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CTerrainProp() = default;

	public:
		virtual HRESULT Initialize(const wstring& strHeightMapPath);
		virtual HRESULT Initialize(_int iSizeX, _int iSizeZ);

	private:
		_uint m_iNumVerticesX = 0;
		_uint m_iNumVerticesZ = 0;

	public:
		static shared_ptr<CTerrainProp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strHeightMapPath);
		static shared_ptr<CTerrainProp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _int iSizeX, _int iSizeZ);

    };
}


