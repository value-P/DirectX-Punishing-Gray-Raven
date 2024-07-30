#pragma once
#include "MovableObject.h"

namespace Client
{
    class CMonsterBase abstract : public CMovableObject
    {
    public:
        struct INIT_DESC
        {
            shared_ptr<CMovableObject> pTarget;
            Vector3 vInitPos;
            _float fSpeed;
            _int iHp;
            _int iAttackPower;
            _int iShield;
            LEVEL eLevel;
            wstring strModelKey;
        };

    public:
        CMonsterBase(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CMonsterBase() = default;

    public:
        virtual void Set_Activate(_bool bActivate) {};

    public:
        virtual HRESULT Initialize(const INIT_DESC& monsterDesc);
        virtual _int Tick(_float fTimeDelta) override;
        virtual void Late_Tick(_float fTimeDelta) override;

    protected:
        virtual void OnCollisionStay(shared_ptr<CCollider> pSrcCol, shared_ptr<CCollider> pCollider) override;

    protected:
        virtual void Move();

    protected:
        _int m_iCurrentShield = 0;
        _int m_iMaxShield = 0;
        _bool m_bCanAttack = false;
    };
}


