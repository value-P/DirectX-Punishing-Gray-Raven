#include "Texture.h"
#include "Shader.h"

Engine::CTexture::CTexture(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CProperty(pDevice, pContext)
{
}

HRESULT Engine::CTexture::Initialize(const wstring& strTextureFilePath, _uint iTextureNum)
{
	_tchar szEXT[MAX_PATH] = L"";

	_wsplitpath_s(strTextureFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

	m_vecSRV.reserve(iTextureNum);

	for (size_t i = 0; i < m_vecSRV.capacity(); ++i)
	{
		_tchar szFileTexturePath[MAX_PATH] = L"";

		wsprintf(szFileTexturePath, strTextureFilePath.c_str(), i);

		HRESULT hr = {};

		ComPtr<ID3D11ShaderResourceView> pSRV = nullptr;

		if (!lstrcmp(szEXT, L".dds")) // dds 颇老老 锭
			hr = CreateDDSTextureFromFile(m_pDevice.Get(), szFileTexturePath, nullptr, pSRV.GetAddressOf());
		else if (!lstrcmp(szEXT, L".tga")) // tga颇老老 锭
		{
			MSG_BOX(".tga Can not Create to Texture!!");
			hr = E_FAIL;
		}
		else
			hr = CreateWICTextureFromFile(m_pDevice.Get(), szFileTexturePath, nullptr, pSRV.GetAddressOf());

		if (FAILED(hr))
			return E_FAIL;

		m_vecSRV.push_back(pSRV);
	}

	return S_OK;
}

HRESULT Engine::CTexture::Add_Texture(const wstring& strTextureFilePath)
{
	_tchar szEXT[MAX_PATH] = L"";

	_wsplitpath_s(strTextureFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

	HRESULT hr = {};

	ComPtr<ID3D11ShaderResourceView> pSRV = nullptr;

	if (!lstrcmp(szEXT, L".dds")) // dds 颇老老 锭
		hr = CreateDDSTextureFromFile(m_pDevice.Get(), strTextureFilePath.c_str(), nullptr, pSRV.GetAddressOf());
	else if (!lstrcmp(szEXT, L".tga")) // tga颇老老 锭
	{
		MSG_BOX(".tga Can not Create to Texture!!");
		hr = E_FAIL;
	}
	else
		hr = CreateWICTextureFromFile(m_pDevice.Get(), strTextureFilePath.c_str(), nullptr, pSRV.GetAddressOf());

	if (FAILED(hr))
		return E_FAIL;

	m_vecSRV.push_back(pSRV);

	return S_OK;
}

ComPtr<ID3D11ShaderResourceView> Engine::CTexture::GetSRV(_uint iTextureIndex)
{
	if (iTextureIndex >= m_vecSRV.size()) return nullptr;

	return m_vecSRV[iTextureIndex];
}

const vector<ComPtr<ID3D11ShaderResourceView>>& Engine::CTexture::GetSRVs()
{
	return m_vecSRV;
}


shared_ptr<CTexture> Engine::CTexture::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureFilePath, _uint iTextureNum)
{
	shared_ptr<CTexture> pInstance = make_shared<CTexture>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strTextureFilePath, iTextureNum)))
	{
		MSG_BOX("Failed to Created : CTexture");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CTexture> Engine::CTexture::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	return make_shared<CTexture>(pDevice, pContext);
}
