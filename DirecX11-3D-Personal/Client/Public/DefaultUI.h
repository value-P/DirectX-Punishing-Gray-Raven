#pragma once
#include "UIBase.h"

namespace Client
{
    class CDefaultUI final : public CUIBase
    {
    public:
        CDefaultUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CDefaultUI() = default;

    public:
        virtual HRESULT Initialize(const INIT_DESC& initDesc);
        virtual _int Tick(_float fTimeDelta) override;
        virtual HRESULT Render() override;

    private:
        HRESULT Add_Component(const INIT_DESC& initDesc);

    public:
        static shared_ptr<CDefaultUI> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);
    };
}