#include "pch.h"
#include "SkillBtnContainer.h"
#include "SkillBtn.h"

Client::CSkillBtnContainer::CSkillBtnContainer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CUIBase(pDevice, pContext)
{
}

void Client::CSkillBtnContainer::CreateSkillBall()
{
    auto pBtn = find_if(m_pSkillBalls.begin(), m_pSkillBalls.end(), [](shared_ptr<CSkillBtn> button)->bool {return !button->isActivate(); });
    if (pBtn == m_pSkillBalls.end())
        return;

    Vector2 vDestPos;
    _int iSlotIndex = 0;
    for (; iSlotIndex < sizeof(m_tSlots) / sizeof(m_tSlots[0]); ++iSlotIndex)
    {
        if (nullptr == m_tSlots[iSlotIndex].button)
        {
            vDestPos = m_tSlots[iSlotIndex].vPos;
            break;
        }
    }

    _int iRandColor = m_pGameInstance.lock()->Rand_Num(0, CSkillBtn::COLOR_END - 1);
    (*pBtn)->Activate(static_cast<CSkillBtn::SKILLCOLOR>(iRandColor), m_vEndPos, vDestPos);
    m_tSlots[iSlotIndex].button = (*pBtn);

    CheckSkillChain();
}

void Client::CSkillBtnContainer::CheckSkillChain()
{
    ReAllocateSlot();

    _int iStartPointer = 0;
    _int iCheckPointer = 1;
    _int iStack = 1;
    while (true)
    {
        if (nullptr == m_tSlots[iCheckPointer].button ||
            iCheckPointer >= 12)
        {
            // 탈출때 미처리된 버튼이 있을 수 있기 때문에 한번 호출
            SetStackAndFriend(iStartPointer, iCheckPointer - 1);

            break;
        }

        // 스택이 3보다 작으면서 체킹포인트의 색이 동일할때
        if (m_tSlots[iStartPointer].button->Get_Color() == m_tSlots[iCheckPointer].button->Get_Color() &&
            iStack < 3)
        {
            // 스택증가 + 체킹포인터 증가
            iStack++;
            iCheckPointer++;
        }
        // 스택이 3이거나 체킹포인트의 색이 동일하지 않다면
        else
        {
            // 지금까지 체킹된 버튼끼리 친구를 등록시키고 모두 스택을 변경한다
            SetStackAndFriend(iStartPointer, iCheckPointer - 1);

            // 현재 체킹포인트로 StartPointer를 이동, 체킹포인트를 그 다음으로 이동시키고 스택을 초기화
            iStack = 1;
            iStartPointer = iCheckPointer;
            iCheckPointer = iStartPointer + 1;
        }
    }
}

void Client::CSkillBtnContainer::ReAllocateSlot()
{
    // 비활성화 된 슬롯들 제거
    for (auto& slot : m_tSlots)
    {
        if (slot.button != nullptr)
        {
            if (false == slot.button->isActivate())
                slot.button.reset();
        }
    }

    // 마지막 슬롯은 검사해서 채워넣을 필요가 없으므로  12 - 1 까지만
    for (_int iPointerIndex = 0; iPointerIndex < 12 - 1; ++iPointerIndex)
    {
        // 빈 자리 발견시
        if (nullptr == m_tSlots[iPointerIndex].button)
        {
            // 자신의 다음 인덱스부터 순회하여 가장 가까운 스킬볼을 가져옴
            for (_int iFindPointer = iPointerIndex; iFindPointer < 12; ++iFindPointer)
            {                
                if (nullptr != m_tSlots[iFindPointer].button)
                {
                    m_tSlots[iPointerIndex].button = m_tSlots[iFindPointer].button;
                    m_tSlots[iFindPointer].button.reset();
                    Vector2 vDestPos = m_tSlots[iPointerIndex].vPos;
                    m_tSlots[iPointerIndex].button->Set_DestPos(vDestPos);
                    break;
                }
            }
        }
    }
}

void Client::CSkillBtnContainer::SetStackAndFriend(_int iStartPointer, _int iCheckEndPointer)
{
    _int iCount = iCheckEndPointer - iStartPointer + 1;

    if (iCount == 2)
    {
        m_tSlots[iStartPointer].button->Set_SkillStack(2);
        m_tSlots[iStartPointer].button->Add_FriendBtn(m_tSlots[iCheckEndPointer].button);

        m_tSlots[iCheckEndPointer].button->Set_SkillStack(2);
        m_tSlots[iCheckEndPointer].button->Add_FriendBtn(m_tSlots[iStartPointer].button);
    }
    else if (iCount == 3)
    {
        _int iMiddlePointer = iStartPointer + 1;

        m_tSlots[iStartPointer].button->Set_SkillStack(3);
        m_tSlots[iStartPointer].button->Add_FriendBtn(m_tSlots[iMiddlePointer].button);
        m_tSlots[iStartPointer].button->Add_FriendBtn(m_tSlots[iCheckEndPointer].button);

        m_tSlots[iMiddlePointer].button->Set_SkillStack(3);
        m_tSlots[iMiddlePointer].button->Add_FriendBtn(m_tSlots[iStartPointer].button);
        m_tSlots[iMiddlePointer].button->Add_FriendBtn(m_tSlots[iCheckEndPointer].button);

        m_tSlots[iCheckEndPointer].button->Set_SkillStack(3);
        m_tSlots[iCheckEndPointer].button->Add_FriendBtn(m_tSlots[iStartPointer].button);
        m_tSlots[iCheckEndPointer].button->Add_FriendBtn(m_tSlots[iMiddlePointer].button);
    }
}

HRESULT Client::CSkillBtnContainer::Initialize(const INIT_DESC& initDesc, shared_ptr<CPlayableVera> pPlayer)
{
    m_vPosition = initDesc.vPos;
    m_pPlayer = pPlayer;
    _float zeroX = m_vPosition.x + 300.f;
    m_vEndPos = Vector2(m_vPosition.x - 400.f, m_vPosition.y);
    for (_int iSlotIndex = 0; iSlotIndex < 12; ++iSlotIndex)
    {
        // 슬롯 위치정보 초기화
        m_tSlots[iSlotIndex].vPos = Vector2(zeroX, m_vPosition.y);
        zeroX -= 50.f;

        CUIBase::INIT_DESC BtninitDesc;
        BtninitDesc.vPos = m_vEndPos;
        BtninitDesc.vSize = Vector2(50.f, 50.f);
        shared_ptr<CSkillBtn> pBtn = CSkillBtn::Create(m_pDevice, m_pContext, BtninitDesc, static_pointer_cast<CSkillBtnContainer>(shared_from_this()));
        pBtn->Set_Player(m_pPlayer.lock());
        pBtn->InActivate();
        m_pSkillBalls.push_back(pBtn);

        m_pGameInstance.lock()->Add_UI(initDesc.eCreateLevel,pBtn);
    }
                                                                                                                                                                                                                                                                                                                                                                                                     
    return S_OK;
}

shared_ptr<CSkillBtnContainer> Client::CSkillBtnContainer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc, shared_ptr<CPlayableVera> pPlayer)
{
    shared_ptr<CSkillBtnContainer> pInstance = make_shared<CSkillBtnContainer>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(initDesc, pPlayer)))
    {
        MSG_BOX("Failed to Created : CSkillBtnContainer");
        pInstance.reset();
    }

    return pInstance;
}