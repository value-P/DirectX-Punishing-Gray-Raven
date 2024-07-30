#pragma once
#include "MonsterBase.h"

namespace Engine
{
    class CBone;
}

namespace Client
{
    class CThrowObject;
    class CLaserObject;
    class CTrailFX;
    class CDistortionPointRect;

    class CBoss_StarKnight : public CMonsterBase
    {
    public:
        enum MonsterState
        {
            Idle,

            BackDash, RightDash, LeftDash, FrontDash,

            ParrySuccessPattern, // 패링 성공시 무력화

            Attack,
            PreParringAttack, MainParringAttack,

            ParryFailedPattern, // 패링 실패시 공중으로 날아가서 패턴시전

            Born_1,Born_2,
            Death,
            MonsterStateEnd
        };

    public:
        CBoss_StarKnight(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CBoss_StarKnight() = default;

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
        virtual HRESULT Set_EventFunctions() override;
        HRESULT Init_Skills(const INIT_DESC& monsterDesc);
        HRESULT Add_Child(LEVEL eLevel);

        void PlaySound();

        virtual void Move() override;
        virtual void Hit(_uint iDamage) override;

        virtual void OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider) override;
        virtual void OnCollisionStay(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider) override;

    private:        
        _uint Pattern[11] = {17,19,20,29,12,18,31,19,25,32,34 };
        //_uint Pattern[1] = { 18 };
        _uint ParringFailedPattern[4] = {22, 22, 27, 25};
        //_uint ParringFailedPattern[2] = {27, 24};
        // 패턴 인덱스
        _int PatternIdx = 0;
        _int ParringFailedPatternIdx = 0;

        _int m_iPatternCount = 0;

        _float m_fJumpDist = 0.f;

        _bool m_bStopAnim = false;
        _bool m_bGroggyCalled = false;

    private:
        shared_ptr<CThrowObject> m_pSkill1 = nullptr;
        shared_ptr<CThrowObject> m_pSkill2 = nullptr;
        shared_ptr<CThrowObject> m_pSkill3 = nullptr;
        shared_ptr<CLaserObject> m_pSkill4 = nullptr;

        // 이펙트
        shared_ptr<CDistortionPointRect> m_pParryDistortion = nullptr;
        shared_ptr<CTrailFX> m_pRightHandTrail = nullptr;
        shared_ptr<CTrailFX> m_pLeftHandTrail = nullptr;
        weak_ptr<CBone> m_RightHandBone;
        weak_ptr<CBone> m_pLeftHandBone;
        Matrix matLeftHand = {};
        Matrix matRightHand = {};

        // 타겟과 나사이의 중간 포지션
        Vector3 m_vCenterWithTarget = {};

    public:
        static shared_ptr<CBoss_StarKnight> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& monsterDesc);
    };
}


