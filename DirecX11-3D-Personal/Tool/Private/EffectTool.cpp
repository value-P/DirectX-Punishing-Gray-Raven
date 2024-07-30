#include "pch.h"
#include "EffectTool.h"
#include "ToolMain.h"
#include "Texture.h"
#include "MeshFX.h"
#include "SampleUI.h"
#include "Tool_Player.h"

Tool::CEffectTool::CEffectTool(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImguiBase(pDevice, pContext)
{
}

HRESULT Tool::CEffectTool::Initialize(shared_ptr<CToolMain> pMainTool)
{
    m_MeshEffectPrefabs.reserve(1000);
    m_szMeshEffectPrefabs.reserve(1000);
    m_ParticlePrefabs.reserve(1000);
    m_szParticlePrefabs.reserve(1000);

    if (pMainTool == nullptr)
        return E_FAIL;

    // 창 옵션 세팅
    //m_eWindowFlag |= ImGuiWindowFlags_NoTitleBar;
    //m_eWindowFlag |= ImGuiWindowFlags_MenuBar;
    m_eWindowFlag |= ImGuiWindowFlags_NoMove;
    //m_eWindowFlag |= ImGuiWindowFlags_NoResize;
    //m_eWindowFlag |= ImGuiWindowFlags_NoCollapse;

    m_pToolMain = pMainTool;
    m_pSampleUI = m_pToolMain.lock()->m_pSampleUI;

    auto texKeys = m_pGameInstance.lock()->Get_PropertyKeyList(LEVEL_STATIC, PropertyType::Texture);

    m_pTextureKeyList.reserve(texKeys->size());

    for(size_t i = 0;i < texKeys->size(); ++i)
        m_pTextureKeyList.push_back((*texKeys)[i].c_str());

    auto KeyList = m_pGameInstance.lock()->Get_AnimEffectPropKeyList(LEVEL_GAMEPLAY);

    for (auto& Owner : *KeyList)
    {
        vector<const _char*> vecEffectKey;
        vecEffectKey.reserve(Owner.second.size());

        for (auto& iter : Owner.second)
        {
            vecEffectKey.push_back(iter.c_str());
        }

        m_EffectMeshKeyList.emplace(Owner.first, vecEffectKey);
    }

    Read_ParticlePrefab("../Bin/EffectPrefab/ParticleEffect");
    Read_MeshFXPrefab("../Bin/EffectPrefab/MeshEffect");

    m_pTextureMap = m_pGameInstance.lock()->Get_TextureMap(LEVEL_STATIC);

    return S_OK;
}

void Tool::CEffectTool::Tick()
{
    ImGui::Begin("EffectToolWindow", 0, m_eWindowFlag);

    // 탭
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("ParticleFX"))
        {
            ParticleTab();

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("MeshFX"))
        {
            MeshEffectTab();

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("ParticlePrefab"))
        {
            ParticlePrefabView();

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("MeshFXPrefab"))
        {
            MeshEffectPrefabView();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void Tool::CEffectTool::Render()
{

}

void Tool::CEffectTool::InitEffectEventData(const string& strModelname)
{
    string path = "../Bin/EffectEventData/" + strModelname + ".json";
    ifstream ifAnimEvent(path, ios::binary);
    if (!ifAnimEvent.fail())
    {
        ifAnimEvent >> AnimEvents;

        for (auto Animation = AnimEvents.begin(); Animation != AnimEvents.end(); ++Animation)
        {
            string strAnim = Animation.key().asString();

            // 애니메이션 이름과 매칭되는 인덱스를 키값으로 포함하는지 검색 후 없다면 추가
            auto keyFrameEffectEvents = EventDatas.find(strAnim);
            if (keyFrameEffectEvents == EventDatas.end())
            {
                map<_int, vector<FX_EVENT_DATA>> events;
                EventDatas.emplace(strAnim, events);
            }

            for (auto KeyFrame = Animation->begin(); KeyFrame != Animation->end(); ++KeyFrame)
            {
                if (KeyFrame->empty())
                    continue;

                // 해당 키프레임의 인덱스를 키값으로 하는배열있는지 확인 후 추가
                _int keyFrameIndex = KeyFrame.key().asInt();
                auto eventData = EventDatas[strAnim].find(keyFrameIndex);
                if (eventData == EventDatas[strAnim].end())
                {
                    vector<FX_EVENT_DATA> events;
                    EventDatas[strAnim].emplace(keyFrameIndex, events);
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
                    EventDatas[strAnim][keyFrameIndex].push_back(tempDat);
                }
            }
        }

    }

    ifAnimEvent.close();

    string particlePath = "../Bin/EffectPrefab/ModelParticleList/" + strModelname + ".json";
    ifstream ifModelParticles(particlePath, ios::binary);
    if (!ifModelParticles.fail())
    {
        Json::Value particleData;
        ifModelParticles >> particleData;

        for (auto particlePrefabName = particleData.begin(); particlePrefabName != particleData.end(); ++particlePrefabName)
            m_ModelUseParticlePrefabNames.push_back(particlePrefabName->asString());
    }
}

void Tool::CEffectTool::ClearEffectEventData()
{
    AnimEvents.clear();
    EventDatas.clear();
    m_ModelUseParticlePrefabNames.clear();
}

void Tool::CEffectTool::ParticleTab()
{
    ImGui::ColorEdit4("Color", (_float*)&m_tParicleData.vColor);
    ImGui::DragFloat3("Pivot", (_float*)&m_tParicleData.vPivot, 0.1f, -100.f, 100.f, "%.1f");
    ImGui::DragFloat3("Center", (_float*)&m_tParicleData.vCenter, 0.1f, -100.f, 100.f, "%.1f");
    ImGui::DragFloat3("Range", (_float*)&m_tParicleData.vRange, 0.1f, 0.f, 1000.f, "%.1f");

    ImGui::DragFloatRange2("XSize", &m_tParicleData.vMinSize.x, &m_tParicleData.vMaxSize.x, 0.01f, 0.f, 100.f, "MinX: %.2f", "MaxX: %.2f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::DragFloatRange2("YSize", &m_tParicleData.vMinSize.y, &m_tParicleData.vMaxSize.y, 0.01f, 0.f, 100.f, "MinY: %.2f", "MaxY: %.2f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::DragFloatRange2("Speed", &m_tParicleData.vSpeed.x, &m_tParicleData.vSpeed.y, 0.1f, -200.f, 200.f, "Min: %.1f", "Max: %.1f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::DragFloatRange2("LifeTime", &m_tParicleData.vLifeTime.x, &m_tParicleData.vLifeTime.y, 0.1f, 0.f, 30.f, "Min: %.1f", "Max: %.1f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::DragFloat("Gravity Acceleration", &m_tParicleData.fGravityAcceleration, 0.01f, 0.f, 9.8f, "%.2f");
    ImGui::DragFloat("Terminal SPeed", &m_tParicleData.fTerminalSpeed, 0.01f, 0.f, 10.f, "%.2f");
    ImGui::DragFloat("Drag", &m_tParicleData.fDrag, 0.01f, 0.f, 1.f, "%.2f");

    ImGui::Checkbox("Loop", &m_tParicleData.isLoop);
    ImGui::DragFloat("Duration", &m_tParicleData.fDuration, 0.1f, 0.f, 30.f, "%.1f");
     
    if (m_pParticleObj == nullptr)
    {
        ImGui::PushItemWidth(70);
        ImGui::DragInt("NumParticle", &m_iNumInstance, 1.f, 2, 200);

        if (ImGui::Button("Create Particle"))
        {
            m_pParticleObj = CTool_ParticleObj::Create(m_pDevice, m_pContext, m_iNumInstance, Vector3(0.f, 0.f, 0.f), LEVEL_STATIC, L"Snow", m_tParicleData);
            m_pParticleObj->PlayOnce();
            m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, L"Particle", m_pParticleObj, &g_TimeScale);
        }

        ImGui::PushItemWidth(100);
    }
    else
    {
        if (ImGui::Combo("Particle Move", &m_iCurrentMove, m_ParitcleMove, IM_ARRAYSIZE(m_ParitcleMove)))
        {
            m_pParticleObj->Change_Option((CParticleSystem::PARTICLE_OPTION)m_iCurrentMove);
            m_eParitcleTickOption = static_cast<CParticleSystem::PARTICLE_OPTION>(m_iCurrentMove);
        }

        if(ImGui::Combo("Texture", &m_iCurTex, m_pTextureKeyList.data(), m_pTextureKeyList.size()))
            m_pMyTexture = m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, CStringUtils::toWstring(m_pTextureKeyList[m_iCurTex]));
        
        if (m_pMyTexture != nullptr)
        {
            ImVec2 size = ImVec2(32.0f, 32.0f);                         // Size of the image we want to make visible
            ImVec2 uv_min = ImVec2(0.0f, 0.0f);                         // Top-left
            ImVec2 uv_max = ImVec2(1.0f, 1.0f);                         // Lower-right
            ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
            ImGui::Image(m_pMyTexture->GetSRV(0).Get(), size, uv_min, uv_max);

            ImGui::InputText("ParticleName", m_strParticleName, IM_ARRAYSIZE(m_strParticleName));
        }

        ImGui::Separator();
        if (ImGui::ArrowButton("PlayOnce", ImGuiDir_Right)) 
        {
            m_pParticleObj->Change_InstanceData(m_tParicleData);
            if (m_pMyTexture != nullptr)
                m_pParticleObj->Set_Texture(m_pMyTexture);

            m_pParticleObj->PlayOnce();
        }

        if (strcmp(m_strParticleName, ""))
        {
            if (ImGui::Button("Save Prefab"))
            {
                SavePariclePrefab();
            }
        }

        ImGui::Separator();
        if (ImGui::Button("Erase"))
        {
            m_pGameInstance.lock()->Clear_Layer(LEVEL_GAMEPLAY, L"Particle");
            m_pParticleObj.reset();
        }
    }
}

void Tool::CEffectTool::MeshEffectTab()
{
    for (auto& Pair : m_EffectMeshKeyList)
    {
        string OwnerName = Pair.first;

        if (ImGui::TreeNode(OwnerName.c_str()))
        {
            if (ImGui::ListBox(OwnerName.c_str(), &m_iCurrentEffectIndex, Pair.second.data(), Pair.second.size()))
            {
                m_currentEffectOwner = OwnerName;
                m_bCanCreate = true;
            }

            ImGui::TreePop();
        }
    }

    ImGui::Separator();
    if (m_bCanCreate)
    {
        if (ImGui::Button("Create Mesh Effect"))
        {
            m_bCanCreate = false;

            string EffectName = m_EffectMeshKeyList[m_currentEffectOwner][m_iCurrentEffectIndex];
            m_pEffectMesh = CMeshFX::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY, CStringUtils::toWstring(m_currentEffectOwner), CStringUtils::toWstring(EffectName), Vector3(0.f, 0.f, 0.f));
            m_pEffectMesh->Activate();

            if (nullptr != m_pEffectMesh)
            {
                m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, L"Effect", m_pEffectMesh, &g_TimeScale);
            }
        }
    }

    if (nullptr != m_pEffectMesh)
    {
        ImGui::DragFloat("USpeed", &m_vUVSpeed.x, 0.1f, 0.f, 100.f, "U: %.1f");
        ImGui::DragFloat("VSpeed", &m_vUVSpeed.y, 0.1f, 0.f, 100.f, "V: %.1f");
        ImGui::DragFloat("Duration", &m_fDuration, 0.01f, 0.f, 100.f, "%.2f");
        m_pEffectMesh->Set_UVSpeed(m_vUVSpeed);
        m_pEffectMesh->Set_Duration(m_fDuration);

        ImGui::Checkbox("Use Primary Color", &m_bUsePrimaryColor);

        if (m_bUsePrimaryColor)
        {
            ImGui::ColorEdit4("PrimeColor", (_float*)&m_vPrimColor);

            _int iIndex = 0;
            ImVec2 scrolling_child_size = ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 7 + 30);
            ImGui::BeginChild("scrolling", scrolling_child_size, true, ImGuiWindowFlags_HorizontalScrollbar);

            for (auto& TexturePair : *m_pTextureMap)
            {
                iIndex++;
                ImGui::PushID(iIndex);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.f, 2.f));

                ImVec2 size = ImVec2(32.0f, 32.0f);                         // Size of the image we want to make visible
                ImVec2 uv_min = ImVec2(0.0f, 0.0f);                         // Top-left
                ImVec2 uv_max = ImVec2(1.0f, 1.0f);                         // Lower-right
                ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
                if (ImGui::ImageButton("", TexturePair.second->GetSRV(0).Get(), size, uv_min, uv_max, bg_col))
                {
                    m_strMaskTextureKey = TexturePair.first;
                    m_pEffectMesh->UsePrimeColor(true);
                    m_pEffectMesh->Set_PrimeColor(m_vPrimColor);
                    m_pEffectMesh->Set_MaskTexture(TexturePair.second);
                }

                ImGui::PopStyleVar();
                ImGui::PopID();
                
                if(iIndex % 5 != 4)
                    ImGui::SameLine();
            }

            ImGui::EndChild();
        }
        else
            m_pEffectMesh->UsePrimeColor(false);

        if (ImGui::DragFloat3("Scale", (_float*)&m_vScale, 0.1f, 0.1f, 100.f, "%.1f"))
        {
            m_pEffectMesh->Get_Transform()->Set_Scaling(m_vScale.x, m_vScale.y, m_vScale.z);
        }

        if (ImGui::DragFloat3("Rotation", (_float*)&m_vRotation, 0.1f, -360.f, 360.f, "%.1f"))
        {
            m_pEffectMesh->Get_Transform()->Rotation(m_vRotation);
        }

        ImGui::Separator();
        if (ImGui::Button("Save Prefab"))
        {
            SaveMeshEffectPrefab();
        }

        ImGui::Separator();
        if (ImGui::ArrowButton("PlayOnce", ImGuiDir_Right))
        {
            m_pEffectMesh->Activate();
        }
    }

    ImGui::Separator();
    if (ImGui::Button("Clear"))
    {
        m_pGameInstance.lock()->Clear_Layer(LEVEL_GAMEPLAY, L"Effect");
        m_pEffectMesh.reset();
    }
}

