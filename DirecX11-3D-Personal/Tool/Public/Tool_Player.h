#pragma once
#include "GameObject.h"

namespace Tool
{
    class CTool_Player : public CGameObject
    {
	public:
		CTool_Player(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CTool_Player() = default;

	public:
		_uint Get_CurrentAnimIndex();
		shared_ptr<CSkeletalMeshCom> Get_MeshCom() { return m_pMeshCom; }

	public:
		virtual HRESULT Initialize(LEVEL eLevel, const wstring& strModelKey, Vector3 vInitPos);
		virtual void Priority_Tick(_float fTimeDelta);
		virtual _int Tick(_float fTimeDelta);
		virtual void Late_Tick(_float fTimeDelta);
		virtual HRESULT Render();

	public:
		_bool IsStopped() { return m_bStopAnim; }
		void Change_Anim(_uint animIndex);
		void StopAnim() { m_bStopAnim = true; }
		void ResumeAnim() { m_bStopAnim = false; }
		void Set_AnimSpeed(_float fAnimSpeed) { m_AnimSpeed = fAnimSpeed; }
		void Set_CurrentKeyFrame(_int iKeyFrame);

	private:
		void Key_Input();

	private:
		HRESULT Add_Component(LEVEL eLevel, const wstring& strModelKey);
		HRESULT Bind_ShaderResources();

	private:
		shared_ptr<CSkeletalMeshCom> m_pMeshCom = nullptr;
		_bool m_bStopAnim = false;
		_float m_AnimSpeed = 1.f;

	public:
		static shared_ptr<CTool_Player> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eLevel, const wstring& strModelKey, Vector3 vInitPos);
	};
}


