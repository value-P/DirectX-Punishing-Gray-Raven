#pragma once
#include "GameObject.h"

namespace Engine
{
    class CPawn;
    // 폰에서 파생된 클래스를 빙의(possess)하여 그 동작을 제어할 수 있는, 눈에 보이지는 않는 게임오브젝트(액터)
    // PlayerController : 사람이 조종하는 폰에 사용
    // AIController : 조종하는 폰에 대한 인공지능을 구현하는 데 사용
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