void Tool::CEffectTool::ParticlePrefabView()
{
    if (ImGui::ListBox("Particle Prefab", &m_iCurrentParticlePrefab, m_szParticlePrefabs.data(), m_szParticlePrefabs.size()))
    {
        m_tData.eFxType = FX_EVENT_DATA::FX_PARTICLE;
        m_tData.strPrefabName = m_ParticlePrefabs[m_iCurrentParticlePrefab];
    }

    ImGui::BulletText("Type : Particle");
    ImGui::BulletText(m_tData.strPrefabName.c_str());

    ImGui::DragFloat3("Pivot Pos", (_float*)&m_tData.vPivot, 0.1f, -50.f, 50.f, "%.1f");


    auto Model = m_pSampleUI.lock()->m_pModelMesh;
    if (Model != nullptr)
    {
        AnimEventView();

        auto ModelCom = Model->Get_MeshCom();
        string AnimName = ModelCom->Get_AnimName();
        _int keyFrame = ModelCom->Get_CurrentKeyFrame();

        if (ImGui::Button("Add_Event"))
        {
            auto Pair = EventDatas[AnimName].find(keyFrame);
            if (Pair == EventDatas[AnimName].end())
            {
                vector<FX_EVENT_DATA> vecData;
                EventDatas[AnimName].emplace(keyFrame, vecData);
            }

            EventDatas[AnimName][keyFrame].push_back(m_tData);

            auto prefabName = find_if(m_ModelUseParticlePrefabNames.begin(), m_ModelUseParticlePrefabNames.end(), [&](const string& prefabName)->bool { return m_tData.strPrefabName == prefabName; });
            if (prefabName == m_ModelUseParticlePrefabNames.end())
                m_ModelUseParticlePrefabNames.push_back(m_tData.strPrefabName);
        }
    }
    else
    {
        if (!EventDatas.empty())
            EventDatas.clear();
    }

}

