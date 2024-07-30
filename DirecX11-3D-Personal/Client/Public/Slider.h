#pragma once
#include "UIBase.h"

namespace Client
{
    class CSlider final : public CUIBase
    {
    public:
        struct INIT_DESC : CUIBase::INIT_DESC
        {
            Vector4 vColor;
        };

    public:
        CSlider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CSlider() = default;

    public:
        void SetSlideRate(_float rate) { m_fSlideRate = rate; }

    public:
        virtual HRESULT Initialize(const INIT_DESC& initDesc);
        virtual _int Tick(_float fTimeDelta) override;
        virtual HRESULT Render() override;

    private:
        HRESULT Add_Component(const INIT_DESC& initDesc);

    private:
        _float m_fSlideRate = 1.f;
        Vector4 m_vColor = {};
    public:
        static shared_ptr<CSlider> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);

    };
}


