#include "Bone.h"

Engine::CBone::CBone()
{
}

Engine::CBone::CBone(const CBone& rhs)
	:m_iParentBoneIndex(rhs.m_iParentBoneIndex),
	m_TransformationMatrix(rhs.m_TransformationMatrix),
	m_CombinedTransformationMatrix(rhs.m_CombinedTransformationMatrix),
	m_bIsRootBone(rhs.m_bIsRootBone)
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT Engine::CBone::Initialize(const wstring& strFilePath, Matrix pivotMatrix, _uint& outFilePointer)
{
	ifstream istream(strFilePath, ios::binary);
	if (istream.fail()) 
		return E_FAIL;

	istream.seekg(outFilePointer);

	// 1. 문자열 수
	_uint numString = 0;
	istream.read((char*)&numString, sizeof(_uint));

	// 2. 본 이름
	_char* pName = new _char[numString];
	ZeroMemory(pName, sizeof(_char) * numString);
	istream.read(pName, sizeof(_char) * numString);

	strcpy_s(m_szName, pName);

	Safe_Delete_Array(pName);

	// 루트본 체크
	if (!strcmp(m_szName, "Bip001"))
		m_bIsRootBone = true;

	// 3. 부모 인덱스
	istream.read((_char*)&m_iParentBoneIndex, sizeof(_int));

	// 4. 내 TransformationMatrix
	istream.read((_char*)& m_TransformationMatrix, sizeof(Matrix));
	m_InitialTransformationMatrix = m_TransformationMatrix;

	// 파싱단계에서 전치해놓기때문에 그냥 받아옴
	if (-1 != m_iParentBoneIndex)
		m_TransformationMatrix = m_TransformationMatrix;
	else
		m_TransformationMatrix = m_TransformationMatrix *pivotMatrix;

	m_CombinedTransformationMatrix = XMMatrixIdentity();

	outFilePointer = static_cast<_uint>(istream.tellg());

	istream.close();

	return S_OK;
}

void Engine::CBone::Invalidate_CombinedTransformationMatrix(const vector<shared_ptr<CBone>>& Bones)
{
	if (-1 == m_iParentBoneIndex)
		m_CombinedTransformationMatrix = m_TransformationMatrix;
	else
		m_CombinedTransformationMatrix = m_TransformationMatrix * Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix;
}

shared_ptr<CBone> Engine::CBone::Create(const wstring& strFilePath, Matrix pivotMatrix, _uint& outFilePointer)
{
	shared_ptr<CBone> pInstance = make_shared<CBone>();

	if (FAILED(pInstance->Initialize(strFilePath, pivotMatrix, outFilePointer)))
	{
		MSG_BOX("Failed to Created : CBone");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CBone> Engine::CBone::Clone()
{
	return make_shared<CBone>(*this);
}