void Tool::CEffectTool::MeshEffectPrefabView()
{
    if (ImGui::ListBox("MeshFX Prefab", &m_iCurrentMeshFXPrefab, m_szMeshEffectPrefabs.data(), m_szMeshEffectPrefabs.size()))
    {
        m_tData.eFxType = FX_EVENT_DATA::FX_MESH;
        m_tData.strPrefabName = m_MeshEffectPrefabs[m_iCurrentMeshFXPrefab];
    }

    ImGui::BulletText("Type : MeshFX");
    ImGui::BulletText(m_tData.strPrefabName.c_str());

    ImGui::DragFloat3("Pivot Pos", (_float*)&m_tData.vPivot, 0.1f, -50.f, 50.f, "%.1f");

    auto Model = m_pSampleUI.lock()->m_pModelMesh;
    if (Model != nullptr)
    {
        AnimEventView();

        auto ModelCom = Model->Get_MeshCom();
        string AnimName = ModelCom->Get_AnimName();
        _int keyFrame = ModelCom->Get_CurrentKeyFrame();

        if (ImGui::Button("Add_Event"))
        {
            auto Pair = EventDatas[AnimName].find(keyFrame);
            if (Pair == EventDatas[AnimName].end())
            {
                vector<FX_EVENT_DATA> vecData;
                EventDatas[AnimName].emplace(keyFrame, vecData);
            }

            EventDatas[AnimName][keyFrame].push_back(m_tData);
        }

    }
    else
    {
        if (!EventDatas.empty())
            EventDatas.clear();
    }
}

