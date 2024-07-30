#include "GameObject.h"
#include "GameInstance.h"
#include "Collider.h"

Engine::CGameObject::CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT Engine::CGameObject::Initialize(const Vector3& vInitPos)
{
	m_pTransformCom = CTransform::Create();
	m_pTransformCom->Set_Pos(vInitPos);

	return S_OK;
}

void Engine::CGameObject::Priority_Tick(_float fTimeDelta)
{
	for (auto& pChild : m_mapChildrens)
		pChild.second->Priority_Tick(fTimeDelta);
}

_int Engine::CGameObject::Tick(_float fTimeDelta)
{
	for (auto& pChild : m_mapChildrens)
		pChild.second->Tick(fTimeDelta);

	return OBJ_ALIVE;
}

void Engine::CGameObject::Late_Tick(_float fTimeDelta)
{
	for (auto& pChild : m_mapChildrens)
		pChild.second->Late_Tick(fTimeDelta);
}

HRESULT Engine::CGameObject::Render()
{
	for (auto& pChild : m_mapChildrens)
	{
		if(FAILED(pChild.second->Render()))
			return E_FAIL;
	}

	return S_OK;
}

void Engine::CGameObject::OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
}

void Engine::CGameObject::OnCollisionStay(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
}

void Engine::CGameObject::OnCollisionExit(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
}

_bool Engine::CGameObject::RayPicking(const Ray& ray, _float& fOutDist)
{
	if (nullptr == m_pBufferCom) return false;

	Ray localRay;
	Matrix matWorldInverse = XMMatrixInverse(nullptr, m_pTransformCom->Get_WorldMatrix());

	localRay.position = XMVector3TransformCoord(ray.position, matWorldInverse);
	localRay.direction = XMVector3TransformNormal(ray.direction, matWorldInverse);
	localRay.direction.Normalize();

	return m_pBufferCom->Check_RayPicked(localRay, fOutDist);
}

HRESULT Engine::CGameObject::Add_Children(const wstring& strChildTag, shared_ptr<CGameObject> pChildObject)
{
	auto iter = m_mapChildrens.find(strChildTag);
	if (iter != m_mapChildrens.end()) return E_FAIL;

	pChildObject->Set_ParentTransform(m_pTransformCom);
	m_mapChildrens.emplace(strChildTag, pChildObject);

	return S_OK;
}

shared_ptr<CGameObject> Engine::CGameObject::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const Vector3& vInitPos)
{
	shared_ptr<CGameObject> pInstance = make_shared<CGameObject>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vInitPos)))
	{
		MSG_BOX("Failed to Created : CGameObject");
		pInstance.reset();
	}

	return pInstance;
}
