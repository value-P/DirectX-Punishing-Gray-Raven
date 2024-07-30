#include "pch.h"
#include "CameraReference.h"
#include "Bone.h"

Client::CCameraReference::CCameraReference(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice, pContext)
{
}

HRESULT Client::CCameraReference::Initialize(shared_ptr<CBone> socketBone, Vector3 vInitPos)
{
    if (FAILED(__super::Initialize(vInitPos)))
        return E_FAIL;

    m_SocketBone = socketBone;

    return S_OK;
}

void Client::CCameraReference::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_WorldMatrix = m_pTransformCom->Get_WorldMatrix() * m_SocketBone.lock()->Get_CombinedTransformationMatrix() * m_pParentTransform.lock()->Get_WorldMatrix();
}

shared_ptr<CCameraReference> Client::CCameraReference::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CBone> socketBone, const Vector3& vInitPos)
{
    shared_ptr<CCameraReference> pInstance = make_shared<CCameraReference>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(socketBone, vInitPos)))
    {
        MSG_BOX("Failed to Created : CCameraReference");
        pInstance.reset();
    }

    return pInstance;
}