void Tool::CEffectTool::AnimEventView()
{
    shared_ptr<CTool_Player> pModelMesh = m_pSampleUI.lock()->m_pModelMesh;
    auto meshCom = pModelMesh->Get_MeshCom();
    string animName = meshCom->Get_AnimName();

    if (!m_pSampleUI.lock()->m_pModelMesh->IsStopped())
        m_iCurrentKeyFrame = meshCom->Get_CurrentKeyFrame();

    ImGui::Begin("AnimKeyFrame", 0, m_eWindowFlag);

    ImGui::Text(animName.c_str());

    _int maxKeyFrame = meshCom->Get_MaxKeyFrame();
    if (ImGui::SliderInt("keyFrame", &m_iCurrentKeyFrame, 0, meshCom->Get_MaxKeyFrame() - 1))
    {
        pModelMesh->Set_CurrentKeyFrame(m_iCurrentKeyFrame);
    }

    if (ImGui::SliderFloat("AnimSpeed", &m_fAnimSpeed, 0.f, 1.f))
    {
        pModelMesh->Set_AnimSpeed(m_fAnimSpeed);

        if (m_pEffectMesh != nullptr) m_pEffectMesh->Set_AnimSpeed(m_fAnimSpeed);
    }

    if (ImGui::BeginListBox("KeyFrameEvents"))
    {
        for (size_t i = 0; i < EventDatas[animName][m_iCurrentKeyFrame].size(); ++i)
        {
            if (EventDatas[animName][m_iCurrentKeyFrame].size() == 0)
                continue;

            FX_EVENT_DATA data = EventDatas[animName][m_iCurrentKeyFrame][i];
            if (ImGui::Selectable(data.strPrefabName.c_str(), true))
                m_iEventIndex = i;

            Vector3 vRight = m_pSampleUI.lock()->m_pModelMesh->Get_Transform()->Get_Right();
            Vector3 vUp = m_pSampleUI.lock()->m_pModelMesh->Get_Transform()->Get_Up();
            Vector3 vLook = m_pSampleUI.lock()->m_pModelMesh->Get_Transform()->Get_Look();
            vRight *= data.vPivot.x;
            vUp *= data.vPivot.y;
            vLook *= data.vPivot.z;
            Vector3 vPivotMove = vRight + vUp + vLook;

            if (data.eFxType == FX_EVENT_DATA::FX_MESH)
            {
                m_meshSamples[data.strPrefabName]->Get_Transform()->Set_Pos(vPivotMove);
                m_meshSamples[data.strPrefabName]->Activate();
            }
            else
            {
                m_particleSamples[data.strPrefabName]->Get_Transform()->Set_Pos(vPivotMove);
                m_particleSamples[data.strPrefabName]->PlayOnce();
            }
        }

        ImGui::EndListBox();
    }

    if (ImGui::Button("Erase_Data"))
    {
        if (EventDatas[animName][m_iCurrentKeyFrame].size() > m_iEventIndex)
        {
            string eventPrefabName = EventDatas[animName][m_iCurrentKeyFrame][m_iEventIndex].strPrefabName;

            EventDatas[animName][m_iCurrentKeyFrame].erase(EventDatas[animName][m_iCurrentKeyFrame].begin() + m_iEventIndex);

            auto prefabName = find_if(m_ModelUseParticlePrefabNames.begin(), m_ModelUseParticlePrefabNames.end(), [&](const string& prefabName)->bool { return m_tData.strPrefabName == prefabName; });
            
            if (prefabName != m_ModelUseParticlePrefabNames.end())
                m_ModelUseParticlePrefabNames.erase(prefabName);
        }
    }

    ImGui::Separator();
    if (ImGui::Button("Save Event Data"))
        Parse_EffectEventData();

    ImGui::End();
}

