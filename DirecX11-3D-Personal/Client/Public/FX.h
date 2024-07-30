#pragma once
#include "GameObject.h"

namespace Client
{
    class CFX abstract : public CGameObject
    {
    public:
        CFX(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CFX() = default;

    public:
        _bool isPlaying() { return m_bActivate; }
        void SetOwner(shared_ptr<CGameObject> pOwner) { m_pOwner = pOwner; }

    public:
        virtual void PlayOnce();
        virtual void PlayLoop();
        virtual void Stop();

    protected:
        weak_ptr<CGameObject> m_pOwner;
        _bool m_bActivate = false;

    };
}