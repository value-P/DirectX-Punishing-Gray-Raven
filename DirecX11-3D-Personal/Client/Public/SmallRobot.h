#pragma once
#include "MonsterBase.h"

namespace Client
{
    class CSmallRobot final: public CMonsterBase
    {
    public:
        enum MonsterState
        {
            Idle,

            Run, RunB, RunL, RunR, Walk,

            BeHitFly, hit, HitDown,FallDown, OverHit, LieOnFloor, StandUp,

            Attack1, Attack2, Attack3, Attack4,
            Death,
            MonsterStateEnd
        };

    public:
        CSmallRobot(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CSmallRobot() = default;

    public:
        virtual void Set_Activate(_bool bActivate) override;

    public:
        virtual HRESULT Initialize(const INIT_DESC& monsterDesc) override;
        virtual void Priority_Tick(_float fTimeDelta) override;
        virtual _int Tick(_float fTimeDelta) override;
        virtual void Late_Tick(_float fTimeDelta) override;
        virtual HRESULT Render() override;

    private:
        HRESULT Add_StateFunction();
        HRESULT Add_Component(LEVEL eLevel, const wstring& strModelKey);
        HRESULT Bind_ShaderResources();
        virtual void Move() override;
        virtual void Hit(_uint iDamage) override;
        virtual void HardHit(_uint iDamage) override;

        virtual void OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider) override;
        virtual void OnCollisionStay(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider) override;

    private:
        _uint AttackPattern[4] = { Attack1 , Attack3 , Attack2 , Attack4 };
        _int m_iAttackPatternPhase = 0;
        _float m_fJumpDist = 0.f;

    public:
        static shared_ptr<CSmallRobot> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& monsterDesc);
    };
}