HRESULT Tool::CEffectTool::SavePariclePrefab()
{
    string particleName(m_strParticleName);
    string prefabPath = "../Bin/EffectPrefab/ParticleEffect/" + particleName + ".Particle";

    ofstream of(prefabPath);

    if (of.fail())
    {
        of.close();
        return E_FAIL;
    }

    of.write((_char*)&m_iNumInstance, sizeof(m_iNumInstance));
    of.write((_char*)&m_tParicleData, sizeof(m_tParicleData));
    of.write((_char*)&m_eParitcleTickOption, sizeof(m_eParitcleTickOption));
    wstring strTexKey = CStringUtils::toWstring(m_pTextureKeyList[m_iCurTex]);
    _int iKeyLen = strTexKey.length(); 
    of.write((_char*)&iKeyLen, sizeof(iKeyLen));
    of.write((_char*)strTexKey.c_str(), sizeof(_tchar) * iKeyLen);

    Add_ParticlePrefabKey(particleName);

    shared_ptr<CTool_ParticleObj> pParticle = CTool_ParticleObj::Create(m_pDevice, m_pContext, m_iNumInstance, Vector3(0.f, 0.f, 0.f), LEVEL_GAMEPLAY, strTexKey, m_tParicleData);
    pParticle->Stop();
    pParticle->Change_Option(m_eParitcleTickOption);
    m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, L"EffectSample", pParticle, &g_TimeScale);

    m_particleSamples.emplace(particleName, pParticle);

    MSG_BOX("Success");
    of.close();

    return S_OK;
}

