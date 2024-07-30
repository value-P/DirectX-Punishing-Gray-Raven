#pragma once
#include "FX.h"

namespace Client
{
    class CTextureFX : public CFX
    {
    public:
        struct INIT_DESC
        {

        };

    public:
        CTextureFX(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CTextureFX() = default;



    public:
        static shared_ptr<CTextureFX> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);
    };
}


