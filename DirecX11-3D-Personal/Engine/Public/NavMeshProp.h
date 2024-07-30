#pragma once
#include "Property.h"

namespace Engine
{
	class CCell;
	class CGameObject;

    class ENGINE_DLL CNavMeshProp : public CProperty
    {
	public:
		CNavMeshProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CNavMeshProp() = default;

	public:
		_bool isHaveAttachedObject() { return m_pAttachedObject.lock() != nullptr; }
		HRESULT Setting_AttachedObject(shared_ptr<CGameObject> pObject);
		Matrix Get_AttachedWorldMatrix() { return m_AttachedWorldMatrix; }
		HRESULT Add_Cell(shared_ptr<CCell> pCell);
		_uint Get_CellSize() { return m_Cells.size(); }
		Vector3 Get_NearestPoint(const Vector3 vInput);
		void Reset() { m_Cells.clear(); }
		const vector<shared_ptr<CCell>>& Get_Cells() { return m_Cells; }

	public:
		HRESULT Initialize(const wstring& strFilePath);
		HRESULT Initialize();
		void Tick();
		
		// 네비게이션을 부착한 오브젝트가 navMeshCell들 중 갈수 있는곳에 있는지 체크해주는 함수(movable오브젝트용 함수)
		_bool isMove(const Vector3& vPosition, _int& iCurrentIndex);
		_int  Get_IndexFromPos(const Vector3& vPosition);

#ifdef _DEBUG
	public:
		HRESULT Render();
#endif

	private:
		HRESULT Make_Neighbors();

	private:
		weak_ptr<CGameObject>		m_pAttachedObject;
		static Matrix m_AttachedWorldMatrix;

		_int  m_iCurrentIndex = { -1 };
		_uint m_iNumCells = 0;
		vector<shared_ptr<CCell>>	m_Cells;

#ifdef _DEBUG
	private:
		shared_ptr<class CShader> m_pShader = { nullptr };
#endif


	public:
		static shared_ptr<CNavMeshProp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strFilePath);
		static shared_ptr<CNavMeshProp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	};
}


