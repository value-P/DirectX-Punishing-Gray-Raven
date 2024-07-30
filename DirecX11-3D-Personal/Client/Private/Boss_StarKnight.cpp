#include "pch.h"
#include "Boss_StarKnight.h"
#include "ThrowObject.h"
#include "CameraReference.h"
#include "LaserObject.h"
#include "TrailFX.h"
#include "Bone.h"
#include "ParticleEffect.h"
#include "DistortionPointRect.h"

Client::CBoss_StarKnight::CBoss_StarKnight(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CMonsterBase(pDevice, pContext)
{
    m_StateFunctions.resize(MonsterStateEnd);
    m_strMovableObjName = L"광휘의 행진자";
}

HRESULT Client::CBoss_StarKnight::Initialize(const INIT_DESC& monsterDesc)
{
    if (FAILED(__super::Initialize(monsterDesc)))
        return E_FAIL;

    if (FAILED(Add_StateFunction()))
        return E_FAIL;

    if (FAILED(Add_Component(monsterDesc.eLevel, monsterDesc.strModelKey))) 
        return E_FAIL;

    if (FAILED(Load_MeshEffects(monsterDesc.eLevel, monsterDesc.eLevel, L"MonsterEffect", &g_TimeScale_Monster, L"../Bin/EffectPrefab/MeshEffect/StartKnight")))
        return E_FAIL;

    if (FAILED(Load_ParticleEffects(monsterDesc.eLevel, monsterDesc.eLevel, L"MonsterEffect", &g_TimeScale_Monster, L"../Bin/EffectPrefab/ModelParticleList/Mb1StarknightMd010001.json")))
        return E_FAIL;

    if (FAILED(Init_Skills(monsterDesc)))
        return E_FAIL;

    if (FAILED(Add_Child(monsterDesc.eLevel)))
        return E_FAIL;

    Change_State(Born_1);
    Change_Anim(8);

    return S_OK;
}

void Client::CBoss_StarKnight::Priority_Tick(_float fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);

    Vector3 vDirection = m_vTargetPos - m_vMyPos;
    m_vCenterWithTarget = m_vMyPos + (vDirection * 0.5f);
}

_int Client::CBoss_StarKnight::Tick(_float fTimeDelta)
{

#ifdef _DEBUG
    if (m_pGameInstance.lock()->GetKey(DIK_P))
        m_iCurrentHp = 0;
#endif 

    if(!m_bStopAnim)
        m_pMeshCom->Play_Animation(fTimeDelta, m_vRootMotionVelocity, m_bLastAnimFrameCalled, m_dCurrentAnimFrame, true, true, true);

    __super::Tick(fTimeDelta);
    
    if (m_iCurrentHp <= 0 && m_iCurrentState < Death)
        Change_State(Death);

    Move();

    if (m_vRootMotionVelocity.Length() > 0.f)
    {
        m_pTransformCom->Go_Right(m_vRootMotionVelocity.x, m_pNavigationCom);

        if(m_fTargetDist > 3.f)
            m_pTransformCom->Go_Straight(-m_vRootMotionVelocity.z, m_pNavigationCom);
    }

    _float y = m_pNavigationCom->Get_HeightOnNav(m_pTransformCom->Get_Pos());
    m_pTransformCom->Set_Y(y);

    m_MapColliderCom[L"Col_Body"]->Tick(m_pCamRef->Get_WorldMatrix());

    PlaySound();

    return OBJ_ALIVE;
}

void Client::CBoss_StarKnight::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_pTransformCom->Go_Straight(moveVelocity.Length() * fTimeDelta);

    if (m_pRightHandTrail->isPlaying() && m_pLeftHandTrail->isPlaying())
    {
        Matrix matWorld = m_pTransformCom->Get_WorldMatrix();
        matLeftHand = m_pLeftHandBone.lock()->Get_CombinedTransformationMatrix() * matWorld;
        matRightHand = m_RightHandBone.lock()->Get_CombinedTransformationMatrix() * matWorld;

        m_pLeftHandTrail->Tick(matLeftHand);
        m_pRightHandTrail->Tick(matRightHand);
    }

    if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;
}

