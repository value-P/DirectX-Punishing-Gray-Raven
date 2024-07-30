#include "pch.h"
#include "MovableObject.h"
#include "Transform.h"
#include "AnimModelProp.h"
#include "CameraReference.h"
#include "MeshEffect.h"
#include "ParticleEffect.h"

Client::CMovableObject::CMovableObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice, pContext)
{
}

Vector3 Client::CMovableObject::Get_CamRefPos()
{
    return m_pCamRef->Get_Pos();
}

HRESULT Client::CMovableObject::Initialize(const Vector3& vInitPos)
{
    if(FAILED(__super::Initialize(vInitPos)))
        return E_FAIL;

    if (FAILED(Set_EventFunctions()))
        return E_FAIL;

    return S_OK;
}

void Client::CMovableObject::Priority_Tick(_float fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);

    KeyInput();

    // 자주 사용하는 자신과 타겟의 위치및 방향정보를 업데이트
    Update_Dir_Pos_Data();
}

_int Client::CMovableObject::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    for (auto& stopWatch : m_StopWatchs)
    {
        if (stopWatch.Start)
            stopWatch.fTime += fTimeDelta;
    }

    if (m_eTimeState == CMovableObject::OnExit)
    {
        if (m_StateFunctions[m_iCurrentState].OnExitState)
            m_StateFunctions[m_iCurrentState].OnExitState(fTimeDelta);
        m_iPrevState = m_iCurrentState;
        m_iCurrentState = m_iNextState;
        m_iNextState = -1;
        m_eTimeState = OnEnter;
    }

    switch (m_eTimeState)
    {
    case Client::CMovableObject::OnEnter:
    {
        if(m_StateFunctions[m_iCurrentState].OnEnterState)
            m_StateFunctions[m_iCurrentState].OnEnterState(fTimeDelta);
        m_eTimeState = Running;
        if (m_StateFunctions[m_iCurrentState].OnState)
            m_StateFunctions[m_iCurrentState].OnState(fTimeDelta);
    }
        break;
    case Client::CMovableObject::Running:
    {
        if (m_StateFunctions[m_iCurrentState].OnState)
            m_StateFunctions[m_iCurrentState].OnState(fTimeDelta);
    }
        break;
    }

    AnimEventTick(fTimeDelta);
    EffectEventTick(fTimeDelta);

    for (auto& Pair : m_MapColliderCom)
        Pair.second->Tick(m_pTransformCom->Get_WorldMatrix());
        
    return OBJ_ALIVE;
}

void Client::CMovableObject::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
    for (auto& Pair : m_MapColliderCom)
    {
        if (Pair.second->IsActivate())
            m_pGameInstance.lock()->Add_DebugComponent(Pair.second);
    }
#endif

    if (m_pGameInstance.lock()->isDarkMode())
    {
        m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_EXCEPTDARK, shared_from_this());
    }
}

