#include "pch.h"
#include "Boss_Pushin.h"
#include "ThrowObject.h"
#include "LaserObject.h"
#include "MeshEffect.h"
#include "ParticleEffect.h"
#include "PlayableVera.h"

Client::CBoss_Pushin::CBoss_Pushin(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CMonsterBase(pDevice, pContext)
{
    m_StateFunctions.resize(MonsterStateEnd);
    m_strMovableObjName = L"푸신";
}

HRESULT Client::CBoss_Pushin::Initialize(const INIT_DESC& monsterDesc)
{
    if (FAILED(__super::Initialize(monsterDesc)))
        return E_FAIL;

    if (FAILED(Add_StateFunction()))
        return E_FAIL;

    if (FAILED(Add_Component(monsterDesc.eLevel, monsterDesc.strModelKey))) 
        return E_FAIL;

    if (FAILED(Load_MeshEffects(monsterDesc.eLevel, monsterDesc.eLevel, L"MonsterEffect", &g_TimeScale_Monster, L"../Bin/EffectPrefab/MeshEffect/Pushin")))
        return E_FAIL;

    if (FAILED(Load_ParticleEffects(monsterDesc.eLevel, monsterDesc.eLevel, L"MonsterEffect", &g_TimeScale_Monster, L"../Bin/EffectPrefab/ModelParticleList/Mb1BruceloongMd010001.json")))
        return E_FAIL;

    if (FAILED(Init_Skills(monsterDesc)))
        return E_FAIL;

    Change_State(Born);
    Change_Anim(81);

    return S_OK;
}

void Client::CBoss_Pushin::Priority_Tick(_float fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);
}

_int Client::CBoss_Pushin::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    if (m_pGameInstance.lock()->GetKey(DIK_P))
        m_iCurrentHp = 0;
#endif 

    m_pMeshCom->Play_Animation(fTimeDelta, m_vRootMotionVelocity, m_bLastAnimFrameCalled, m_dCurrentAnimFrame, true, true, true);

    __super::Tick(fTimeDelta);

    if (isSecondPhase == false && m_iCurrentHp <= 0 && m_iCurrentState != HitDown)
        Change_State(HitDown);
    else if (isSecondPhase == true && m_iCurrentHp <= 0 && m_iCurrentState < Death)
        Change_State(Death);
    
    Move();

    if (m_iCurrentState == Born)
    {
        m_pTransformCom->Go_Right(m_vRootMotionVelocity.x, m_pNavigationCom);
        m_pTransformCom->Go_Straight(-m_vRootMotionVelocity.z, m_pNavigationCom);
    }

    _float y = m_pNavigationCom->Get_HeightOnNav(m_pTransformCom->Get_Pos());
     m_pTransformCom->Set_Y(y);

     PlaySound();

    return OBJ_ALIVE;
}

void Client::CBoss_Pushin::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_pTransformCom->Go_Straight(moveVelocity.Length() * fTimeDelta);

    if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;

}

HRESULT Client::CBoss_Pushin::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    for (_uint i = 0; i < m_pMeshCom->Get_MeshCount(); ++i)
    {
        // 텍스처 바인딩
        if (FAILED(m_pMeshCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
            return E_FAIL;
        if (FAILED(m_pMeshCom->Bind_BoneMatrices("g_BoneMatrices", i)))
            return E_FAIL;

        m_pMeshCom->Render(i, 0);
    }

    return S_OK;
}

void Client::CBoss_Pushin::PlayBornSound()
{
    PlaySoundOnce(L"PushinTalk1", 1.f);
}

HRESULT Client::CBoss_Pushin::Add_StateFunction()
{
#pragma region 스탠딩
    m_StateFunctions[MonsterState::Idle].OnEnterState = [&](_float fTimeDelta) { 
        if (isSecondPhase)
            Change_State(77);
        else
            Change_Anim(76);
        };
#pragma endregion
    
#pragma region 컷신
    m_StateFunctions[MonsterState::Born].OnEnterState = [&](_float fTimeDelta) { Change_Anim(81); };
    m_StateFunctions[MonsterState::Born].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Born_2); } };
    m_StateFunctions[MonsterState::Born_2].OnEnterState = [&](_float fTimeDelta) { Change_Anim(26); };
    m_StateFunctions[MonsterState::Born_2].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) { Change_State(Idle); } };