HRESULT Client::CBoss_StarKnight::Render()
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

HRESULT Client::CBoss_StarKnight::Add_StateFunction()
{
#pragma region 스탠딩
    m_StateFunctions[MonsterState::Idle].OnEnterState = [&](_float fTimeDelta) { Change_Anim(36);};
#pragma endregion
    
#pragma region 이동
    m_StateFunctions[MonsterState::BackDash].OnEnterState = [&](_float fTimeDelta) { Change_Anim(1); };
    m_StateFunctions[MonsterState::BackDash].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };

    m_StateFunctions[MonsterState::RightDash].OnEnterState = [&](_float fTimeDelta) { Change_Anim(3); };
    m_StateFunctions[MonsterState::RightDash].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };

    m_StateFunctions[MonsterState::LeftDash].OnEnterState = [&](_float fTimeDelta) { Change_Anim(2); };
    m_StateFunctions[MonsterState::LeftDash].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };

    m_StateFunctions[MonsterState::FrontDash].OnEnterState = [&](_float fTimeDelta) { Change_Anim(0); };
    m_StateFunctions[MonsterState::FrontDash].OnState = [&](_float fTimeDelta) { if (moveVelocity.Length() < 0.1f) { Change_State(Idle); } };
#pragma endregion

#pragma region 공격
    m_StateFunctions[MonsterState::Attack].OnEnterState = [&](_float fTimeDelta) { Change_Anim(Pattern[PatternIdx]); };
    m_StateFunctions[MonsterState::Attack].OnState = [&](_float fTimeDelta) { if (m_bLastAnimFrameCalled) Change_State(Idle); };
    m_StateFunctions[MonsterState::Attack].OnExitState = [&](_float fTimeDelta) {
        PatternIdx++;
            _int iSize = sizeof(Pattern) / sizeof(Pattern[0]);
            if (PatternIdx >= iSize)
                PatternIdx = 0;

            m_iPatternCount++;

            resetSoundInfo();
        };

    m_StateFunctions[MonsterState::ParryFailedPattern].OnEnterState = [&](_float fTimeDelta) { Change_Anim(ParringFailedPattern[ParringFailedPatternIdx]);};
    m_StateFunctions[MonsterState::ParryFailedPattern].OnState = [&](_float fTimeDelta) {
        if (m_bLastAnimFrameCalled)
        {
            _int iSize = sizeof(ParringFailedPattern) / sizeof(ParringFailedPattern[0]);
            if (ParringFailedPatternIdx < iSize - 1)
                Change_State(ParryFailedPattern);
            else
                Change_State(Idle);
        }
        };
    m_StateFunctions[MonsterState::ParryFailedPattern].OnExitState = [&](_float fTimeDelta) {
            ParringFailedPatternIdx++;
            _int iSize = sizeof(ParringFailedPattern) / sizeof(ParringFailedPattern[0]);
            if (ParringFailedPatternIdx >= iSize)
                ParringFailedPatternIdx = 0;

            resetSoundInfo();
        };

    m_StateFunctions[MonsterState::PreParringAttack].OnEnterState = [&](_float fTimeDelta) { Change_Anim(21); };
    m_StateFunctions[MonsterState::PreParringAttack].OnState = [&](_float fTimeDelta) {if (m_bLastAnimFrameCalled) Change_State(ParryFailedPattern); };
    
    m_StateFunctions[MonsterState::MainParringAttack].OnEnterState = [&](_float fTimeDelta) {
        // 플레이어와 보스 사이에 디스토션이 생성되도록
        m_pParryDistortion->Get_Transform()->Set_Pos(m_vCenterWithTarget);
        m_pParryDistortion->PlayOnce();
        m_MapColliderCom[L"Col_Attack1"]->Set_Activate(false);
        m_MapColliderCom[L"Col_Attack2"]->Set_Activate(false);
        m_MapColliderCom[L"Col_Parry"]->Set_Activate(false);
        m_pRightHandTrail->Stop();
        m_pLeftHandTrail->Stop();

        Change_Anim(25);
        };
    m_StateFunctions[MonsterState::MainParringAttack].OnState = [&](_float fTimeDelta) {if (m_bLastAnimFrameCalled) Change_State(Idle); };

