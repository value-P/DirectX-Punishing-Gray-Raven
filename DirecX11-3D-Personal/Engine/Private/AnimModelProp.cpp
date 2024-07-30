#include "AnimModelProp.h"
#include "Bone.h"
#include "Animation.h"
#include "Mesh.h"

Engine::CAnimModelProp::CAnimModelProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CNonAnimModelProp(pDevice, pContext)
{
}

Engine::CAnimModelProp::CAnimModelProp(const CAnimModelProp& rhs)
	:CNonAnimModelProp(rhs),
	m_iNumBones(rhs.m_iNumBones),
	m_iNumAnimations(rhs.m_iNumAnimations),
	m_iCurrentAnimation(rhs.m_iCurrentAnimation),
	m_iNextAnimation(rhs.m_iNextAnimation)
{
	m_Animations.reserve(rhs.m_Animations.size());
	for (auto& pProtoAnimation : rhs.m_Animations)
		m_Animations.push_back(pProtoAnimation->Clone());
	
	m_Bones.reserve(rhs.m_Bones.size());
	for (auto& pProtoBone : rhs.m_Bones)
		m_Bones.push_back(pProtoBone->Clone());
}

_int Engine::CAnimModelProp::Get_BoneIndex(const char* pBoneName) const
{
	_int iBoneIndex = -1; // 부모가 없는 경우 -1
	
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<CBone> pBone) {
		iBoneIndex++;
		return !strcmp(pBoneName, pBone->Get_BoneName());
		});

	if (iter == m_Bones.end()) return -1;

	return iBoneIndex;
}

_int Engine::CAnimModelProp::Get_AnimIndex(const string& strAnimName)
{
	_int iIndex = -1;

	for (size_t i = 0;i < m_Animations.size(); ++i)
	{
		if (!strcmp(m_Animations[i]->Get_Name(), strAnimName.c_str()))
		{
			iIndex = i;
			break;
		}
	}

	return iIndex;
}

const _char* Engine::CAnimModelProp::Get_AnimName(_uint iIndex)
{
	return m_Animations[iIndex]->Get_Name();
}

_int Engine::CAnimModelProp::Get_MaxKeyFrame(_int iIndex)
{
	return m_Animations[iIndex]->Get_MaxKeyFrame();
}

_int Engine::CAnimModelProp::Get_CurrentKeyFrame(_int iIndex)
{
	return m_Animations[iIndex]->Get_CurrentKeyFrame();
}

shared_ptr<CBone> Engine::CAnimModelProp::Get_Bone(const _char* pBoneName)
{
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<CBone> pBone)->bool{
		return !strcmp(pBoneName, pBone->Get_BoneName());
	});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter);
}

void Engine::CAnimModelProp::Set_KeyFrame(_int iKeyFrame)
{
	m_Animations[m_iCurrentAnimation]->Set_KeyFrame(iKeyFrame, m_Bones);

	for (auto& pBone : m_Bones)
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
}

void Engine::CAnimModelProp::Set_Animataion(_uint iAnimIndex)
{
	m_iNextAnimation = iAnimIndex;
	m_bAnimChangeFlag = true;
}

void Engine::CAnimModelProp::Set_AnimationNext()
{
	_uint nextIndex = m_iCurrentAnimation + 1;
	if (nextIndex >= m_iNumAnimations)
		nextIndex = 0;

	Set_Animataion(nextIndex);
}

void Engine::CAnimModelProp::Set_AnimationPrev()
{
	_uint nextIndex;
	if (m_iCurrentAnimation == 0)
		nextIndex = m_iNumAnimations - 1;
	else
		nextIndex = m_iCurrentAnimation - 1;

	Set_Animataion(nextIndex);
}

vector<Matrix> Engine::CAnimModelProp::Get_BoneMatrices(_uint iMeshIndex)
{
	vector<Matrix> BoneMatrices;
	BoneMatrices.resize(512, XMMatrixIdentity());

	m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_Bones);

	return BoneMatrices;
}