#pragma endregion

#pragma region 공격
    m_StateFunctions[MonsterState::MeleeAttack].OnEnterState = [&](_float fTimeDelta) { 
        if (isSecondPhase)
            Change_Anim(MeleePattern_SecondPhase[Phase2MeleePatternIdx]);
        else
            Change_Anim(MeleePattern_FirstPhase[Phase1MeleePatternIdx]);
        };
    m_StateFunctions[MonsterState::MeleeAttack].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) Change_State(Idle); };
    m_StateFunctions[MonsterState::MeleeAttack].OnExitState = [&](_float fTimeDelta) {
        if (isSecondPhase)
        {
            Phase2MeleePatternIdx++;
            _int iSize = sizeof(MeleePattern_SecondPhase) / sizeof(MeleePattern_SecondPhase[0]);
            if (Phase2MeleePatternIdx >= iSize)
                Phase2MeleePatternIdx = 0;
        }
        else
        {
            Phase1MeleePatternIdx++;
            _int iSize = sizeof(MeleePattern_FirstPhase) / sizeof(MeleePattern_FirstPhase[0]);
            if (Phase1MeleePatternIdx >= iSize)
                Phase1MeleePatternIdx = 0;
        }

        resetSoundInfo();
        };

    m_StateFunctions[MonsterState::LongAttack].OnEnterState = [&](_float fTimeDelta) {
        m_fJumpDist = m_fTargetDist;

        if (isSecondPhase)
            Change_Anim(LongPattern_SecondPhase[Phase2LongPatternIdx]);
        else
            Change_Anim(LongPattern_FirstPhase[Phase1LongPatternIdx]);
        };
    m_StateFunctions[MonsterState::LongAttack].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) Change_State(Idle); };
    m_StateFunctions[MonsterState::LongAttack].OnExitState = [&](_float fTimeDelta) {
        if (isSecondPhase)
        {
            Phase2LongPatternIdx++;
            _int iSize = sizeof(LongPattern_SecondPhase) / sizeof(LongPattern_SecondPhase[0]);
            if (Phase2LongPatternIdx >= iSize)
                Phase2LongPatternIdx = 0;
        }
        else
        {
            Phase1LongPatternIdx++;
            _int iSize = sizeof(LongPattern_FirstPhase) / sizeof(LongPattern_FirstPhase[0]);
            if (Phase1LongPatternIdx >= iSize)
                Phase1LongPatternIdx = 0;
        }

        resetSoundInfo();
        };
#pragma endregion

#pragma region 피격
    //m_StateFunctions[MonsterState::hit].OnEnterState = [&](_float fTimeDelta) { Change_Anim(); };
    //m_StateFunctions[MonsterState::hit].OnState = [&](_float fTimeDelta) {};
    
    //m_StateFunctions[MonsterState::BeHitFly].OnEnterState = [&](_float fTimeDelta) { Change_Anim(); };
    //m_StateFunctions[MonsterState::BeHitFly].OnState = [&](_float fTimeDelta) {};
    
    //m_StateFunctions[MonsterState::FallDown].OnEnterState = [&](_float fTimeDelta) { Change_Anim(); };
    //m_StateFunctions[MonsterState::FallDown].OnState = [&](_float fTimeDelta) {};
    
    //m_StateFunctions[MonsterState::OverHit].OnEnterState = [&](_float fTimeDelta) { Change_Anim(); };
    //m_StateFunctions[MonsterState::OverHit].OnState = [&](_float fTimeDelta) {};
    
    //m_StateFunctions[MonsterState::LieOnFloor].OnEnterState = [&](_float fTimeDelta) { Change_Anim(); };
    //m_StateFunctions[MonsterState::LieOnFloor].OnState = [&](_float fTimeDelta) {};
    #pragma endregion
    