#pragma endregion

#pragma region 피격
    
    m_StateFunctions[MonsterState::ParrySuccessPattern].OnEnterState = [&](_float fTimeDelta) {
        // 플레이어와 보스 사이에 디스토션이 생성되도록
        m_pParryDistortion->Get_Transform()->Set_Pos(m_vCenterWithTarget);
        m_pParryDistortion->PlayOnce();
        m_MapColliderCom[L"Col_Attack1"]->Set_Activate(false);
        m_MapColliderCom[L"Col_Attack2"]->Set_Activate(false);
        m_MapColliderCom[L"Col_Parry"]->Set_Activate(false);
        m_pRightHandTrail->Stop();
        m_pLeftHandTrail->Stop();

        Change_Anim(33);
        };
    m_StateFunctions[MonsterState::ParrySuccessPattern].OnState = [&](_float fTimeDelta) { 
        if (m_pMeshCom->Get_CurrentKeyFrame() == 63 && !m_bGroggyCalled)
        {
            m_StopWatchs[SW_3].StartCheck();
            m_bStopAnim = true;
            m_bGroggyCalled = true;
        }

        if (m_bStopAnim)
        {
            if (m_StopWatchs[SW_3].fTime >= 3.5f)
            {
                m_bStopAnim = false;
                m_StopWatchs[SW_3].Clear();
            }
        }

        if (m_bLastAnimFrameCalled) Change_State(Idle);
        };
    m_StateFunctions[MonsterState::ParrySuccessPattern].OnExitState = [&](_float fTimeDelta) { m_bGroggyCalled = false; };

    m_StateFunctions[MonsterState::Death].OnEnterState = [&](_float fTimeDelta) { 
        m_MapColliderCom[L"Col_Attack1"]->Set_Activate(false);
        m_MapColliderCom[L"Col_Attack2"]->Set_Activate(false);
        m_MapColliderCom[L"Col_Parry"]->Set_Activate(false);
        Change_Anim(33); 
        };
    m_StateFunctions[MonsterState::Death].OnState = [&](_float fTimeDelta) {if (m_pMeshCom->Get_CurrentKeyFrame() == 63) { m_bStopAnim = true; } };
#pragma endregion
    
#pragma region 연출
    m_StateFunctions[MonsterState::Born_1].OnEnterState = [&](_float fTimeDelta) { Change_Anim(8); };
    m_StateFunctions[MonsterState::Born_1].OnState = [&](_float fTimeDelta) {if (m_bLastAnimFrameCalled) { Change_State(Born_2); }};
    m_StateFunctions[MonsterState::Born_2].OnEnterState = [&](_float fTimeDelta) { Change_Anim(7); };
    m_StateFunctions[MonsterState::Born_2].OnState = [&](_float fTimeDelta) {if (m_bLastAnimFrameCalled) { Change_State(Idle); }};

#pragma endregion

    return S_OK;
}

HRESULT Client::CBoss_StarKnight::Add_Component(LEVEL eLevel, const wstring& strModelKey)
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

    // 패링 콜라이더
    sphereColDesc.vCenter = Vector3(0.f, 1.f, 0.f);
    sphereColDesc.fRadius = 7.5f;

    colliderDesc.eType = CCollider::TYPE_SPHERE;
    colliderDesc.iLayerType = Col_MonsterParry;
    colliderDesc.pInitDesc = make_shared<CBounding_Sphere::INIT_DESC>(sphereColDesc);
    colliderDesc.pOwner = shared_from_this();
    colliderDesc.iCreateLevelIndex = eLevel;
    shared_ptr<CCollider> pParryCol = CCollider::Create(m_pDevice, m_pContext, colliderDesc);
    pParryCol->Set_Activate(false);
    m_MapColliderCom.emplace(L"Col_Parry", pParryCol);

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