HRESULT Engine::CAnimModelProp::Initialize(const wstring& strFilePath, Matrix pivotMatrix)
{
	m_matPivot = pivotMatrix;

	ifstream istream(strFilePath, ios::binary);
	if (istream.fail()) return E_FAIL;

	istream.read((_char*)&m_iNumBones, sizeof(_uint));
	istream.read((_char*)&m_iNumMeshes, sizeof(_uint));
	istream.read((_char*)&m_iNumMaterials, sizeof(_uint));
	istream.read((_char*)&m_iNumAnimations, sizeof(_uint));

	m_Meshes.reserve(m_iNumMeshes);
	m_Materials.reserve(m_iNumMaterials);
	m_Bones.reserve(m_iNumBones);
	m_Animations.reserve(m_iNumAnimations);

	_uint FilePointer = (_uint)istream.tellg();

	if (FAILED(Init_Bones(strFilePath, FilePointer)))
	{
		istream.close();
		return E_FAIL;
	}

	if (FAILED(Init_Mesh(strFilePath, FilePointer)))
	{
		istream.close();
		return E_FAIL;
	}

	if (FAILED(Init_Material(strFilePath, FilePointer)))
	{
		istream.close();
		return E_FAIL;
	}

	if (FAILED(Init_Animation(strFilePath, FilePointer)))
	{
		istream.close();
		return E_FAIL;
	}

	istream.close();

	return S_OK;
}

void Engine::CAnimModelProp::Play_Animation(_float fTimeDelta, _bool isLoop, Vector3& outMoveVelocity, _bool& outLastFrameCalled, _double& outFrameRate, _bool enableRootMotion, _bool useLinearChange)
{
	// 현재 애니메이션 상태에 맞게 뼈들의 TransformationMatrix행렬 갱신
	if(false == m_bAnimChangeFlag)
		m_Animations[m_iCurrentAnimation]->Inivalidate_TransformationMatrix(fTimeDelta, m_Bones, isLoop, outMoveVelocity, outLastFrameCalled, outFrameRate, enableRootMotion);
	else
	{
		outLastFrameCalled = false;

		outFrameRate = 0.0;

		if (useLinearChange)
		{
			//선형보간 코드
			_bool result = m_Animations[m_iCurrentAnimation]->Invalidate_TransformationMatrix_Lerp(fTimeDelta,
				m_Animations[m_iNextAnimation]->Get_Channels(), m_Bones, outMoveVelocity);

			if (true == result)
			{
				m_Animations[m_iCurrentAnimation]->Reset();
				m_bAnimChangeFlag = false;
				m_iCurrentAnimation = m_iNextAnimation;
			}

		}
		else
		{
			// 선형보간 끈 코드
			outMoveVelocity = Vector3(0.f, 0.f, 0.f);
			m_Animations[m_iCurrentAnimation]->Reset();
			m_bAnimChangeFlag = false;
			m_iCurrentAnimation = m_iNextAnimation;
		}
	}

	// 모든 뼈들의 CombinedTransformationMatrix구해줌
	for (auto& pBone : m_Bones)
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
}

HRESULT Engine::CAnimModelProp::Init_Mesh(const wstring& strFilePath, _uint& outFilePointer)
{
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		shared_ptr<CMesh> pMesh = CMesh::Create(m_pDevice, m_pContext, MeshType::ANIM, strFilePath, m_matPivot, outFilePointer);
		if (pMesh == nullptr)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT Engine::CAnimModelProp::Init_Bones(const wstring& strFilePath, _uint& outFilePointer)
{
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		shared_ptr<CBone> pBone = CBone::Create(strFilePath, m_matPivot, outFilePointer);
		if (pBone == nullptr)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}

 	return S_OK;
}

HRESULT Engine::CAnimModelProp::Init_Animation(const wstring& strFilePath, _uint& outFilePointer)
{
	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		shared_ptr<CAnimation> pAnim = CAnimation::Create(strFilePath, static_pointer_cast<CAnimModelProp>(shared_from_this()), outFilePointer);
		if (pAnim == nullptr)
			return E_FAIL;

		m_Animations.push_back(pAnim);
	}

	return S_OK;
}


shared_ptr<CAnimModelProp> Engine::CAnimModelProp::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strFilePath, Matrix pivotMatrix)
{
	shared_ptr<CAnimModelProp> pInstance = make_shared<CAnimModelProp>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strFilePath, pivotMatrix)))
	{
		MSG_BOX("Failed to Created : CAnimModelProp");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CAnimModelProp> Engine::CAnimModelProp::Clone()
{
	return make_shared<CAnimModelProp>(*this);
}