#pragma region 연출
    // 1페 사망
    m_StateFunctions[MonsterState::HitDown].OnEnterState = [&](_float fTimeDelta) { Change_Anim(71); };
    m_StateFunctions[MonsterState::HitDown].OnState = [&](_float fTimeDelta) {
        if (m_bLastAnimFrameCalled)
        {
            isSecondPhase = true;
            m_iCurrentHp = m_iMaxHp;
            m_iCurrentShield = m_iMaxShield;
            Change_State(StandUp);
            m_MapColliderCom[L"Col_Attack1"]->Set_Activate(false);
            m_MapColliderCom[L"Col_Attack2"]->Set_Activate(false);
        }
        };

    // 2페 돌입
    m_StateFunctions[MonsterState::StandUp].OnEnterState = [&](_float fTimeDelta) { Change_Anim(79); };
    m_StateFunctions[MonsterState::StandUp].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) Change_State(Idle); };

    // 2페 사망
    m_StateFunctions[MonsterState::Death].OnEnterState = [&](_float fTimeDelta) { Change_Anim(70); };
    m_StateFunctions[MonsterState::Death].OnState = [&](_float fTimeDelta) { 
        if (m_bLastAnimFrameCalled)
        {
            static_pointer_cast<CPlayableVera>(m_pTarget.lock())->Kill_Render();
            m_MapColliderCom[L"Col_Attack1"]->Set_Activate(false);
            m_MapColliderCom[L"Col_Attack2"]->Set_Activate(false);
            Change_State(EndCutScene);
            m_pGameInstance.lock()->Set_MainCamera(LEVEL_BOSS, L"Boss_Die");
        }};

    // 마무리 컷신
    m_StateFunctions[MonsterState::EndCutScene].OnEnterState = [&](_float fTimeDelta) { Change_Anim(55); };

    //m_StateFunctions[MonsterState::EndCutScene].OnEnterState = [&](_float fTimeDelta) { Change_Anim(); };
    //m_StateFunctions[MonsterState::EndCutScene].OnState = [&](_float fTimeDelta) {};
#pragma endregion
    return S_OK;
}

HRESULT Client::CBoss_Pushin::Add_Component(LEVEL eLevel, const wstring& strModelKey)
{
    if (FAILED(__super::Add_Component(eLevel, eLevel, strModelKey)))
        return E_FAIL;

    CBounding_Sphere::INIT_DESC sphereColDesc;
    CCollider::INIT_DESC colliderDesc;

    // 충돌 콜라이더
    sphereColDesc.vCenter = Vector3(0.f, 1.f, 0.f);
    sphereColDesc.fRadius = 1.5f;

    colliderDesc.eType = CCollider::TYPE_SPHERE;
    colliderDesc.iLayerType = Col_Monster;
    colliderDesc.pInitDesc = make_shared<CBounding_Sphere::INIT_DESC>(sphereColDesc);
    colliderDesc.pOwner = shared_from_this();
    colliderDesc.iCreateLevelIndex = eLevel;
    m_MapColliderCom.emplace(L"Col_Body", CCollider::Create(m_pDevice, m_pContext, colliderDesc));

    // 공격 콜라이더1
    sphereColDesc.vCenter = Vector3(0.f, 1.f, 2.5f);
    sphereColDesc.fRadius = 2.5f;

    colliderDesc.eType = CCollider::TYPE_SPHERE;
    colliderDesc.iLayerType = Col_MonsterAttack;
    colliderDesc.pInitDesc = make_shared<CBounding_Sphere::INIT_DESC>(sphereColDesc);
    colliderDesc.pOwner = shared_from_this();
    colliderDesc.iCreateLevelIndex = eLevel;
    shared_ptr<CCollider> pAttackCol = CCollider::Create(m_pDevice, m_pContext, colliderDesc);
    pAttackCol->Set_Activate(false);
    m_MapColliderCom.emplace(L"Col_Attack1", pAttackCol);

    // 공격 콜라이더2
    sphereColDesc.vCenter = Vector3(0.f, 1.f, 0.f);
    sphereColDesc.fRadius = 4.5f;

    colliderDesc.eType = CCollider::TYPE_SPHERE;
    colliderDesc.iLayerType = Col_MonsterAttack;
    colliderDesc.pInitDesc = make_shared<CBounding_Sphere::INIT_DESC>(sphereColDesc);
    colliderDesc.pOwner = shared_from_this();
    colliderDesc.iCreateLevelIndex = eLevel;
    pAttackCol = CCollider::Create(m_pDevice, m_pContext, colliderDesc);
    pAttackCol->Set_Activate(false);
    m_MapColliderCom.emplace(L"Col_Attack2", pAttackCol);

    return S_OK;
}