HRESULT Tool::CEffectTool::SaveMeshEffectPrefab()
{
    string EffectName = m_EffectMeshKeyList[m_currentEffectOwner][m_iCurrentEffectIndex];
    string ownerFile = "../Bin/EffectPrefab/MeshEffect/" + m_currentEffectOwner;

    filesystem::create_directory(ownerFile);

    string prefabPath = ownerFile + "/" + EffectName + ".MeshFX";

    ofstream of(prefabPath);

    if (of.fail())
    {
        of.close();
        return E_FAIL;
    }

    wstring strOwnerName = CStringUtils::toWstring(m_currentEffectOwner);
    _int iKeyLen = strOwnerName.length();
    of.write((_char*)&iKeyLen, sizeof(iKeyLen));
    of.write((_char*)strOwnerName.c_str(), sizeof(_tchar) * iKeyLen);

    wstring strEffectName = CStringUtils::toWstring(EffectName);
    iKeyLen = strEffectName.length();
    of.write((_char*)&iKeyLen, sizeof(iKeyLen));
    of.write((_char*)strEffectName.c_str(), sizeof(_tchar) * iKeyLen);

    of.write((_char*)&m_vUVSpeed, sizeof(m_vUVSpeed));
    of.write((_char*)&m_fDuration, sizeof(m_fDuration));

    of.write((_char*)&m_bUsePrimaryColor, sizeof(m_bUsePrimaryColor));
    iKeyLen = m_strMaskTextureKey.length();
    of.write((_char*)&iKeyLen, sizeof(iKeyLen));
    of.write((_char*)m_strMaskTextureKey.c_str(), sizeof(_tchar) * iKeyLen);
    of.write((_char*)&m_vPrimColor, sizeof(m_vPrimColor));
    of.write((_char*)&m_vScale, sizeof(m_vScale));
    of.write((_char*)&m_vRotation, sizeof(m_vRotation));

    Add_MeshFXPrefabKey(EffectName);

    shared_ptr<CMeshFX> pMeshFX = CMeshFX::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY, strOwnerName, strEffectName, Vector3(0.f, 0.f, 0.f));
    pMeshFX->Set_Duration(m_fDuration);
    pMeshFX->Set_UVSpeed(m_vUVSpeed);
    pMeshFX->UsePrimeColor(m_bUsePrimaryColor);
    if (m_bUsePrimaryColor)
    {
        pMeshFX->Set_MaskTexture(m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, m_strMaskTextureKey));
        pMeshFX->Set_PrimeColor(m_vPrimColor);
    }

    pMeshFX->Get_Transform()->Set_Scaling(m_vScale.x, m_vScale.y, m_vScale.z);
    pMeshFX->Get_Transform()->Rotation(m_vRotation);

    m_meshSamples.emplace(EffectName, pMeshFX);

    m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, L"EffectSample", pMeshFX, &g_TimeScale);

    MSG_BOX("Success");
    of.close();

    return S_OK;
}

