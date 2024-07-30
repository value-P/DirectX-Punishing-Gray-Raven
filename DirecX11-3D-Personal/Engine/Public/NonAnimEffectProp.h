#pragma once
#include "EffectProp.h"

namespace Engine
{
    class CNonAnimModelProp;

    class ENGINE_DLL CNonAnimEffectProp final : public CEffectProp
    {
    public:
        CNonAnimEffectProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CNonAnimEffectProp() = default;

    public:
        virtual HRESULT Add_Buffer(const wstring& strBufferPath, const Matrix& pivotMatrix) override;
        shared_ptr<CNonAnimModelProp> Get_Buffer() { return m_pModelProp; }

    public:
        virtual HRESULT Initialize() override;

    private:
        shared_ptr<CNonAnimModelProp> m_pModelProp = nullptr;

    public:
        static shared_ptr<CNonAnimEffectProp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

    };
}

