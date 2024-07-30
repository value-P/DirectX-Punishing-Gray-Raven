#include "pch.h"
#include "SampleUI.h"
#include "Tool_Terrain.h"
#include "Tool_MapMesh.h"
#include "Tool_Player.h"
#include "SkeletalMeshCom.h"
#include "EffectTool.h"
#include "ToolMain.h"

Tool::CSampleUI::CSampleUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImguiBase(pDevice, pContext)
{
}

HRESULT Tool::CSampleUI::Initialize(shared_ptr<CToolMain> pMainTool)
{
    // 창 열것인가
    //m_bOpen = true;

    //// 창 옵션 세팅
    ////m_eWindowFlag |= ImGuiWindowFlags_NoTitleBar;
    //m_eWindowFlag |= ImGuiWindowFlags_MenuBar;
    m_eWindowFlag |= ImGuiWindowFlags_NoMove;
    ////m_eWindowFlag |= ImGuiWindowFlags_NoResize;
    //m_eWindowFlag |= ImGuiWindowFlags_NoCollapse;

    if (nullptr == pMainTool)
        return E_FAIL;

    m_pToolMain = pMainTool;

    auto nonAnimKeys = m_pGameInstance.lock()->Get_PropertyKeyList(LEVEL_GAMEPLAY, PropertyType::NonAnimModel);
    auto AnimKeys = m_pGameInstance.lock()->Get_PropertyKeyList(LEVEL_GAMEPLAY, PropertyType::AnimModel);

    szMapKeys.resize(nonAnimKeys->size());
    szAnimModelKeys.resize(AnimKeys->size());

    for (size_t i = 0; i < nonAnimKeys->size(); i++)
        szMapKeys[i] = (*nonAnimKeys)[i].c_str();

    for (size_t i = 0; i < AnimKeys->size(); i++)
        szAnimModelKeys[i] = (*AnimKeys)[i].c_str();

    NavMeshProp = CNavMeshProp::Create(m_pDevice, m_pContext);
    m_pCheckingNavMesh = CNavMeshProp::Create(m_pDevice, m_pContext);

    return S_OK;
}

