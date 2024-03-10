#pragma once
#include "Property.h"

namespace Engine
{
    class CShader;

    class ENGINE_DLL CTexture final : public CProperty
    {
	public:
		CTexture(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CTexture() = default;

	public:
		HRESULT Initialize(const wstring& strTextureFilePath, _uint iTextureNum);
		HRESULT Add_Texture(const wstring& strTextureFilePath);

	public:
		ComPtr<ID3D11ShaderResourceView> GetSRV(_uint iTextureIndex);
		const vector<ComPtr<ID3D11ShaderResourceView>>& GetSRVs();

	private:
		vector<ComPtr<ID3D11ShaderResourceView>> m_vecSRV;

	public:
		static shared_ptr<CTexture> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureFilePath, _uint iTextureNum = 1);
		static shared_ptr<CTexture> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    };
}


