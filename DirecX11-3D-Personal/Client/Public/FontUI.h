#pragma once
#include "UIBase.h"

namespace Client
{
    class CFontUI final : public CUIBase
    {
    public:
        struct INIT_DESC
        {
            wstring strFontTag;
            Vector2 vPosition;
            Vector4 vColor;
            _float fScale;
        };

    public:
        CFontUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CFontUI() = default;

    public:
        void Set_Text(const wstring& strText) { m_strText = strText; }

    public:
        virtual HRESULT Initialize(const INIT_DESC& initDesc);        
        virtual _int Tick(_float fTimeDelta) override;
        virtual HRESULT Render() override;

    private:
        wstring m_strFontTag = L"";
        wstring m_strText = L"";
        Vector2 m_vPosition = {};
        Vector4 m_vColor = {};
        _float m_fScale = 1.f;

    public:
        static shared_ptr<CFontUI> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);
    };
}


