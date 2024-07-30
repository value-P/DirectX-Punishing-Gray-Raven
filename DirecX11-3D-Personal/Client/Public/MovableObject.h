#pragma once
#include "GameObject.h"

namespace Client
{
    class CCameraReference;
    class CFX;
    class CParticleEffect;

    class CMovableObject abstract : public CGameObject
    {
    public:
        struct StateFunction
        {
            function<void(_float)> OnEnterState;
            function<void(_float)> OnState;
            function<void(_float)> OnExitState;
        };

        enum TimeState { OnEnter, Running, OnExit, TimeState_End };

    public:
        CMovableObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CMovableObject() = default;
    
    public:
        _float Get_HpRate() { return static_cast<_float>(m_iCurrentHp) / static_cast<_float>(m_iMaxHp); }
        Vector3 Get_CamRefPos();
        _bool isDead() { return m_bDead; }
        wstring Get_Name() { return m_strMovableObjName; }

    public:
        virtual HRESULT Initialize(const Vector3& vInitPos) override;
        virtual void Priority_Tick(_float fTimeDelta) override;
        virtual _int Tick(_float fTimeDelta) override;
        virtual void Late_Tick(_float fTimeDelta) override;
        virtual HRESULT Render() override;
        virtual HRESULT Render_Shadow() override;
        virtual HRESULT Render_ExceptDark() override;

        virtual void Hit(_uint iDamage);
        virtual void HardHit(_uint iDamage) {}

    protected:
        virtual HRESULT Set_EventFunctions();
        void Change_State(_int State);
        void Update_Dir_Pos_Data();
        
        // 애니메이션 이벤트
        void AnimEventTick(_float fTimeDelta);
        void ReadAnimEventDataFromJson(const Json::Value& root, const shared_ptr<CAnimModelProp> pAnimModelProp);

        // 이펙트 활성화 이벤트
        void EffectEventTick(_float fTimeDelta);
        void ReadEffectEventDataFromJson(const Json::Value& root, const shared_ptr<CAnimModelProp> pAnimModelProp);
        HRESULT Load_MeshEffects(LEVEL ePropLevel, LEVEL eCreateLevel, const wstring& strLayerTag, _float* pTimeScale, const wstring& fileRoot);
        HRESULT Load_ParticleEffects(LEVEL ePropLevel, LEVEL eCreateLevel, const wstring& strLayerTag, _float* pTimeScale, const wstring& fileRoot);

    protected:
        virtual void KeyInput() {}
        void Change_Anim(_uint animIndex);
        virtual HRESULT Add_Component(LEVEL ePropLevel, LEVEL eCreateLevel, const  wstring& strModelKey);
        virtual HRESULT Add_CamRef();

        // 사운드 재생 함수
        void PlaySoundOnce(const wstring& strSoundKey, _float fVolume);
        void resetSoundInfo();

    protected:
        // 애니메이션 이벤트 정보를 저장해놓는 컨테이너
        // 검색을 자주 할수 있기 때문에 umap으로 검색시간을 낮춘다
        using Events = vector<string>;
        using KeyFrameEvents = unordered_map<_int, Events>; // key: 애니메이션 키프레임 value:이벤트 문자열들
        unordered_map<_int, KeyFrameEvents>m_AnimEvents; // key : 애니메이션 인덱스
        // 이벤트 함수들과 이벤트 정보들
        unordered_map<string, function<void()>> EventFunctions;

        // 객체 이름
        wstring m_strMovableObjName = L"";

        // 이펙트 활성화 이벤트
        using EffectEvents = vector<FX_EVENT_DATA>;
        using EffectKeyFrameEvents = unordered_map<_int, EffectEvents>;
        unordered_map<_int, EffectKeyFrameEvents> m_EffectEvents; // 애니메이션별 -> 키프레임별 -> 이펙트 정보 배열
        unordered_map<string, shared_ptr<CFX>> EventEffects; // key : 이펙트 이름

        // 피격 이펙트
        shared_ptr<CParticleEffect> m_pHitParticle = nullptr;

        map<wstring,shared_ptr<CCollider>> m_MapColliderCom;
        shared_ptr<CSkeletalMeshCom> m_pMeshCom = nullptr;
        shared_ptr<CNavigation> m_pNavigationCom = nullptr;
        weak_ptr<CMovableObject> m_pTarget;
        shared_ptr<CCameraReference> m_pCamRef = nullptr;

        vector<StateFunction> m_StateFunctions;

        Vector3 m_vRootMotionVelocity = {};
        _double m_dCurrentAnimFrame = 0.0; // 현재 애니메이션이 몇퍼센트진행되었는지를 반환받는 변수
        _bool m_bLastAnimFrameCalled = false;

        TimeState m_eTimeState = OnEnter;
        _int    m_iPrevState = -1;
        _int    m_iCurrentState = 0;
        _int    m_iNextState = -1;

        _int    m_iHitStack = 0;
        _int    m_iCurrentHp = 0;
        _int    m_iMaxHp = 0;
        _int    m_iAttackPower = 0;

        Vector3 moveVelocity = {};

        _float   m_fSpeed = 4.f;
        _bool m_bDead = false;

        // 시간 체크용
        StopWatch m_StopWatchs[SW_END];

        // 타겟 위치관련 정보
        Vector3 m_vTargetPos = {};
        Vector3 m_vMyPos = {};
        Vector3 m_vMyLook = {};
        Vector3 m_vMyRight = {};
        Vector3 m_vTargetDir = {};
        _float m_fTargetDist = 0.f;

        // 사운드 재생 정보
        map<wstring, CHANNELID> m_mapSoundPlayInfo;
    };
}


