#include "FontManager.h"
#include "CustomFont.h"

Engine::CFontManager::CFontManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice), m_pContext(pContext)
{
}

HRESULT Engine::CFontManager::Initialize()
{
    return S_OK;
}

HRESULT Engine::CFontManager::Add_Font(const wstring& strFontTag, const wstring& strFontFilePath)
{
	if (nullptr != Find_Font(strFontTag))
		return E_FAIL;

	shared_ptr<CCustomFont> pFont = CCustomFont::Create(m_pDevice, m_pContext, strFontFilePath);
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(strFontTag, pFont);

    return S_OK;
}

HRESULT Engine::CFontManager::Render(const wstring& strFontTag, const wstring& strText, const Vector2& vPosition, Vector4 vColor, _float fRotation, _float fScale)
{
	shared_ptr<CCustomFont> pFont = Find_Font(strFontTag);
	if (nullptr == pFont)
		return E_FAIL;

    return pFont->Render(strText, vPosition, vColor, fRotation, fScale);
}

shared_ptr<CCustomFont> Engine::CFontManager::Find_Font(const wstring& strFontTag)
{
	auto Pair = m_Fonts.find(strFontTag);

	if (Pair == m_Fonts.end())
		return nullptr;

    return Pair->second;
}

shared_ptr<CFontManager> Engine::CFontManager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CFontManager> pInstance = make_shared<CFontManager>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFontManager");
		pInstance.reset();
	}

	return pInstance;
}
