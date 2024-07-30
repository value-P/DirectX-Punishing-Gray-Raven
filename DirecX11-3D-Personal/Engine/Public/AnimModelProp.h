#pragma once
#include "NonAnimModelProp.h"

namespace Engine
{
	class CShader;
	class CAnimation;
	class CBone;

    class ENGINE_DLL CAnimModelProp final : public CNonAnimModelProp
    {
	public:
		CAnimModelProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		CAnimModelProp(const CAnimModelProp& rhs);
		virtual ~CAnimModelProp() = default;

	public:
		_int Get_BoneIndex(const char* pBoneName) const;
		_int Get_AnimCount() { return m_iNumAnimations; }
		_int Get_CurrentAnimIndex() { return m_iCurrentAnimation; }
		_int Get_AnimIndex(const string& strAnimName);
		const _char* Get_AnimName(_uint iIndex);
		_int Get_MaxKeyFrame(_int iIndex);
		_int Get_CurrentKeyFrame(_int iIndex);
		shared_ptr<CBone> Get_Bone(const _char* pBoneName);

		void Set_KeyFrame(_int iKeyFrame);
		void Set_Animataion(_uint iAnimIndex);
		void Set_AnimationNext();
		void Set_AnimationPrev();

		vector<Matrix> Get_BoneMatrices(_uint iMeshIndex);

	public:
		virtual HRESULT Initialize(const wstring& strFilePath, Matrix pivotMatrix) override;

		void Play_Animation(_float fTimeDelta, _bool isLoop, Vector3& outMoveVelocity, _bool& outLastFrameCalled, _double& outFrameRate, _bool enableRootMotion, _bool useLinearChange);

	private:
		virtual HRESULT Init_Mesh(const wstring& strFilePath, _uint& outFilePointer) override;
		HRESULT Init_Bones(const wstring& strFilePath, _uint& outFilePointer);
		HRESULT Init_Animation(const wstring& strFilePath, _uint& outFilePointer);

	private:
		// 본
		_uint		m_iNumBones = 0;
		vector<shared_ptr<CBone>> m_Bones;

		// 애니메이션
		_uint		m_iNumAnimations = 0;
		_uint		m_iCurrentAnimation = 0;
		_uint		m_iNextAnimation = 0;
		_bool		m_bAnimChangeFlag = false;
		vector<shared_ptr<CAnimation>> m_Animations;

	public:
		static shared_ptr<CAnimModelProp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strFilePath, Matrix pivotMatrix = XMMatrixIdentity());
		shared_ptr<CAnimModelProp> Clone();
	};
}