void Tool::CEffectTool::Add_ParticlePrefabKey(const string& PrefabName)
{
    auto iter = find_if(m_ParticlePrefabs.begin(), m_ParticlePrefabs.end(), [&PrefabName](const string& name) { return name == PrefabName; });

    if (iter == m_ParticlePrefabs.end())
    {
        m_ParticlePrefabs.push_back(PrefabName);
        size_t iSize = m_ParticlePrefabs.size();
        m_szParticlePrefabs.push_back(m_ParticlePrefabs[iSize - 1].c_str());
    }
}

void Tool::CEffectTool::Add_MeshFXPrefabKey(const string& prefabName)
{
    auto iter = find_if(m_MeshEffectPrefabs.begin(), m_MeshEffectPrefabs.end(), [&prefabName](const string& name) { return name == prefabName; });

    if (iter == m_MeshEffectPrefabs.end())
    {
        m_MeshEffectPrefabs.push_back(prefabName);
        size_t iSize = m_MeshEffectPrefabs.size();
        m_szMeshEffectPrefabs.push_back(m_MeshEffectPrefabs[iSize - 1].c_str());
    }
}

void Tool::CEffectTool::Read_ParticlePrefab(const string& strRootFile)
{
    filesystem::path particlePath(strRootFile);
    filesystem::directory_iterator iter(particlePath);

    while (iter != filesystem::end(iter))
    {
        // 일반 파일 일 경우
        if (iter->is_regular_file() && iter->path().extension() == ".Particle")
        {
            ifstream istream(iter->path().string());

            if (istream.fail())
                continue;

            _int iNumInstnace;
            CPointParticle::INSTANCE_DESC tParicleData;
            CParticleSystem::PARTICLE_OPTION eParitcleTickOption;
            _int iKeyLen;
            wstring strTexKey;
            
            istream.read((_char*)&iNumInstnace, sizeof(iNumInstnace));
            istream.read((_char*)&tParicleData, sizeof(tParicleData));
            istream.read((_char*)&eParitcleTickOption, sizeof(eParitcleTickOption));
            istream.read((_char*)&iKeyLen, sizeof(iKeyLen));

            strTexKey.resize(iKeyLen);
            istream.read((_char*)strTexKey.c_str(), sizeof(_tchar) * iKeyLen);

            m_ParticlePrefabs.push_back(iter->path().stem().string());
            size_t iSize = m_ParticlePrefabs.size();
            m_szParticlePrefabs.push_back(m_ParticlePrefabs[iSize - 1].c_str());

            shared_ptr<CTool_ParticleObj> pParticle = CTool_ParticleObj::Create(m_pDevice, m_pContext, iNumInstnace, Vector3(0.f, 0.f, 0.f), LEVEL_GAMEPLAY, strTexKey, tParicleData);
            pParticle->Stop();
            pParticle->Change_Option(eParitcleTickOption);
            m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, L"EffectSample", pParticle, &g_TimeScale);

            m_particleSamples.emplace(iter->path().stem().string(), pParticle);

            istream.close();
        }
        else if (iter->is_directory())
        {
            Read_ParticlePrefab(iter->path().string());
        }

        ++iter;
    }
}

