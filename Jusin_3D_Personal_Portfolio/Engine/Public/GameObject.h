#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CComponent;
	class CGameInstance;
	class CTransform;
	class CPrimitiveComponent;
	class CCollider;

	class ENGINE_DLL CGameObject : public enable_shared_from_this<CGameObject>
	{
	public:
		CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CGameObject() = default;

	public:
		shared_ptr<CTransform> Get_Transform() { return m_pTransformCom; }
		
	protected:
		void Set_ParentTransform(shared_ptr<CTransform> pParentTransform) { m_pParentTransform = pParentTransform; }

	public:
		virtual HRESULT Initialize(const Vector3& vInitPos);
		virtual void Priority_Tick(_float fTimeDelta);
		virtual _int Tick(_float fTimeDelta);
		virtual void Late_Tick(_float fTimeDelta);
		virtual HRESULT Render();
		virtual HRESULT Render_Shadow() { return S_OK; }
		virtual HRESULT Render_Distortion() { return S_OK; }
		virtual HRESULT Render_ExceptDark() { return S_OK; }

	public:
		virtual void OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider);
		virtual void OnCollisionStay(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider);
		virtual void OnCollisionExit(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider);

	public:
		_bool RayPicking(const Ray& ray, _float& fOutDist);

	protected:
		HRESULT Add_Children(const wstring& strChildTag, shared_ptr<CGameObject> pChildObject);

	protected:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

	protected:
		weak_ptr<CGameInstance> m_pGameInstance;
		shared_ptr<CTransform> m_pTransformCom = nullptr;
		shared_ptr<CPrimitiveComponent> m_pBufferCom = nullptr;
		
		weak_ptr<CTransform> m_pParentTransform;

		map<const wstring, shared_ptr<CGameObject>> m_mapChildrens;

	public:
		shared_ptr<CGameObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const Vector3& vInitPos = Vector3(0.f, 0.f, 0.f));
	};
}
