#include "CustomFont.h"

Engine::CCustomFont::CCustomFont(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice), m_pContext(pContext)
{
}

HRESULT Engine::CCustomFont::Initialize(const wstring& strFontFilePath)
{
	m_pBatch = make_shared<SpriteBatch>(m_pContext.Get());
	m_pFont = make_shared<SpriteFont>(m_pDevice.Get(), strFontFilePath.c_str());

	return S_OK;
}

HRESULT Engine::CCustomFont::Render(const wstring& strText, const Vector2& vPosition, Vector4 vColor, _float fRotation, _float fFontScale)
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pBatch->Begin();

	Vector4 stringSize = m_pFont->MeasureString(strText.c_str(), true) * fFontScale;
	Vector2 originPos = { stringSize.x * 0.5f, stringSize.y * 0.5f };
	m_pFont->DrawString(m_pBatch.get(), strText.c_str(), vPosition, vColor, fRotation, originPos, fFontScale);

	m_pBatch->End();

	return S_OK;
}

shared_ptr<CCustomFont> Engine::CCustomFont::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strFontFilePath)
{
	shared_ptr<CCustomFont> pInstance = make_shared<CCustomFont>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strFontFilePath)))
	{
		MSG_BOX("Failed to Created : CCustomFont");
		pInstance.reset();
	}

	return pInstance;
}
