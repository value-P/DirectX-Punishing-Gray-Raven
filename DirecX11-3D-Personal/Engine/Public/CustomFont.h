#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CCustomFont final
	{
	public:
		CCustomFont(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CCustomFont() = default;

	public:
		HRESULT Initialize(const wstring& strFontFilePath);
		HRESULT Render(const wstring& strText, const Vector2& vPosition, Vector4 vColor, _float fRotation, _float fFontScale);

	private:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

		shared_ptr<SpriteBatch> m_pBatch = nullptr;
		shared_ptr<SpriteFont> m_pFont = nullptr;

	public:
		static shared_ptr<CCustomFont> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strFontFilePath);
	};
}


