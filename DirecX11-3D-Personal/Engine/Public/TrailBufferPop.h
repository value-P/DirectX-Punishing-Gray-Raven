#pragma once
#include "VIBuffer.h"

namespace Engine
{
    class ENGINE_DLL CTrailBufferPop : public CVIBuffer
    {
        struct TrailBase
        {
            Vector3 vUppderPos;
            Vector3 vDownPos;
        };

    public:
        CTrailBufferPop(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CTrailBufferPop() = default;

    public:
        HRESULT Initialize(_uint iNumTrailRect);
        void Tick(const Vector3& vPosUpperWorld,const Vector3& vPosDownWorld);

    protected:
        _uint m_iNumTrailRect = 0; // 트레일에 사용할 사각형의 개수
        list<TrailBase> m_TrailVertexPositions;

    public:
        static shared_ptr<CTrailBufferPop> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumTrailRect);
    };
}