HRESULT Client::CBoss_StarKnight::Bind_ShaderResources()
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

HRESULT Client::CBoss_StarKnight::Set_EventFunctions()
{
    if (FAILED(__super::Set_EventFunctions()))
        return E_FAIL;

    //"EnableBodyCollider"
    //"DisAbleBodyCollider"
    //"EnAbleAttackCollider1"
    //"DisAbleAttackCollider1"
    //"EnAbleAttackCollider2"
    //"DisAbleAttackCollider2"
    //"EnAbleAttackCollider3" : 땅레이저
    //"DisAbleAttackCollider3"
    //"ActiveParryFlag"
    //"InActiveParryFlag"
    //"UseSkill1" : 기본총알발사
    //"UseSkill2" : x검기
    //"UseSkill3" : 중력포
    //"UseSkill4" : 가슴레이저

    EventFunctions["EnAbleAttackCollider1"] = [&]() { m_MapColliderCom[L"Col_Attack1"]->Set_Activate(true); };
    EventFunctions["DisAbleAttackCollider1"] = [&]() {m_MapColliderCom[L"Col_Attack1"]->Set_Activate(false); };
    EventFunctions["EnAbleAttackCollider2"] = [&]() {m_MapColliderCom[L"Col_Attack2"]->Set_Activate(true); };
    EventFunctions["DisAbleAttackCollider2"] = [&]() {m_MapColliderCom[L"Col_Attack2"]->Set_Activate(false); };
    EventFunctions["ActiveParryFlag"] = [&]() {m_MapColliderCom[L"Col_Parry"]->Set_Activate(true); }; 
    EventFunctions["InActiveParryFlag"] = [&]() {m_MapColliderCom[L"Col_Parry"]->Set_Activate(false); };
    //EventFunctions["UseSkill1"] = [&]() {
    //    Vector3 vMyPos = m_pCamRef->Get_Pos();
    //    Vector3 vTargetPos = m_vTargetPos;
    //    vTargetPos.y += 1.5f;
    //    Vector3 vDir = vTargetPos - vMyPos;
    //    vDir.Normalize();
    //    vTargetPos = vMyPos + vDir;
    //    m_pSkill1.PopInActivate()->Throw(vMyPos, vTargetPos);
    //    };
    EventFunctions["UseSkill1"] = [&]() {
        Vector3 vMyPos = m_pCamRef->Get_Pos();
        Vector3 vTargetPos = m_vTargetPos;
        vTargetPos.y += 1.5f;
        Vector3 vDir = vTargetPos - vMyPos;
        vDir.Normalize();
        vTargetPos = vMyPos + vDir;
        m_pSkill1->Throw(vMyPos, vTargetPos);
        };
    EventFunctions["UseSkill2"] = [&]() {
        Vector3 vMyPos = m_pCamRef->Get_Pos() + m_vMyLook;
        Vector3 vTargetPos = vMyPos + m_vMyLook;
        m_pSkill2->Throw(vMyPos, vTargetPos);
        };
    EventFunctions["UseSkill3"] = [&]() {
        m_pSkill3->Throw(m_pCamRef->Get_Pos(), m_vTargetPos);
        };
    EventFunctions["UseSkill4"] = [&]() {
        m_pSkill4->ActiveRotationRage(XMConvertToRadians(90.f),XMConvertToRadians(30.f));
        };
    EventFunctions["PlayTrail"] = [&]() {
        m_pRightHandTrail->PlayLoop(); 
        m_pLeftHandTrail->PlayLoop(); 
        };
    EventFunctions["StopTrail"] = [&]() {
        m_pRightHandTrail->Stop();
        m_pLeftHandTrail->Stop();
        };

    return S_OK;
}

