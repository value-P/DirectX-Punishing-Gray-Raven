#pragma once
#include "GameObject.h"

namespace Engine
{
    class CPawn;
    // ������ �Ļ��� Ŭ������ ����(possess)�Ͽ� �� ������ ������ �� �ִ�, ���� �������� �ʴ� ���ӿ�����Ʈ(����)
    // PlayerController : ����� �����ϴ� ���� ���
    // AIController : �����ϴ� ���� ���� �ΰ������� �����ϴ� �� ���
    class ENGINE_DLL CController abstract : public CGameObject
    {
    public:
        CController() = default;
        CController(const CController& rhs) = default;
        virtual ~CController() = default;

    public:
        _bool    IsPossessed() { return m_pPawn.lock() != nullptr; }

    public:
        HRESULT Possess(shared_ptr<CPawn> pGameObject);
        void UnPossess();

    protected:
        weak_ptr<CPawn> m_pPawn;
    };
}

