#pragma once
#include "GameObject.h"

namespace Client
{
    class CEventTrigger : public CGameObject
    {
    public:
        struct INIT_DESC
        {
            LEVEL eLevel;
            Vector3 vInitPos;
        };

    public:
        CEventTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CEventTrigger() = default;

    public:
        HRESULT Initialize(const INIT_DESC& initDesc);
        virtual _int Tick(_float fTimeDelta) override;
        virtual void Late_Tick(_float fTimeDelta) override;

    public:
        virtual void OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider);

    private:
        shared_ptr<CCollider> m_pTrigger = nullptr;
        LEVEL eCurrentLevel = LEVEL_END;

    public:
        static shared_ptr<CEventTrigger> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);
    };
}


