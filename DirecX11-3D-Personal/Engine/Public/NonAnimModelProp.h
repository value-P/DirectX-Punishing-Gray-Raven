#pragma once
#include "Property.h"

namespace Engine
{
	class CShader;
	class CMesh;
	class CMaterial;

    class ENGINE_DLL CNonAnimModelProp :public CProperty
    {
	public:
		CNonAnimModelProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		CNonAnimModelProp(const CNonAnimModelProp& rhs);
		virtual ~CNonAnimModelProp() = default;

	public:
		_uint Get_NumMeshes() const { return m_iNumMeshes; }

		ComPtr<ID3D11ShaderResourceView> Get_MaterialSRV(_uint iMeshIndex, MaterialTexType eMaterialType) const;
		
	public:
		virtual HRESULT Initialize(const wstring& strFilePath, Matrix pivotMatrix);
		virtual HRESULT Render(_uint iMeshIndex);

		_bool Check_RayPicked(const Ray& ray, _float& fOutDist);

	protected:
		virtual HRESULT Init_Mesh(const wstring& strFilePath, _uint& outFilePointer);
		HRESULT Init_Material(const wstring& strFilePath, _uint& outFilePointer);
		
	protected:
		Matrix		m_matPivot;

	protected:
		// 메시
		_uint		m_iNumMeshes = 0;
		vector<shared_ptr<CMesh>> m_Meshes;

		// 머티리얼
		_uint		m_iNumMaterials = 0;
		vector<shared_ptr<CMaterial>> m_Materials;
				
	public:
		static shared_ptr<CNonAnimModelProp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strFilePath, Matrix pivotMatrix = XMMatrixIdentity());
    };
}