HRESULT Client::CBoss_StarKnight::Init_Skills(const INIT_DESC& monsterDesc)
{
    CThrowObject::INIT_DESC desc;
    desc.fSpeed = 40.f;
    desc.vInitPos = m_pCamRef->Get_Pos();
    desc.iDamage = m_iAttackPower;
    desc.eCreateLevel = monsterDesc.eLevel;
    desc.fDuration = 4.f;
    desc.fColRadius = 1.f;
    desc.strSkillPrefabPath_1 = L"../Bin/EffectPrefab/MeshEffect/StartKnight/IronBall.MeshFX";
    shared_ptr<CThrowObject> pSkill1;

    //for (_int iBulletNum = 0; iBulletNum < 10; ++iBulletNum)
    //{
    //    pSkill1 = CThrowObject::Create(m_pDevice, m_pContext, desc);
    //    m_pSkill1.PushBack(pSkill1, monsterDesc.eLevel, L"Layer_MonsterSkill", &g_TimeScale_Monster);
    //}
    m_pSkill1 = CThrowObject::Create(m_pDevice, m_pContext, desc);
    if (nullptr == m_pSkill1)
        return E_FAIL;

    if (FAILED(m_pGameInstance.lock()->Add_Object(monsterDesc.eLevel, L"Layer_MonsterSkill", m_pSkill1, &g_TimeScale_Monster)))
        return E_FAIL;

    // ------------------------------------------
    desc.fSpeed = 20.f;
    desc.vInitPos = m_pTransformCom->Get_Pos();
    desc.iDamage = m_iAttackPower;
    desc.eCreateLevel = monsterDesc.eLevel;
    desc.fDuration = 4.f;
    desc.fColRadius = 2.f;
    desc.strSkillPrefabPath_1 = L"../Bin/EffectPrefab/MeshEffect/StartKnight/FxBaiyangnvzhuazi004_1.MeshFX";
    desc.strSkillPrefabPath_2 = L"../Bin/EffectPrefab/MeshEffect/StartKnight/FxBaiyangnvzhuazi004_2.MeshFX";
    m_pSkill2 = CThrowObject::Create(m_pDevice, m_pContext, desc);
    if (nullptr == m_pSkill2)
        return E_FAIL;

    if (FAILED(m_pGameInstance.lock()->Add_Object(monsterDesc.eLevel, L"Layer_MonsterSkill", m_pSkill2, &g_TimeScale_Monster)))
        return E_FAIL;

    // ------------------------------------------
    desc.fSpeed = 20.f;
    desc.vInitPos = m_pTransformCom->Get_Pos();
    desc.iDamage = m_iAttackPower;
    desc.eCreateLevel = monsterDesc.eLevel;
    desc.fDuration = 4.f;
    desc.fColRadius = 3.f;
    m_pSkill3 = CThrowObject::Create(m_pDevice, m_pContext, desc);
    if (nullptr == m_pSkill3)
        return E_FAIL;

    if (FAILED(m_pGameInstance.lock()->Add_Object(monsterDesc.eLevel, L"Layer_MonsterSkill", m_pSkill3, &g_TimeScale_Monster)))
        return E_FAIL;

    // ------------------------------------------
    CLaserObject::INIT_DESC laserDesc;
    laserDesc.fDuration = 2.2f;
    laserDesc.eCreateLevel = monsterDesc.eLevel;
    laserDesc.vInitPos = m_vMyPos;
    laserDesc.iDamage = m_iAttackPower;
    laserDesc.vColExtents = Vector3(0.3f, 0.3f, 10.f);
    laserDesc.vColOffset = Vector3(0.f, 0.8f, 10.f);
    laserDesc.pSocketBone = m_pMeshCom->Get_Bone("Bip001");
    laserDesc.strSkillPrefabPath = L"../Bin/EffectPrefab/MeshEffect/StartKnight/FxMbStarnightRay901.MeshFX";
    m_pSkill4 = CLaserObject::Create(m_pDevice, m_pContext, laserDesc);
    if (nullptr == m_pSkill4)
        return E_FAIL;

    __super::Add_Children(L"Skill4", m_pSkill4);

    // 피격 이펙트 생성
    CParticleEffect::INIT_DESC particleDesc;
    particleDesc.eLevel = monsterDesc.eLevel;
    particleDesc.FXPrefabPath = L"../Bin/EffectPrefab/ParticleEffect/VeraHit.Particle";
    m_pHitParticle = CParticleEffect::Create(m_pDevice, m_pContext, particleDesc);
    if (FAILED(m_pGameInstance.lock()->Add_Object(monsterDesc.eLevel, L"Layer_MonsterEffect", m_pHitParticle, &g_TimeScale_Player)))
    {
        MSG_BOX("CBoss_StarKnight::Init_Skills");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT Client::CBoss_StarKnight::Add_Child(LEVEL eLevel)
{
    // 소켓본 저장
    m_pLeftHandBone = m_pMeshCom->Get_Bone("Bip001LHand");
    m_RightHandBone = m_pMeshCom->Get_Bone("Bip001RHand");
    if (nullptr == m_pLeftHandBone.lock() || nullptr == m_RightHandBone.lock())
        return E_FAIL;

    CTrailFX::INIT_DESC initDesc;

    initDesc.iMaxTrailRectNum = 30;
    initDesc.trailTexKey = L"FxMb1Bruceloong1208";
    initDesc.vLocalUpperPos = Vector3(2.f, 0.f, 0.f);
    initDesc.vLocalDownPos = Vector3(-1.f, 0.f, 0.f);
    initDesc.vColor = Vector4(1.f, 1.f, 0.f, 1.f);
    m_pLeftHandTrail = CTrailFX::Create(m_pDevice, m_pContext, initDesc);
    if (nullptr == m_pLeftHandTrail)
        return E_FAIL;

    initDesc.iMaxTrailRectNum = 30;
    initDesc.trailTexKey = L"FxMb1Bruceloong1208";
    initDesc.vLocalUpperPos = Vector3(2.f, 0.f, 0.f);
    initDesc.vLocalDownPos = Vector3(-1.f, 0.f, 0.f);
    initDesc.vColor = Vector4(1.f, 1.f, 0.f, 1.f);
    m_pRightHandTrail = CTrailFX::Create(m_pDevice, m_pContext, initDesc);
    if (nullptr == m_pRightHandTrail)
        return E_FAIL;

    // 패링 디스토션 이펙트
    CDistortionPointRect::INIT_DESC distortRectDesc;
    distortRectDesc.eCreatLevel = eLevel;
    distortRectDesc.fDuration = 0.6f;
    distortRectDesc.fStartDistortionPower = 1.f;
    distortRectDesc.fEndDistortionPower = 0.f;
    distortRectDesc.strMaskTexKey = L"TO14_T_FX_Round_SO_03";
    distortRectDesc.strNoiseTexKey = L"Distortion";
    distortRectDesc.vStartUvSpeed = Vector2(0.f, 0.f);
    distortRectDesc.vEndUvSpeed = Vector2(0.f, 0.f);
    distortRectDesc.vStartScale = Vector2(1.f, 1.f);
    distortRectDesc.vEndScale = Vector2(30.f, 30.f);

    m_pParryDistortion = CDistortionPointRect::Create(m_pDevice, m_pContext, distortRectDesc);
    m_pGameInstance.lock()->Add_Object(eLevel, L"MonsterEffect", m_pParryDistortion, &g_TimeScale_Player);

    return S_OK;
}

void Client::CBoss_StarKnight::PlaySound()
{
    _int iCurrentKeyFrame = m_pMeshCom->Get_CurrentKeyFrame();

    switch (m_iCurrentState)
    {
    case Attack:
    {
        switch (PatternIdx)
        {
        case 0:
            if (iCurrentKeyFrame == 2)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_2001_xuli", 0.5f);
            else if (iCurrentKeyFrame == 30)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_2001_ray_hit", 0.5f);
            break;
        case 1:
            if (iCurrentKeyFrame == 9)
                PlaySoundOnce(L"mb_starknightLancebonefm_01_burst_01", 0.5f);
            else if (iCurrentKeyFrame == 41)
                PlaySoundOnce(L"mb_starknight_atk_02", 0.5f);
            break;
        case 2:
            if (iCurrentKeyFrame == 10)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_2301", 0.5f);
            else if (iCurrentKeyFrame == 36)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_2302", 0.5f);
            else if (iCurrentKeyFrame == 64)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_2303_bip_001_rhand", 0.5f);
            break;
        case 3:
            if (iCurrentKeyFrame == 28)
                PlaySoundOnce(L"mb_starknight_atk_13_dandao", 0.5f);
            else if (iCurrentKeyFrame == 48)
                PlaySoundOnce(L"mb_starknight_atk_13_hit", 0.5f);
            break;
        case 4:
            if (iCurrentKeyFrame == 1)
                PlaySoundOnce(L"mb_starknightLancebonefm_01_burst_01", 0.5f);
            else if (iCurrentKeyFrame == 38)
                PlaySoundOnce(L"mb_starknight_atk_02", 0.5f);
            else if (iCurrentKeyFrame == 89)
                PlaySoundOnce(L"mb_starknight_atk_02", 0.5f);
            break;
        case 5:
            if (iCurrentKeyFrame == 4)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_2001_xuli", 0.5f);
            else if (iCurrentKeyFrame == 37)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_2101", 0.5f);
            break;
        case 6:
            if (iCurrentKeyFrame == 21)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_4001_dilie", 0.5f);
            break;
        case 7:
            if (iCurrentKeyFrame == 1)
                PlaySoundOnce(L"mb_starknightLancebonefm_01_burst_01", 0.5f);
            else if (iCurrentKeyFrame == 41)
                PlaySoundOnce(L"mb_starknight_atk_02", 0.5f);
            break;
        case 8:
            if (iCurrentKeyFrame == 12)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_2901_sound", 0.5f);
            else if (iCurrentKeyFrame == 66)
                PlaySoundOnce(L"mb_starknight_atk_03", 0.5f);
            else if (iCurrentKeyFrame == 113)
                PlaySoundOnce(L"mb_starknight_atk_04", 0.5f); 
            break;
        case 9:
            if (iCurrentKeyFrame == 13)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_4001_dilie", 0.5f);
            else if (iCurrentKeyFrame == 49)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_4101", 0.5f);
            break;
        case 10:
            if (iCurrentKeyFrame == 4)
                PlaySoundOnce(L"mb_starknight_atk_43_sound", 0.5f);
            else if (iCurrentKeyFrame == 47)
                PlaySoundOnce(L"mb_starknight_atk_43_chongci", 0.5f);
            break;
        }
    }
        break;
    case ParryFailedPattern:
        switch (ParringFailedPatternIdx)
        {
        case 0:
            if (iCurrentKeyFrame == 0)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_2701_bone", 0.5f);
            else if(iCurrentKeyFrame == 15)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_2701", 0.5f);
            break;
        case 1:
            if (iCurrentKeyFrame == 0)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_2701_bone", 0.5f);
            else if (iCurrentKeyFrame == 15)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_2701", 0.5f);
            break;
        case 2:
            if (iCurrentKeyFrame == 0)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_3501_bone", 0.5f);
            else if (iCurrentKeyFrame == 34)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_3501", 0.5f);
            break;
        case 3:
            if (iCurrentKeyFrame == 12)
                PlaySoundOnce(L"mb_starknight_md_010001_atk_2901_sound", 0.5f);
            else if (iCurrentKeyFrame == 66)
                PlaySoundOnce(L"mb_starknight_atk_03", 0.5f);
            else if (iCurrentKeyFrame == 113)
                PlaySoundOnce(L"mb_starknight_atk_04", 0.5f);
            break;
        }
        break;
    case ParrySuccessPattern:
    {
        if (iCurrentKeyFrame == 13)
            PlaySoundOnce(L"mb_starknight_atk_42_sound", 1.f);
    }
        break;
    case MainParringAttack:
    {
        if (iCurrentKeyFrame == 3)
            PlaySoundOnce(L"mb_starknight_md_010001_atk_3001", 1.f);
        else if (iCurrentKeyFrame == 31)
            PlaySoundOnce(L"mb_starknight_md_010001_atk_3001_dilie", 1.f);
    }
        break;
    case PreParringAttack:
    {
        if (iCurrentKeyFrame == 2)
            PlaySoundOnce(L"mb_starknight_md_010001_atk_2401_dilie", 1.f);
        else if (iCurrentKeyFrame == 28)
            PlaySoundOnce(L"mb_starknight_md_010001_atk_2701_bone", 1.f);
    }
        break;
    }
}

void Client::CBoss_StarKnight::Move()
{
    if (m_iCurrentState >= Born_1)
        return;

    // 타겟 주시
    Vector3 LookAtDir = Vector3::Lerp(m_vMyLook, m_vTargetDir, 0.1f * g_TimeScale_Monster);
    LookAtDir.y = 0.f;
    m_pTransformCom->LookAt(XMVectorSetW(m_vMyPos + LookAtDir, 1.f));

    if (m_iCurrentState == Idle)
    {
        if (m_iPatternCount == 4)
        {
            m_iPatternCount = 0;
            Change_State(PreParringAttack);
        }
        else
            Change_State(Attack);    
    }

    if (m_iCurrentState == Attack && PatternIdx != 0)
    {
        if (m_fTargetDist > 6.f && m_dCurrentAnimFrame < 0.4)
        {
            moveVelocity = m_vTargetDir * m_fTargetDist;
        }
        else
            moveVelocity = Vector3(0.f, 0.f, 0.f);
    }
    else if (m_iCurrentState == ParryFailedPattern && ParringFailedPatternIdx == 3)
    {
        if (m_fTargetDist > 6.f && m_dCurrentAnimFrame < 0.4)
        {
            moveVelocity = m_vTargetDir * m_fTargetDist;
        }
        else
            moveVelocity = Vector3(0.f, 0.f, 0.f);
    }
}

void Client::CBoss_StarKnight::Hit(_uint iDamage)
{
    if (m_iCurrentState >= Born_1)
        return;

    __super::Hit(iDamage);

    m_iCurrentHp -= iDamage;
}

void Client::CBoss_StarKnight::OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
    if (pCollider->Get_ColliderLayer() == Col_Player)
    {
        moveVelocity = Vector3(0.f, 0.f, 0.f);

        if (pSrcCol == m_MapColliderCom[L"Col_Attack1"] || pSrcCol == m_MapColliderCom[L"Col_Attack2"])
        {
            static_pointer_cast<CMovableObject>(pCollider->Get_Owner().lock())->Hit(m_iAttackPower);
        }        
    }

    if (pCollider->Get_ColliderLayer() == Col_PlayerAttack)
    {
        if (pSrcCol == m_MapColliderCom[L"Col_Parry"])
        {
            if (m_iCurrentState == PreParringAttack)
            {
                Change_State(MainParringAttack);
                m_pGameInstance.lock()->Play_Sound(L"Parry1", 1.f);
            }
            else
            {
                Change_State(ParrySuccessPattern);
                m_pGameInstance.lock()->Play_Sound(L"Parry2", 1.f);
            }
        }
    }
}

void Client::CBoss_StarKnight::OnCollisionStay(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
    __super::OnCollisionStay(pSrcCol, pCollider);
}

shared_ptr<CBoss_StarKnight> Client::CBoss_StarKnight::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& monsterDesc)
{
    shared_ptr<CBoss_StarKnight> pInstance = make_shared<CBoss_StarKnight>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(monsterDesc)))
    {
        MSG_BOX("Failed to Created : CBoss_StarKnight");
        pInstance.reset();
    }

    return pInstance;
}
