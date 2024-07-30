#include "NonAnimModelProp.h"
#include "Material.h"
#include "Mesh.h"

Engine::CNonAnimModelProp::CNonAnimModelProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CProperty(pDevice, pContext)
{
}

Engine::CNonAnimModelProp::CNonAnimModelProp(const CNonAnimModelProp& rhs)
	:CProperty(rhs.m_pDevice, rhs.m_pContext),
	m_matPivot(rhs.m_matPivot),
	m_iNumMeshes(rhs.m_iNumMeshes),
	m_Meshes(rhs.m_Meshes),
	m_iNumMaterials(rhs.m_iNumMaterials),
	m_Materials(rhs.m_Materials)
{
}

ComPtr<ID3D11ShaderResourceView> Engine::CNonAnimModelProp::Get_MaterialSRV(_uint iMeshIndex, MaterialTexType eMaterialType) const
{
	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	return m_Materials[iMaterialIndex]->Get_MatrialSRV(eMaterialType);
}

HRESULT Engine::CNonAnimModelProp::Initialize(const wstring& strFilePath, Matrix pivotMatrix)
{
	m_matPivot = pivotMatrix;

	ifstream istream(strFilePath, ios::binary);
	if (istream.fail()) return E_FAIL;

	istream.read((char*)&m_iNumMeshes, sizeof(_uint));
	istream.read((char*)&m_iNumMaterials, sizeof(_uint));

	m_Meshes.reserve(m_iNumMeshes);
	m_Materials.reserve(m_iNumMaterials);

	_uint FilePointer = (_uint)istream.tellg();

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

	istream.close();

	return S_OK;
}

HRESULT Engine::CNonAnimModelProp::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

_bool Engine::CNonAnimModelProp::Check_RayPicked(const Ray& ray, _float& fOutDist)
{
	_bool ObjPicked = false;
	_float minDist = 1000.f;
	// 모델의 메쉬들을 순회하고 피킹 성공시 가장 가까운 성공지점 반환
	for (auto& mesh : m_Meshes)
	{
		_float result;
		if (mesh->CheckRayPick(ray, result))
		{
			if (!ObjPicked) ObjPicked = true;

			if (minDist > result)
				minDist = result;
		}
	}

	if (ObjPicked)
	{
		fOutDist = minDist;
		return true;
	}

	return false;
}

HRESULT Engine::CNonAnimModelProp::Init_Mesh(const wstring& strFilePath, _uint& outFilePointer)
{
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		shared_ptr<CMesh> pMesh = CMesh::Create(m_pDevice, m_pContext, MeshType::NON_ANIM, strFilePath, m_matPivot, outFilePointer);
		if (pMesh == nullptr)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT Engine::CNonAnimModelProp::Init_Material(const wstring& strFilePath, _uint& outFilePointer)
{
	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		shared_ptr<CMaterial> pMaterial = CMaterial::Create(m_pDevice, m_pContext, strFilePath, outFilePointer);
		if (pMaterial == nullptr)
			return E_FAIL;

		m_Materials.push_back(pMaterial);
	}

	return S_OK;
}

shared_ptr<CNonAnimModelProp> Engine::CNonAnimModelProp::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strFilePath, Matrix pivotMatrix)
{
	shared_ptr<CNonAnimModelProp> pInstance = make_shared<CNonAnimModelProp>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strFilePath, pivotMatrix)))
	{
		MSG_BOX("Failed to Created : CNonAnimModelProp");
		pInstance.reset();
	}

	return pInstance;
}