void Tool::CSampleUI::Tick()
{
    vCamPosition = m_pGameInstance.lock()->Get_CamPos();

    Key_Input();

    ImGui::Begin("Hello, world!", 0, m_eWindowFlag);

    if (ImGui::CollapsingHeader("Terrain"))
    {
        if (nullptr == m_pTerrain)
        {
            ImGui::InputFloat3("trans", m_vInitPos);
            ImGui::InputInt2("terrainSize", m_Size);

            ImGui::InputText("terrainKey", m_strPropKey, IM_ARRAYSIZE(m_strPropKey));

            if (ImGui::Button("Create Terrain"))
            {
                wstring key(m_strPropKey, &m_strPropKey[strlen(m_strPropKey)]);

                m_pTerrain = CTool_Terrain::Create(m_pDevice, m_pContext, key, m_Size[0], m_Size[1], Vector3(0.f,0.f,0.f));

                m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, L"Layer_Terrain", m_pTerrain, &g_TimeScale);
            }
        }
    }

    if (ImGui::CollapsingHeader("MapMesh"))
    {
        ImGui::InputFloat3("trans", m_vInitPos);
        ImGui::InputFloat3("scale", m_scales);

        ImGui::Combo("Map", &iPickedMap, szMapKeys.data(), (_int)szMapKeys.size(), 8);

        if (ImGui::Button("Create MapMesh"))
        {
            size_t len = strlen(szMapKeys[iPickedMap]);
            wstring key(&szMapKeys[iPickedMap][0], &szMapKeys[iPickedMap][len]);
            Vector3 initPos = { m_vInitPos[0], m_vInitPos[1], m_vInitPos[2] };
            m_pMapMesh = CTool_MapMesh::Create(m_pDevice, m_pContext,LEVEL_GAMEPLAY, key, initPos);
            m_pMapMesh->Get_Transform()->Set_Scaling(m_scales[0], m_scales[1], m_scales[2]);

            if (nullptr == m_pMapMesh)
            {
                MSG_BOX("존재하지 않는 model키값입니다");
            }
            else
                m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, L"LAYER_MAP", m_pMapMesh, &g_TimeScale);
        }
    }

    if (ImGui::CollapsingHeader("ModelMesh"))
    {
        ImGui::BulletText("Picking Pos");
        ImGui::Text("X : %.2f Y : %.2f Z : %.2f", m_vPickedPos.x, m_vPickedPos.y, m_vPickedPos.z);

        ImGui::BulletText("Camera Pos");
        ImGui::Text("X : %.2f Y : %.2f Z : %.2f", vCamPosition.x, vCamPosition.y, vCamPosition.z);
        
        ImGui::Text("");
        ImGui::Combo("AnimModel", &iPickedAnimModel, szAnimModelKeys.data(), (_int)szAnimModelKeys.size(), 8);

        if (ImGui::Button("Create Model"))
        {
            size_t len = strlen(szAnimModelKeys[iPickedAnimModel]);
            wstring key(&szAnimModelKeys[iPickedAnimModel][0], &szAnimModelKeys[iPickedAnimModel][len]);
            pickedModelName = szAnimModelKeys[iPickedAnimModel];

            m_pModelMesh = CTool_Player::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY, key, m_vPickedPos);
            m_pModelMesh->Get_Transform()->Rotation(Vector4(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
            m_pModelMesh->Get_Transform()->Rotation(Vector4(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
            m_pModelMesh->Get_Transform()->Go_Up(-0.8f);

            if (nullptr == m_pModelMesh)
            {
                MSG_BOX("존재하지 않는 model키값입니다");
            }
            else
                m_pGameInstance.lock()->Add_Object(LEVEL_GAMEPLAY, L"LAYER_MODEL", m_pModelMesh, &g_TimeScale);

            auto ModelCom = m_pModelMesh->Get_MeshCom();

            for (_int i = 0; i < ModelCom->Get_AnimCount(); ++i)
            {
                map<_int, vector<const _char*>> eventMap;
                map<_int, vector<string>> eventMapStrVer;
                EventDatas.emplace(ModelCom->Get_AnimName(i), eventMap);

                map<_int, vector<FX_EVENT_DATA>> effectEventMap;
                m_pToolMain.lock()->m_pEffectTool->EventDatas.emplace(ModelCom->Get_AnimName(i), effectEventMap);
            }

            string path = "../Bin/AnimEventData/" + pickedModelName + ".json";
            ifstream ifAnimEvent(path, ios::binary);
            if (!ifAnimEvent.fail())
            {
                ifAnimEvent >> AnimEvents;

                for (auto Animation = AnimEvents.begin(); Animation != AnimEvents.end(); ++Animation)
                {
                    string strAnim = Animation.key().asString();

                    for (auto KeyFrame = Animation->begin(); KeyFrame != Animation->end(); ++KeyFrame)
                    {
                        if (KeyFrame->empty())
                            continue;

                        _int keyFrameNum = KeyFrame.key().asInt();
                        auto iter = EventDatas[strAnim].find(keyFrameNum);
                        if (iter == EventDatas[strAnim].end())
                        {
                            vector<const _char*> vecSz;
                            EventDatas[strAnim].emplace(keyFrameNum, vecSz);
                        }

                        for (auto Event = KeyFrame->begin(); Event != KeyFrame->end(); ++Event)
                        {
                            originStr.push_back(Event->asString());
                            EventDatas[strAnim][keyFrameNum].push_back(originStr.back().c_str());
                        }
                    }
                }
            }
            ifAnimEvent.close();

            m_pToolMain.lock()->m_pEffectTool->InitEffectEventData(pickedModelName);
        }

    }

    if (ImGui::CollapsingHeader("AnimSet"))
    {
        if (m_pModelMesh != nullptr)
        {
            ImGui::Checkbox("AnimationFrameWindow", &AnimationMode);
            _int index = m_pModelMesh->Get_CurrentAnimIndex();
            ImGui::BulletText("%d", index);
        }

        ImGui::InputInt("AnimIndex", &iAnimIndex);            

        if (ImGui::Button("ChangeAnim"))
        {
            if(nullptr != m_pModelMesh)
                m_pModelMesh->Change_Anim(iAnimIndex);
        }
    }

    if(ImGui::Button("Clear Object"))
    {
        m_pGameInstance.lock()->Clear_Layer(LEVEL_GAMEPLAY, L"LAYER_MODEL");
        m_pGameInstance.lock()->Clear_Layer(LEVEL_GAMEPLAY, L"LAYER_MAP");
        m_pMapMesh.reset();
        m_pModelMesh.reset();
        AnimEvents.clear();
        EventDatas.clear();
        originStr.clear();
        m_pToolMain.lock()->m_pEffectTool->ClearEffectEventData();
    }

    if (ImGui::CollapsingHeader("NavMeshMaker"))
    {
        ImGui::Checkbox("Activate NavMeshMakingMode", &m_bActivateNavmeshMaking);

        if (m_bActivateNavmeshMaking)
        {
            if (m_bPointsFull)
            {
                sortPointsClockWise();

                ImGui::BulletText("PointA : %.2f %.2f %.2f", vPoints[0].x, vPoints[0].y, vPoints[0].z);
                ImGui::BulletText("PointB : %.2f %.2f %.2f", vPoints[1].x, vPoints[1].y, vPoints[1].z);
                ImGui::BulletText("PointC : %.2f %.2f %.2f", vPoints[2].x, vPoints[2].y, vPoints[2].z);

                if (m_pCheckingNavMesh->Get_CellSize() == 0)
                    m_pCheckingNavMesh->Add_Cell(CCell::Create(m_pDevice, m_pContext, vPoints[0], vPoints[1], vPoints[2], 0));

                if(ImGui::Button("Apply Cell") || m_pGameInstance.lock()->GetKeyDown(DIK_F1))
                {
                    _int index = NavMeshProp->Get_CellSize();
                    NavMeshProp->Add_Cell(CCell::Create(m_pDevice, m_pContext, vPoints[0], vPoints[1], vPoints[2], index));
                    m_pCheckingNavMesh->Reset();
                    m_iPointsCount = 0;
                    m_bPointsFull = false;
                }
            }

            if (m_pGameInstance.lock()->GetKeyDown(DIK_ESCAPE))
            {
                if (m_iPointsCount > 0)
                {
                    if (m_bPointsFull) m_bPointsFull = false;
                    vPoints[m_iPointsCount - 1] = Vector3(0.f, 0.f, 0.f);
                    m_iPointsCount--;
                }
            }

            ImGui::InputText("ReadNonAnimSaveFilePath", navMeshSavePath, MAX_PATH);
            if (ImGui::Button("Parsing"))
            {
                string path(navMeshSavePath);
                ParseNavMeshData(path);
            }

            if (ImGui::Button("HardReset"))
                NavMeshProp->Reset();
        }
    }

    ImGui::End();

    if (AnimationMode)
    {
        AnimationKeyFrameWindow();
        AnimationEventWindow();
    }
}

void Tool::CSampleUI::Render()
{
#ifdef _DEBUG
    if (m_bPointsFull && m_pCheckingNavMesh != nullptr)
    {
        m_pCheckingNavMesh->Render();
    }

    if (NavMeshProp != nullptr)
        NavMeshProp->Render();
#endif // _DEBUG
}

void Tool::CSampleUI::Key_Input()
{
    if (m_pGameInstance.lock()->GetMouseButtonDown(DIM_RB))
    {
        Ray ray = m_pGameInstance.lock()->RayAtWorldSpace();
        _float dist = 0.f;
        if (m_pGameInstance.lock()->RayPicking(Tool::LEVEL_GAMEPLAY, ray, dist))
        {
            m_vPickedPos = ray.position + ray.direction * dist;
        }

        if (m_bActivateNavmeshMaking)
        {
            m_vPickedPos = NavMeshProp->Get_NearestPoint(m_vPickedPos);
            if (!m_bPointsFull)
            {
                vPoints[m_iPointsCount] = m_vPickedPos;
                m_iPointsCount++;
                if (m_iPointsCount > 2)
                    m_bPointsFull = true;
            }
        }
    }
}

void Tool::CSampleUI::sortPointsClockWise()
{
    Vector3 vAB = vPoints[1] - vPoints[0];
    Vector3 vAC = vPoints[2] - vPoints[0];
    Vector3 vCross = vAB.Cross(vAC);

    if (vCross.y < 0.f)
    {
        Vector3 vTemp = vPoints[1];
        vPoints[1] = vPoints[2];
        vPoints[2] = vTemp;
    }
}

_bool Tool::CSampleUI::ParseNavMeshData(const string& savePath)
{
    ofstream ofNavMesh(savePath, ios::binary);

    if (ofNavMesh.fail())
    {
        MSG_BOX("저장할 경로가 잘못되었습니다");
        return false;
    }

    for (auto& cell : NavMeshProp->Get_Cells())
    {
        Vector3 vPoint = cell->Get_Point(CCell::POINT_A);
        ofNavMesh.write((char*)&vPoint, sizeof(Vector3));
        vPoint = cell->Get_Point(CCell::POINT_B);
        ofNavMesh.write((char*)&vPoint, sizeof(Vector3));
        vPoint = cell->Get_Point(CCell::POINT_C);
        ofNavMesh.write((char*)&vPoint, sizeof(Vector3));
    }

    ofNavMesh.close();

    MSG_BOX("파싱성공");
    return true;
}

void Tool::CSampleUI::AnimationKeyFrameWindow()
{
    auto meshCom = m_pModelMesh->Get_MeshCom();

    if(!m_pModelMesh->IsStopped())
        iCurrentKeyFrame = meshCom->Get_CurrentKeyFrame();

    ImGui::Begin("AnimKeyFrame", 0, m_eWindowFlag);

    _int maxKeyFrame = meshCom->Get_MaxKeyFrame();
    if (ImGui::SliderInt("keyFrame", &iCurrentKeyFrame, 0, meshCom->Get_MaxKeyFrame()-1))
    {
        m_pModelMesh->Set_CurrentKeyFrame(iCurrentKeyFrame);
    }

    ImGui::Text("");

    if (ImGui::SliderFloat("AnimSpeed", &fAnimSpeed, 0.f, 1.f))
        m_pModelMesh->Set_AnimSpeed(fAnimSpeed);
    
    ImGui::Text("");

    if (ImGui::Button("Stop"))
        m_pModelMesh->StopAnim();
    
    ImGui::SameLine();

    if (ImGui::Button("Resume"))
        m_pModelMesh->ResumeAnim();

    ImGui::SameLine();
    ImGui::Text("                                 ");
    ImGui::SameLine();
    
    if (ImGui::Button("SaveEventData"))
    {
        string path = "../Bin/AnimEventData/" + pickedModelName + ".json";
        ofstream ofEventData(path,ios::binary);
        if (ofEventData.fail())
        {
            MSG_BOX("SaveEventData : 존재하지 않는 경로입니다");
            return;
        }

        AnimEvents.clear();

        for (auto& Pair1 : EventDatas)
        {
            for (auto& Pair2 : Pair1.second)
            {
                Json::Value vector;
                for (auto& szStr : Pair2.second)
                {
                    string str(szStr);
                    AnimEvents[Pair1.first][Pair2.first].append(str);
                }
            }
        }

        ofEventData << AnimEvents;

        MSG_BOX("저장 완료");

        ofEventData.close();
    }

    ImGui::End();
}

void Tool::CSampleUI::AnimationEventWindow()
{
    auto meshCom = m_pModelMesh->Get_MeshCom();
    string curAnimName = meshCom->Get_AnimName();
    _int iCurrentKeyFrame = meshCom->Get_CurrentKeyFrame();

    ImGui::Begin("AnimEvent");

    ImGui::Text(curAnimName.c_str());
    ImGui::SameLine();
    ImGui::Text("%d",meshCom->Get_CurrentKeyFrame());

    ImGui::Combo("EventFunctions", &functionsIndex, EventFunctions, IM_ARRAYSIZE(EventFunctions));

    if (ImGui::Button("Add_Event"))
    {
        auto Pair = EventDatas[curAnimName].find(iCurrentKeyFrame);
        if (Pair == EventDatas[curAnimName].end())
        {
            vector<const _char*> vecStr;
            EventDatas[curAnimName].emplace(iCurrentKeyFrame, vecStr);
        }

        EventDatas[curAnimName][iCurrentKeyFrame].push_back(EventFunctions[functionsIndex]);
    }

    if (ImGui::Button("Erase_Data"))
    {
        if (EventDatas[curAnimName][iCurrentKeyFrame].size() > iListIndex)
        {
            EventDatas[curAnimName][iCurrentKeyFrame].erase(EventDatas[curAnimName][iCurrentKeyFrame].begin() + iListIndex);
        }
    }

    ImGui::ListBox("KeyFrameEvents", &iListIndex, EventDatas[curAnimName][iCurrentKeyFrame].data(), EventDatas[curAnimName][iCurrentKeyFrame].size());

    ImGui::End();
}

shared_ptr<CSampleUI> Tool::CSampleUI::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CToolMain> pMainTool)
{
    shared_ptr<CSampleUI> pInstance = make_shared<CSampleUI>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pMainTool)))
    {
        MSG_BOX("Failed to Created : CSampleUI");
        pInstance.reset();
    }

    return pInstance;
}