void Tool::CEffectTool::Read_MeshFXPrefab(const string& strRootFile)
{
    filesystem::path particlePath(strRootFile);
    filesystem::directory_iterator iter(particlePath);

    while (iter != filesystem::end(iter))
    {
        // 일반 파일 일 경우
        if (iter->is_regular_file() && iter->path().extension() == ".MeshFX")
        {
            ifstream istream(iter->path().string());

            if (istream.fail())
                continue;

            _int iStrLen = 0;
            wstring OwnerKey;
            wstring effectName;
            Vector2 uvSpeed;
            _float fDuration;
            _bool usePrimaryColor;
            wstring maskTexKey;
            Vector4 primeColor;
            Vector3 vScale;
            Vector3 vRotation;

            istream.read((_char*)&iStrLen, sizeof(iStrLen));
            OwnerKey.resize(iStrLen);
            istream.read((_char*)OwnerKey.c_str(), sizeof(_tchar) * iStrLen);

            istream.read((_char*)&iStrLen, sizeof(iStrLen));
            effectName.resize(iStrLen);
            istream.read((_char*)effectName.c_str(), sizeof(_tchar) * iStrLen);

            istream.read((_char*)&uvSpeed, sizeof(uvSpeed));
            istream.read((_char*)&fDuration, sizeof(fDuration));

            istream.read((_char*)&usePrimaryColor, sizeof(usePrimaryColor));
            istream.read((_char*)&iStrLen, sizeof(iStrLen));
            maskTexKey.resize(iStrLen);
            istream.read((_char*)maskTexKey.c_str(), sizeof(_tchar) * iStrLen);
            istream.read((_char*)&primeColor, sizeof(primeColor));
            istream.read((_char*)&vScale, sizeof(vScale));
            istream.read((_char*)&vRotation, sizeof(vRotation));

            m_MeshEffectPrefabs.push_back(iter->path().stem().string());
            size_t iSize = m_MeshEffectPrefabs.size();
            m_szMeshEffectPrefabs.push_back(m_MeshEffectPrefabs[iSize - 1].c_str());

            shared_ptr<CMeshFX> pMeshFX = CMeshFX::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY, OwnerKey, effectName, Vector3(0.f,0.f,0.f));
            pMeshFX->Set_Duration(fDuration);
            pMeshFX->Set_UVSpeed(uvSpeed);
            pMeshFX->UsePrimeColor(usePrimaryColor);
            if (usePrimaryColor)
            {
                pMeshFX->Set_MaskTexture(m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, maskTexKey));
                pMeshFX->Set_PrimeColor(primeColor);
            }

            pMeshFX->Get_Transform()->Set_Scaling(vScale.x, vScale.y, vScale.z);
            pMeshFX->Get_Transform()->Rotation(vRotation);

            m_meshSamples.emplace(iter->path().stem().string(), pMeshFX);

            m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, L"EffectSample", pMeshFX, &g_TimeScale);

            istream.close();
        }
        else if (iter->is_directory())
        {
            Read_MeshFXPrefab(iter->path().string());
        }

        ++iter;
    }

}

void Tool::CEffectTool::Parse_EffectEventData()
{
    string path = "../Bin/EffectEventData/" + m_pSampleUI.lock()->pickedModelName + ".json";
    ofstream ofEventData(path, ios::binary);
    if (ofEventData.fail())
    {
        MSG_BOX("EffectEventData : 존재하지 않는 경로입니다");
        return;
    }

    AnimEvents.clear();

    for (auto& Pair1 : EventDatas)
    {
        for (auto& Pair2 : Pair1.second)
        {
            for (auto& eventData : Pair2.second)
            {
                Json::Value Dat;
                Dat["Effect Type"] = eventData.eFxType;
                Dat["Effect PrefabName"] = eventData.strPrefabName;
                Dat["Pivot X"] = eventData.vPivot.x;
                Dat["Pivot Y"] = eventData.vPivot.y;
                Dat["Pivot Z"] = eventData.vPivot.z;
                
                AnimEvents[Pair1.first][Pair2.first].append(Dat);
            }
        }
    }

    ofEventData << AnimEvents;

    // 플레이어가 사용할  파트클들의 이름을 저장
    string ModelPrefabsPath = "../Bin/EffectPrefab/ModelParticleList/" + m_pSampleUI.lock()->pickedModelName + ".json";
    ofstream ofModelParticleData (ModelPrefabsPath, ios::binary);
    if (ofModelParticleData.fail())
    {
        MSG_BOX("ofModelParticleData : 존재하지 않는 경로입니다");
        return;
    }

    Json::Value ModelUseParticleData;
    for (auto& prefabName : m_ModelUseParticlePrefabNames)
    {
        ModelUseParticleData.append(prefabName);
    }

    ofModelParticleData << ModelUseParticleData;

    MSG_BOX("저장 완료");

    ofEventData.close();

}

shared_ptr<CEffectTool> Tool::CEffectTool::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CToolMain> pMainTool)
{
    shared_ptr<CEffectTool> pInstance = make_shared<CEffectTool>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pMainTool)))
    {
        MSG_BOX("Failed to Created : CEffectTool");
        pInstance.reset();
    }

    return pInstance;
}