HRESULT Client::CMovableObject::Render()
{
    if(FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT Client::CMovableObject::Render_Shadow()
{
    Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    Matrix ViewMatrix = *(m_pGameInstance.lock()->Get_LightViewMatrix(0));

    Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

    if (FAILED(m_pMeshCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pMeshCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pMeshCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    for (_uint i = 0; i < m_pMeshCom->Get_MeshCount(); ++i)
    {
        // 텍스처 바인딩
        if (FAILED(m_pMeshCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
            return E_FAIL;
        if (FAILED(m_pMeshCom->Bind_BoneMatrices("g_BoneMatrices", i)))
            return E_FAIL;

        m_pMeshCom->Render(i, 1);
    }

    return S_OK;
}

HRESULT Client::CMovableObject::Render_ExceptDark()
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

    for (_uint i = 0; i < m_pMeshCom->Get_MeshCount(); ++i)
    {
        // 텍스처 바인딩
        if (FAILED(m_pMeshCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
            return E_FAIL;
        if (FAILED(m_pMeshCom->Bind_BoneMatrices("g_BoneMatrices", i)))
            return E_FAIL;

        m_pMeshCom->Render(i, 2);
    }

    return S_OK;
}

void Client::CMovableObject::Hit(_uint iDamage)
{
    if (nullptr != m_pHitParticle)
    {
        if (!m_pHitParticle->isPlaying())
        {
            m_pHitParticle->Get_Transform()->Set_Pos(m_vMyPos + (m_vMyLook * 0.3f) + Vector3(0.f, 1.2f, 0.f));
            m_pHitParticle->PlayOnce();
        }
    }
}

HRESULT Client::CMovableObject::Set_EventFunctions()
{
    function<void()> emptyFunction;
    EventFunctions.emplace("EnableBodyCollider", emptyFunction);
    EventFunctions.emplace("DisAbleBodyCollider", emptyFunction);
    EventFunctions.emplace("EnAbleAttackCollider1", emptyFunction);
    EventFunctions.emplace("DisAbleAttackCollider1", emptyFunction);
    EventFunctions.emplace("EnAbleAttackCollider2", emptyFunction);
    EventFunctions.emplace("DisAbleAttackCollider2", emptyFunction);
    EventFunctions.emplace("EnAbleAttackCollider3", emptyFunction);
    EventFunctions.emplace("DisAbleAttackCollider3", emptyFunction);
    EventFunctions.emplace("ActiveParryFlag", emptyFunction);
    EventFunctions.emplace("InActiveParryFlag", emptyFunction);
    EventFunctions.emplace("UseSkill1", emptyFunction);
    EventFunctions.emplace("UseSkill2", emptyFunction);
    EventFunctions.emplace("UseSkill3", emptyFunction);
    EventFunctions.emplace("UseSkill4", emptyFunction);
    EventFunctions.emplace("PlayTrail", emptyFunction);
    EventFunctions.emplace("StopTrail", emptyFunction);
    EventFunctions.emplace("GroundEffectPlay", emptyFunction);

    return S_OK;
}

void Client::CMovableObject::Change_State(_int State)
{
    m_eTimeState = TimeState::OnExit;
    m_iNextState = State;
}

void Client::CMovableObject::Update_Dir_Pos_Data()
{
    m_vMyPos = m_pTransformCom->Get_Pos();
    m_vMyLook = m_pTransformCom->Get_Look();
    m_vMyRight = m_pTransformCom->Get_Right();

if (m_pTarget.lock() != nullptr)
{
    m_vTargetPos = m_pTarget.lock()->Get_Transform()->Get_Pos();
    m_vTargetDir = m_vTargetPos - m_vMyPos;
    m_vTargetDir.y = 0.f;
    m_fTargetDist = m_vTargetDir.Length();
    m_vTargetDir.Normalize();
}
}

void Client::CMovableObject::AnimEventTick(_float fTimeDelta)
{
    if (m_AnimEvents.empty())
        return;

    // 애니메이션 이름으로 keyframe정보 검색
    _int iCurrentAnimIndex = m_pMeshCom->Get_CurrentAnimIndex();
    auto KeyFrameData = m_AnimEvents.find(iCurrentAnimIndex);
    if (KeyFrameData != m_AnimEvents.end())
    {
        // 현재 키프레임으로 이벤트문자열들 검색
        _int iCurrentKeyFrame = m_pMeshCom->Get_CurrentKeyFrame();
        auto EventData = KeyFrameData->second.find(iCurrentKeyFrame);
        if (EventData != KeyFrameData->second.end())
        {
            auto strEvents = EventData->second;

            for (auto eventStr : strEvents)
            {
                if (EventFunctions[eventStr])
                    EventFunctions[eventStr]();
            }
        }
    }
}

void Client::CMovableObject::ReadAnimEventDataFromJson(const Json::Value& root, const shared_ptr<CAnimModelProp> pAnimModelProp)
{
    // 애니메이션 이름으로 순회하며 해당하는 인덱스를 키값으로 넣음
    for (auto Animation = root.begin(); Animation != root.end(); ++Animation)
    {
        string strAnim = Animation.key().asString();
        _int animIndex = pAnimModelProp->Get_AnimIndex(strAnim);

        // 애니메이션 이름과 매칭되는 인덱스를 키값으로 포함하는지 검색 후 없다면 추가
        auto keyFrameEvents = m_AnimEvents.find(animIndex);
        if (keyFrameEvents == m_AnimEvents.end())
        {
            KeyFrameEvents events;
            m_AnimEvents.emplace(animIndex, events);
        }

        // 키프레임인덱스를 순회, 배열데이터가 null이라면 넘어가기
        for (auto KeyFrame = Animation->begin(); KeyFrame != Animation->end(); KeyFrame++)
        {
            if (KeyFrame->empty())
                continue;

            // 해당 키프레임의 인덱스를 키값으로 하는배열있는지 확인 후 추가
            _int keyFrameIndex = KeyFrame.key().asInt();
            auto strVec = m_AnimEvents[animIndex].find(keyFrameIndex);
            if (strVec == m_AnimEvents[animIndex].end())
            {
                Events strEvents;
                m_AnimEvents[animIndex].emplace(keyFrameIndex, strEvents);
            }

            // 문자열 배열에 이벤트 문자열 채워넣기
            for (auto strEvent = KeyFrame->begin(); strEvent != KeyFrame->end(); ++strEvent)
            {
                m_AnimEvents[animIndex][keyFrameIndex].push_back(strEvent->asString());
            }
        }
    }

}

void Client::CMovableObject::EffectEventTick(_float fTimeDelta)
{
    if (m_EffectEvents.empty())
        return;

    // 애니메이션 이름으로 keyframe정보 검색
    _int iCurrentAnimIndex = m_pMeshCom->Get_CurrentAnimIndex();
    auto KeyFrameData = m_EffectEvents.find(iCurrentAnimIndex);
    if (KeyFrameData != m_EffectEvents.end())
    {
        // 현재 키프레임으로 이벤트문자열들 검색
        _int iCurrentKeyFrame = m_pMeshCom->Get_CurrentKeyFrame();
        auto EventData = KeyFrameData->second.find(iCurrentKeyFrame);
        if (EventData != KeyFrameData->second.end())
        {
            auto eventDatas = EventData->second;

            for (auto data : eventDatas)
            {
                auto Pair = EventEffects.find(data.strPrefabName);
                if (Pair != EventEffects.end())
                {
                    if (data.eFxType == FX_EVENT_DATA::FX_PARTICLE)
                    {
                        Vector3 vPos = m_pTransformCom->Get_Pos();
                        Vector3 vRight = m_vMyRight * data.vPivot.x;;
                        Vector3 vUp = m_pTransformCom->Get_Up() *  data.vPivot.y;
                        Vector3 vLook = m_vMyLook * data.vPivot.z;

                        vPos = vPos + vRight + vUp + vLook;

                        (*Pair).second->Get_Transform()->Set_Pos(vPos);
                        (*Pair).second->PlayOnce();
                        //(*Pair).second->Get_Transform()->Set_Pos(data.vPivot);
                        //(*Pair).second->PlayOnce();
                    }
                    else
                    {
                        (*Pair).second->Get_Transform()->Set_Pos(data.vPivot);
                        (*Pair).second->PlayOnce();
                    }
                }
            }
        }
    }

}

void Client::CMovableObject::ReadEffectEventDataFromJson(const Json::Value& root, const shared_ptr<CAnimModelProp> pAnimModelProp)
{
    for (auto Animation = root.begin(); Animation != root.end(); ++Animation)
    {
        string strAnim = Animation.key().asString();
        _int animIndex = pAnimModelProp->Get_AnimIndex(strAnim);

        // 애니메이션 이름과 매칭되는 인덱스를 키값으로 포함하는지 검색 후 없다면 추가
        auto keyFrameEffectEvents = m_EffectEvents.find(animIndex);
        if (keyFrameEffectEvents == m_EffectEvents.end())
        {
            EffectKeyFrameEvents events;
            m_EffectEvents.emplace(animIndex, events);
        }

        for (auto KeyFrame = Animation->begin(); KeyFrame != Animation->end(); ++KeyFrame)
        {
            if (KeyFrame->empty())
                continue;

            // 해당 키프레임의 인덱스를 키값으로 하는배열있는지 확인 후 추가
            _int keyFrameIndex = KeyFrame.key().asInt();
            auto eventData = m_EffectEvents[animIndex].find(keyFrameIndex);
            if (eventData == m_EffectEvents[animIndex].end())
            {
                EffectEvents events;
                m_EffectEvents[animIndex].emplace(keyFrameIndex, events);
            }

            // 데이터 채워넣기
            for (auto DatVec = KeyFrame->begin(); DatVec != KeyFrame->end(); ++DatVec)
            {
                FX_EVENT_DATA tempDat = {};
                tempDat.strPrefabName = (*DatVec)["Effect PrefabName"].asString();
                tempDat.eFxType = static_cast<FX_EVENT_DATA::FXType>((*DatVec)["Effect Type"].asInt());
                tempDat.vPivot.x = (*DatVec)["Pivot X"].asFloat();
                tempDat.vPivot.y = (*DatVec)["Pivot Y"].asFloat();
                tempDat.vPivot.z = (*DatVec)["Pivot Z"].asFloat();
                m_EffectEvents[animIndex][keyFrameIndex].push_back(tempDat);
            }
        }
    }
}

HRESULT Client::CMovableObject::Load_MeshEffects(LEVEL ePropLevel, LEVEL eCreateLevel, const wstring& strLayerTag, _float* pTimeScale, const wstring& fileRoot)
{
    filesystem::path path(fileRoot);
    filesystem::directory_iterator iter(path);

    while(iter != filesystem::end(iter))
    {
        if (iter->is_regular_file() && iter->path().extension() == ".MeshFX")
        {
            CMeshEffect::INIT_DESC desc;
            desc.eLevel = ePropLevel;
            desc.FXPrefabPath = iter->path().wstring();
            desc.vInitPos = Vector3(0.f, 0.f, 0.f);

            shared_ptr<CMeshEffect> meshEffect = CMeshEffect::Create(m_pDevice, m_pContext, desc);
            meshEffect->SetOwner(shared_from_this());
            m_pGameInstance.lock()->Add_Object(eCreateLevel, strLayerTag, meshEffect, pTimeScale);
            EventEffects.emplace(iter->path().stem().string(), meshEffect);
        }
        else if (iter->is_directory())
        {
            Load_MeshEffects(ePropLevel, eCreateLevel, strLayerTag, pTimeScale, iter->path().wstring());
        }

        ++iter;
    }

    return S_OK;
}

HRESULT Client::CMovableObject::Load_ParticleEffects(LEVEL ePropLevel, LEVEL eCreateLevel, const wstring& strLayerTag, _float* pTimeScale, const wstring& fileRoot)
{
    filesystem::path path(fileRoot);
    if (path.extension() != ".json")
    {
        MSG_BOX("ERROR : CMovableObject::Load_ParticleEffects");
        return E_FAIL;
    }

    ifstream ifModelParticleData(path, ios::binary);

    if (!ifModelParticleData.fail())
    {
        Json::Value particleData;
        ifModelParticleData >> particleData;

        for (auto particlePrefabName = particleData.begin(); particlePrefabName != particleData.end(); ++particlePrefabName)
        {
            string name = particlePrefabName->asString();
            wstring particlePrefabPath = L"../Bin/EffectPrefab/ParticleEffect/" + CStringUtils::toWstring(name) + L".Particle";
            CParticleEffect::INIT_DESC desc;
            desc.eLevel = eCreateLevel;
            desc.FXPrefabPath = particlePrefabPath;
            desc.vInitPos = Vector3(0.f, 0.f, 0.f);
            shared_ptr<CParticleEffect> pParticle = CParticleEffect::Create(m_pDevice, m_pContext, desc);
            pParticle->SetOwner(shared_from_this());
            if (nullptr == pParticle)
            {
                MSG_BOX("ERROR : CMovableObject::Load_ParticleEffects");
                return E_FAIL;
            }
            m_pGameInstance.lock()->Add_Object(eCreateLevel, strLayerTag, pParticle, pTimeScale);
            EventEffects.emplace(name, pParticle);
        }
    }

    ifModelParticleData.close();

    return S_OK;
}

void Client::CMovableObject::Change_Anim(_uint animIndex)
{
    m_bLastAnimFrameCalled = false;
    m_dCurrentAnimFrame = 0.0;
    m_pMeshCom->Set_Animation(animIndex);
}

HRESULT Client::CMovableObject::Add_Component(LEVEL ePropLevel, LEVEL eCreateLevel, const wstring& strModelKey)
{
    // Navigation
    m_pNavigationCom = CNavigation::Create(m_pDevice, m_pContext, eCreateLevel,m_pTransformCom->Get_Pos());
    if (nullptr == m_pNavigationCom)
        return E_FAIL;

    // Mesh Com
    auto pModelProp = m_pGameInstance.lock()->Get_AnimModel(ePropLevel, strModelKey);
    m_pBufferCom = m_pMeshCom = CSkeletalMeshCom::Create(m_pDevice, m_pContext, pModelProp);

    if (nullptr == m_pMeshCom)
        return E_FAIL;

    if (FAILED(Add_CamRef()))
        return E_FAIL;

    shared_ptr<Json::Value> pEventData = m_pGameInstance.lock()->Get_AnimEventData(strModelKey);
    if(pEventData != nullptr)
        ReadAnimEventDataFromJson(*pEventData, pModelProp);

    shared_ptr<Json::Value> pEffectEventData = m_pGameInstance.lock()->Get_EffectEventData(strModelKey);
    if (pEffectEventData != nullptr)
        ReadEffectEventDataFromJson(*pEffectEventData, pModelProp);

    // 쉐이더 세팅
    if (FAILED(m_pMeshCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxAnimMesh"))))
        return E_FAIL;

    return S_OK;
}

HRESULT Client::CMovableObject::Add_CamRef()
{
    m_pCamRef = CCameraReference::Create(m_pDevice, m_pContext,m_pMeshCom->Get_Bone("Bip001"), Vector3(0.f,-0.8f,0.f));
    
    Add_Children(L"CamRef", m_pCamRef);

    return S_OK;
}

void Client::CMovableObject::PlaySoundOnce(const wstring& strSoundKey, _float fVolume)
{
    auto Pair = m_mapSoundPlayInfo.find(strSoundKey);
    if (Pair == m_mapSoundPlayInfo.end())
        m_mapSoundPlayInfo.emplace(strSoundKey, CHANNEL_END);

    if (m_mapSoundPlayInfo[strSoundKey] == CHANNEL_END)
    {
        m_mapSoundPlayInfo[strSoundKey] = m_pGameInstance.lock()->Play_Sound(strSoundKey, fVolume);
    }
    else
    {
        if (m_pGameInstance.lock()->isPlaying(m_mapSoundPlayInfo[strSoundKey]))
            return;
        else
            m_mapSoundPlayInfo[strSoundKey] = m_pGameInstance.lock()->Play_Sound(strSoundKey, fVolume);
    }
}

void Client::CMovableObject::resetSoundInfo()
{
    m_mapSoundPlayInfo.clear();
}
