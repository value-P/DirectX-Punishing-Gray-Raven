#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CCustomFont;

	class CFontManager final
	{
	public:
		CFontManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CFontManager() = default;

	public:
		HRESULT Initialize();
		HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
		HRESULT Render(const wstring& strFontTag, const wstring& strText, const Vector2& vPosition, Vector4 vColor = Vector4(1.f,1.f,1.f,1.f), _float fRotation = 0.f, _float fScale = 1.f);

	private:
		shared_ptr<CCustomFont> Find_Font(const wstring& strFontTag);

	private:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

		map<wstring, shared_ptr<CCustomFont>> m_Fonts;

	public:
		static shared_ptr<CFontManager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	};
}


