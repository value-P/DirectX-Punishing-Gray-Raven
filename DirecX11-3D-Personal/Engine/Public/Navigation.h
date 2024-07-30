#pragma once
#include "Component.h"

namespace Engine
{
	class CCell;
	class CShader;
	class CNavMeshProp;

    class ENGINE_DLL CNavigation final: public CComponent
    {
	public:
		CNavigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CNavigation() = default;

	public:
		HRESULT Initialize(_int iStartIndex);
		HRESULT Initialize(_uint iLevelIndex, const Vector3& vStartPos);
		_bool isMove(const Vector3& vPosition);
		_float Get_HeightOnNav(const Vector3& vPosition);

	private:
		_int m_iCurrentIndex = -1;

	public:
		static shared_ptr<CNavigation> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _int iStartIndex);
		static shared_ptr<CNavigation> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,_uint iLevelIndex, const Vector3& vStartPos);
    };
}


