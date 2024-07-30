#include "EffectProp.h"
#include "Texture.h"
#include "StringUtils.h"

Engine::CEffectProp::CEffectProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CProperty(pDevice, pContext)
{
}

shared_ptr<CTexture> Engine::CEffectProp::Get_Texture(const string& strTextureKey)
{
	auto Pair = m_EffectTextures.find(strTextureKey);

	if (Pair == m_EffectTextures.end())
		return nullptr;

	return Pair->second;
}

HRESULT Engine::CEffectProp::Add_Texture(const string& strTexFullPath)
{
	filesystem::path texturePath(strTexFullPath);

	shared_ptr<CTexture> pTexture = CTexture::Create(m_pDevice, m_pContext, CStringUtils::toWstring(strTexFullPath));
	if (nullptr == pTexture)
		return E_FAIL;

	string texKey = texturePath.stem().string();
	m_TextureKeys.push_back(texKey);
	m_EffectTextures.emplace(texKey, pTexture);

	return S_OK;
}

HRESULT Engine::CEffectProp::Initialize()
{
	return S_OK;
}