#pragma once
#include "MonsterBase.h"

namespace Client
{
    class CThrowObject;
    class CLaserObject;
    class CMeshEffect;
    class CParticleEffect;

    class CBoss_Pushin : public CMonsterBase
    {
    public:
        enum MonsterState
        {
            Idle,

            Walk, WalkL, WalkR,

            BeHitFly, hit, FallDown, OverHit,

            HitDown, LieOnFloor, StandUp,

            MeleeAttack, LongAttack,

            Born, Born_2,
            Death, EndCutScene,
            MonsterStateEnd
        };

    public:
        CBoss_Pushin(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CBoss_Pushin() = default;

    public:
        virtual HRESULT Initialize(const INIT_DESC& monsterDesc) override;
        virtual void Priority_Tick(_float fTimeDelta) override;
        virtual _int Tick(_float fTimeDelta) override;
        virtual void Late_Tick(_float fTimeDelta) override;
        virtual HRESULT Render() override;

        void PlayBornSound();

    private:
        HRESULT Add_StateFunction();
        HRESULT Add_Component(LEVEL eLevel, const wstring& strModelKey);
        HRESULT Bind_ShaderResources();
        virtual HRESULT Set_EventFunctions() override;
        HRESULT Init_Skills(const INIT_DESC& monsterDesc);

        void PlaySound();

        virtual void Move() override;
        virtual void Hit(_uint iDamage) override;

        virtual void OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider) override;
        virtual void OnCollisionStay(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider) override;

    private:
        shared_ptr<CThrowObject> m_pSkill1 = nullptr;
        shared_ptr<CLaserObject> m_pSkill2 = nullptr;
        shared_ptr<CMeshEffect> m_pGroundCrash1 = nullptr;
        shared_ptr<CMeshEffect> m_pGroundCrash2 = nullptr;

    private:
        _bool isSecondPhase = false;
        
        //------------------------- 1Æä--------------------------
        // ÁÖ¸ÔÀ¸·Î¹Ù´Ú³»·ÁÂï±â - ¿¤º¸¿ì - ÃÏ - ¹ß¹Ù´ÚÀ¸·Î¹Ù´ÚÂï±â - ·¹ÀÌÀú - ÆÝÄ¡ - ÇÒÄû±â
        _uint MeleePattern_FirstPhase[7] = {32, 17, 20, 30, 24, 16, 19};
        // ÅÂÅ¬¹ßÂ÷±â - ÃÏ - Á¡ÇÁ³»·ÁÂï±â - ·¹ÀÌÀú - Á¡ÇÁ³»·ÁÂï±â - ÇÒÄû±â
        _uint LongPattern_FirstPhase[6] = {29, 20, 8, 24, 8, 19};
        // 1Æä ÆÐÅÏ ÀÎµ¦½º
        _int Phase1MeleePatternIdx = 0;
        _int Phase1LongPatternIdx = 0;
        
        //------------------------- 2Æä--------------------------
        _uint MeleePattern_SecondPhase[7] = {33, 36, 44, 53, 42, 48, 45};
        _uint LongPattern_SecondPhase[6] = {31, 33, 43, 21, 40, 18};
        // 2Æä ÆÐÅÏ ÀÎµ¦½º
        _int Phase2MeleePatternIdx = 0;
        _int Phase2LongPatternIdx = 0;
        
        _float m_fJumpDist = 0.f;
    public:
        static shared_ptr<CBoss_Pushin> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& monsterDesc);
    };
}