HRESULT Client::CBoss_Pushin::Bind_ShaderResources()
{
    Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
    Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

    if (FAILED(m_pMeshCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pMeshCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pMeshCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    return S_OK;
}

HRESULT Client::CBoss_Pushin::Set_EventFunctions()
{
    if (FAILED(__super::Set_EventFunctions()))
        return E_FAIL;

    //"EnableBodyCollider"
    //"DisAbleBodyCollider"
    //"EnAbleAttackCollider1"
    //"DisAbleAttackCollider1"
    //"EnAbleAttackCollider2"
    //"DisAbleAttackCollider2"
    //"EnAbleAttackCollider3"
    //"DisAbleAttackCollider3"
    //"ActiveParryFlag"
    //"InActiveParryFlag"
    //"UseSkill1" : 검기촙
    //"UseSkill2" : 브레스
    //"UseSkill3"
    //"UseSkill4"

    EventFunctions["EnAbleAttackCollider1"] = [&]() {m_MapColliderCom[L"Col_Attack1"]->Set_Activate(true); };
    EventFunctions["DisAbleAttackCollider1"] = [&]() {m_MapColliderCom[L"Col_Attack1"]->Set_Activate(false); };
    EventFunctions["EnAbleAttackCollider2"] = [&]() {m_MapColliderCom[L"Col_Attack2"]->Set_Activate(true); };
    EventFunctions["DisAbleAttackCollider2"] = [&]() {m_MapColliderCom[L"Col_Attack2"]->Set_Activate(false); };

    EventFunctions["UseSkill1"] = [&]() {
        Vector3 vPos = m_vMyPos + m_vMyLook;
        vPos.y += 1.f;
        Vector3 vDestPos = m_vTargetPos;
        vDestPos.y = vPos.y;
        m_pSkill1->Throw(vPos, vDestPos);
        };
    EventFunctions["UseSkill2"] = [&]() {
        m_pSkill2->Activate();
        };
    EventFunctions["GroundEffectPlay"] = [&]() {
        m_pGroundCrash1->Get_Transform()->Set_Pos(m_vMyPos);
        m_pGroundCrash1->PlayOnce();
        m_pGroundCrash2->Get_Transform()->Set_Pos(m_vMyPos);
        m_pGroundCrash2->PlayOnce();
        };

    return S_OK;
}

HRESULT Client::CBoss_Pushin::Init_Skills(const INIT_DESC& monsterDesc)
{
    CThrowObject::INIT_DESC desc;
    desc.fSpeed = 12.f;
    desc.vInitPos = m_vMyPos;
    desc.iDamage = m_iAttackPower;
    desc.eCreateLevel = monsterDesc.eLevel;
    desc.fDuration = 10.f;
    desc.fColRadius = 2.f;
    desc.strSkillPrefabPath_1 = L"../Bin/EffectPrefab/MeshEffect/Pushin/Longjuan502.MeshFX";
    m_pSkill1 = CThrowObject::Create(m_pDevice, m_pContext, desc);
    if (nullptr == m_pSkill1)
        return E_FAIL;

    if (FAILED(m_pGameInstance.lock()->Add_Object(monsterDesc.eLevel, L"Layer_MonsterSkill", m_pSkill1, &g_TimeScale_Monster)))
        return E_FAIL;

    // ------------------------------------------

    CLaserObject::INIT_DESC skill2desc;
    skill2desc.fDuration = 2.f;
    skill2desc.eCreateLevel = monsterDesc.eLevel;
    skill2desc.vInitPos = m_vMyPos;
    skill2desc.iDamage = m_iAttackPower;
    skill2desc.vColExtents = Vector3(1.f, 1.f, 20.f);
    skill2desc.vColOffset = Vector3(0.f, 1.3f, 20.f);

    m_pSkill2 = CLaserObject::Create(m_pDevice, m_pContext, skill2desc);
    if (nullptr == m_pSkill2)
        return E_FAIL;

    __super::Add_Children(L"Skill2", m_pSkill2);

    // 이펙트
    CMeshEffect::INIT_DESC effectDesc;
    effectDesc.eLevel = monsterDesc.eLevel;
    effectDesc.FXPrefabPath = L"../Bin/EffectPrefab/MeshEffect/Pushin/FxMb1BruceloongDlie101.MeshFX";
    m_pGroundCrash1 = CMeshEffect::Create(m_pDevice, m_pContext, effectDesc);
    effectDesc.FXPrefabPath = L"../Bin/EffectPrefab/MeshEffect/Pushin/FxMb1BruceloongDlie103.MeshFX";
    m_pGroundCrash2 = CMeshEffect::Create(m_pDevice, m_pContext, effectDesc);

    if (FAILED(m_pGameInstance.lock()->Add_Object(monsterDesc.eLevel, L"Layer_MonsterSkill", m_pGroundCrash1, &g_TimeScale_Monster)))
    {
        MSG_BOX("CBoss_Pushin::Init_Skills");
        return E_FAIL;
    }
    if (FAILED(m_pGameInstance.lock()->Add_Object(monsterDesc.eLevel, L"Layer_MonsterSkill", m_pGroundCrash2, &g_TimeScale_Monster)))
    {
        MSG_BOX("CBoss_Pushin::Init_Skills");
        return E_FAIL;
    }

    // 피격 이펙트 생성
    CParticleEffect::INIT_DESC particleDesc;
    particleDesc.eLevel = monsterDesc.eLevel;
    particleDesc.FXPrefabPath = L"../Bin/EffectPrefab/ParticleEffect/VeraHit.Particle";
    m_pHitParticle = CParticleEffect::Create(m_pDevice, m_pContext, particleDesc);
    if (FAILED(m_pGameInstance.lock()->Add_Object(monsterDesc.eLevel, L"Layer_MonsterEffect", m_pHitParticle, &g_TimeScale_Player)))
    {
        MSG_BOX("CBoss_Pushin::Init_Skills");
        return E_FAIL;
    }


    return S_OK;
}

void Client::CBoss_Pushin::PlaySound()
{
    _int iCurrentKeyFrame = m_pMeshCom->Get_CurrentKeyFrame();

    switch (m_iCurrentState)
    {
    case MeleeAttack:
        if (false == isSecondPhase) // 1페
        {
            switch (Phase1MeleePatternIdx)
            {
            case 0: // Attack21_1
            {
                if (iCurrentKeyFrame == 31)
                    PlaySoundOnce(L"mb_bruceloong01_atk21xuli", 0.5f);
                else if (iCurrentKeyFrame == 47)
                    PlaySoundOnce(L"mb_bruceloong01_atk21", 0.5f);
            }
                break;
            case 1: // Attack11
            {
                if (iCurrentKeyFrame == 28)
                    PlaySoundOnce(L"mb_bruceloong01_atk11xuli", 0.5f);
                else if (iCurrentKeyFrame == 51)
                    PlaySoundOnce(L"mb_bruceloong01_atk11", 0.5f);
            }
                break;
            case 2: // Attack13
            {
                if (iCurrentKeyFrame == 47)
                    PlaySoundOnce(L"mb_bruceloong02_atk1202", 0.5f);
            }
                break;
            case 3: // Attack20
            {
                if (iCurrentKeyFrame == 23)
                    PlaySoundOnce(L"mb_bruceloong01_atk20bip001lfoot", 0.5f);
                else if (iCurrentKeyFrame == 52)
                    PlaySoundOnce(L"mb_bruceloong01_atk20", 0.5f);
            }
                break;
            case 4: // Attack15
            {
                if (iCurrentKeyFrame == 13)
                    PlaySoundOnce(L"mb_bruceloong01_atk15juqi", 0.5f);
                else if (iCurrentKeyFrame == 37)
                    PlaySoundOnce(L"mb_bruceloong01_atk15", 0.5f);
            }
                break;
            case 5: // Attack10_1
            {
                if (iCurrentKeyFrame == 12)
                    PlaySoundOnce(L"mb_bruceloong01_atk10tishi", 0.5f);
                else if (iCurrentKeyFrame == 38)
                    PlaySoundOnce(L"mb_bruceloong01_atk10n", 0.5f);
            }
                break;
            case 6: // Attack12_1
            {
                if (iCurrentKeyFrame == 31)
                    PlaySoundOnce(L"mb_bruceloong02_atk1202", 0.5f);
            }
                break;
            }
        }
        else // 2페
        {
            switch (Phase2MeleePatternIdx)
            {
            case 0: // Attack22
            {
                if (iCurrentKeyFrame == 50)
                    PlaySoundOnce(L"mb_bruceloong02_atk07", 0.5f);
            }
            break;
            case 1: // Attack23_1
            {
                if (iCurrentKeyFrame == 26)
                    PlaySoundOnce(L"mb_bruceloong02_atk23", 0.5f);
            }
                break;
            case 2: // Attack33
            {
                if (iCurrentKeyFrame == 14)
                    PlaySoundOnce(L"mb_bruceloong02_atk3301", 0.5f);
                if (iCurrentKeyFrame == 29)
                    PlaySoundOnce(L"mb_bruceloong02_atk3302", 0.5f);
                if (iCurrentKeyFrame == 61)
                    PlaySoundOnce(L"mb_bruceloong02_atk33", 0.5f);
            }
            break;
            case 3: // Attack43
            {
                if (iCurrentKeyFrame == 20)
                    PlaySoundOnce(L"mb_bruceloong02_atk4202", 0.5f);
            }
            break;
            case 4: // Attack31
            {
                if (iCurrentKeyFrame == 29)
                    PlaySoundOnce(L"mb_bruceloong02_atk31", 0.5f);
            }
            break;
            case 5: // Attack37
            {
                if (iCurrentKeyFrame == 21)
                    PlaySoundOnce(L"mb_bruceloong02_atk3701", 0.5f);
                else if (iCurrentKeyFrame == 32)
                    PlaySoundOnce(L"mb_bruceloong02_atk3702", 0.5f);
                else if (iCurrentKeyFrame == 48)
                    PlaySoundOnce(L"mb_bruceloong02_atk37", 0.5f);
            }
            break;
            case 6: // Attack34
            {
                if (iCurrentKeyFrame == 52)
                    PlaySoundOnce(L"mb_bruceloong02_atk34", 0.5f);
            }
            break;
            }
        }        
        break;
    case LongAttack:
        if (false == isSecondPhase) // 1페
        {
            switch (Phase1LongPatternIdx)
            {
            case 0: // Attack19
            {
                if (iCurrentKeyFrame == 13)
                    PlaySoundOnce(L"mb_bruceloong01_atk19ti", 0.5f);
                else if (iCurrentKeyFrame == 31)
                    PlaySoundOnce(L"mb_bruceloong01_atk19", 0.5f);
            }
            break;
            case 1: // Attack13
            {
                if (iCurrentKeyFrame == 47)
                    PlaySoundOnce(L"mb_bruceloong02_atk1202", 0.5f);
            }
            break;
            case 2: // Attack05
            {
                if (iCurrentKeyFrame == 13)
                    PlaySoundOnce(L"mb_bruceloong01_atk05", 0.5f);
                else if (iCurrentKeyFrame == 36)
                    PlaySoundOnce(L"mb_bruceloong01_atk05dilie", 0.5f);
            }
            break;
            case 3: // Attack15
            {
                if (iCurrentKeyFrame == 13)
                    PlaySoundOnce(L"mb_bruceloong01_atk15juqi", 0.5f);
                else if (iCurrentKeyFrame == 37)
                    PlaySoundOnce(L"mb_bruceloong01_atk15", 0.5f);
            }
            break;
            case 4: // Attack05
            {
                if (iCurrentKeyFrame == 13)
                    PlaySoundOnce(L"mb_bruceloong01_atk05", 0.5f);
                else if (iCurrentKeyFrame == 36)
                    PlaySoundOnce(L"mb_bruceloong01_atk05dilie", 0.5f);
            }
            break;
            case 5: // Attack12_1
            {
                if (iCurrentKeyFrame == 31)
                    PlaySoundOnce(L"mb_bruceloong02_atk1202", 0.5f);
            }
            break;
            }
        }
        else // 2페
        {
            switch (Phase2LongPatternIdx)
            {
            case 0: // Attack21
            {
                if (iCurrentKeyFrame == 28)
                    PlaySoundOnce(L"mb_bruceloong01_atk2201", 0.5f);
                else if (iCurrentKeyFrame == 50)
                    PlaySoundOnce(L"mb_bruceloong01_atk2202", 0.5f);
            }
            break;
            case 1: // Attack22
            {
                if (iCurrentKeyFrame == 50)
                    PlaySoundOnce(L"mb_bruceloong02_atk07", 0.5f);
            }
            break;
            case 2: // Attack32
            {
                if (iCurrentKeyFrame == 34)
                    PlaySoundOnce(L"mb_bruceloong02_atk32", 0.5f);
                else if (iCurrentKeyFrame == 63)
                    PlaySoundOnce(L"mb_bruceloong02_atk32qitiao", 0.5f);

            }
            break;
            case 3: // Attack13_1
            {
                if (iCurrentKeyFrame == 13)
                    PlaySoundOnce(L"mb_bruceloong02_atk38", 0.5f);

            }
            break;
            case 4: // Attack27
            {
                if (iCurrentKeyFrame == 14)
                    PlaySoundOnce(L"mb_bruceloong02_atk27juqi", 0.5f);
                else if (iCurrentKeyFrame == 36)
                    PlaySoundOnce(L"mb_bruceloong02_atk27", 0.5f);

            }
            break;
            case 5: // Attack12
            {
                if (iCurrentKeyFrame == 21)
                    PlaySoundOnce(L"mb_bruceloong01_atk18", 0.5f);
                else if (iCurrentKeyFrame == 46)
                    PlaySoundOnce(L"mb_bruceloong01_atk21", 0.5f);

            }
            break;
            }
        }
        break;
    }
}

void Client::CBoss_Pushin::Move()
{
    if (m_iCurrentState >= Born)
        return;

    if (m_iCurrentState == HitDown)
    {
        moveVelocity = Vector3(0.f, 0.f, 0.f);
        return;
    }

    // 타겟 주시
    Vector3 LookAtDir = Vector3::Lerp(m_vMyLook, m_vTargetDir, 0.03f * g_TimeScale_Monster);
    LookAtDir.y = 0.f;
    //if (m_iCurrentState <= WalkR ||
    //    (m_iCurrentState <= LongAttack && m_iCurrentState >= MeleeAttack)) //&& m_dCurrentAnimFrame < 0.2))
    //    m_pTransformCom->LookAt(XMVectorSetW(m_vMyPos + LookAtDir, 1.f));
    m_pTransformCom->LookAt(XMVectorSetW(m_vMyPos + LookAtDir, 1.f));

    if (m_iCurrentState == Idle)
    {
        if (m_fTargetDist > 4.5f)
            Change_State(LongAttack);
        else
            Change_State(MeleeAttack);
    }

    if (m_iCurrentState == LongAttack)
    {
        // 2페 돌진기 이동량 설정
        if (isSecondPhase)
        {
            if (Phase2LongPatternIdx == 0)
            {
                if (m_dCurrentAnimFrame > 0.01 && m_dCurrentAnimFrame < 0.15)
                {
                    moveVelocity = Vector3(0.f, 0.f, m_fJumpDist * 1.5f);
                    return;
                }
            }
            else if (Phase2LongPatternIdx == 2)
            {
                if (m_dCurrentAnimFrame > 0.4 && m_dCurrentAnimFrame < 0.5)
                {
                    moveVelocity = Vector3(0.f, 0.f, m_fJumpDist * 3.f);
                    return;
                }
            }
            else if (Phase2LongPatternIdx == 5)
            {
                if (m_dCurrentAnimFrame > 0.2 && m_dCurrentAnimFrame < 0.5)
                {
                    moveVelocity = Vector3(0.f, 0.f, m_fJumpDist * 1.5f);
                    return;
                }
            }
        }    
        // 1페 돌진기 이동량 설정
        else
        {
            if (Phase1LongPatternIdx == 0)
            {
                if (m_dCurrentAnimFrame > 0.4 && m_dCurrentAnimFrame < 0.5)
                {
                    moveVelocity = Vector3(0.f, 0.f, m_fJumpDist * 3.f);
                    return;
                }
            }
            else if (Phase1LongPatternIdx == 2 || Phase1LongPatternIdx == 4)
            {
                if (m_dCurrentAnimFrame > 0.2 && m_dCurrentAnimFrame < 0.4)
                {
                    moveVelocity = Vector3(0.f, 0.f, m_fJumpDist * 2.f);
                    return;
                }
            }
        }
    }

    moveVelocity = Vector3(0.f, 0.f, 0.f);
}

void Client::CBoss_Pushin::Hit(_uint iDamage)
{
    if (m_iCurrentState >= Born)
        return;

    __super::Hit(iDamage);

    m_iCurrentHp -= iDamage;

    if (m_iCurrentShield <= 0)
    {
        if (m_iCurrentState == MonsterState::hit && m_dCurrentAnimFrame < 0.3)
            return;

        Change_State(MonsterState::hit);
    }
}

void Client::CBoss_Pushin::OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
    if (pCollider->Get_ColliderLayer() == Col_Player)
    {
        moveVelocity = Vector3(0.f, 0.f, 0.f);

        if (pSrcCol == m_MapColliderCom[L"Col_Attack1"] || pSrcCol == m_MapColliderCom[L"Col_Attack2"])
        {
            static_pointer_cast<CMovableObject>(pCollider->Get_Owner().lock())->Hit(m_iAttackPower);
        }
    }
}

void Client::CBoss_Pushin::OnCollisionStay(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
}

shared_ptr<CBoss_Pushin> Client::CBoss_Pushin::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& monsterDesc)
{
    shared_ptr<CBoss_Pushin> pInstance = make_shared<CBoss_Pushin>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(monsterDesc)))
    {
        MSG_BOX("Failed to Created : CBoss_Pushin");
        pInstance.reset();
    }

    return pInstance;
}
