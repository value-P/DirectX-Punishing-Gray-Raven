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
            // Ż�⶧ ��ó���� ��ư�� ���� �� �ֱ� ������ �ѹ� ȣ��
            SetStackAndFriend(iStartPointer, iCheckPointer - 1);

            break;
        }

        // ������ 3���� �����鼭 üŷ����Ʈ�� ���� �����Ҷ�
        if (m_tSlots[iStartPointer].button->Get_Color() == m_tSlots[iCheckPointer].button->Get_Color() &&
            iStack < 3)
        {
            // �������� + üŷ������ ����
            iStack++;
            iCheckPointer++;
        }
        // ������ 3�̰ų� üŷ����Ʈ�� ���� �������� �ʴٸ�
        else
        {
            // ���ݱ��� üŷ�� ��ư���� ģ���� ��Ͻ�Ű�� ��� ������ �����Ѵ�
            SetStackAndFriend(iStartPointer, iCheckPointer - 1);

            // ���� üŷ����Ʈ�� StartPointer�� �̵�, üŷ����Ʈ�� �� �������� �̵���Ű�� ������ �ʱ�ȭ
            iStack = 1;
            iStartPointer = iCheckPointer;
            iCheckPointer = iStartPointer + 1;
        }
    }
}

void Client::CSkillBtnContainer::ReAllocateSlot()
{
    // ��Ȱ��ȭ �� ���Ե� ����
    for (auto& slot : m_tSlots)
    {
        if (slot.button != nullptr)
        {
            if (false == slot.button->isActivate())
                slot.button.reset();
        }
    }

    // ������ ������ �˻��ؼ� ä������ �ʿ䰡 �����Ƿ�  12 - 1 ������
    for (_int iPointerIndex = 0; iPointerIndex < 12 - 1; ++iPointerIndex)
    {
        // �� �ڸ� �߽߰�
        if (nullptr == m_tSlots[iPointerIndex].button)
        {
            // �ڽ��� ���� �ε������� ��ȸ�Ͽ� ���� ����� ��ų���� ������
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
        // ���� ��ġ���� �ʱ�ȭ
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