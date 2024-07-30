#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CCellBufferProp;

	class ENGINE_DLL CCell
	{
	public:
		enum POINTS { POINT_A, POINT_B, POINT_C, POINT_END };
		enum LINES {LINE_AB, LINE_BC, LINE_CA, LINE_END};
	public:
		CCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CCell() = default;

	public:
		Vector3 Get_Point(POINTS ePoint) { return m_vPoints[ePoint]; }
		_uint Get_Index() { return m_iIndex; }
		void  Set_Neighbor(LINES eLine, shared_ptr<CCell> pNeighbor) {
			m_iNeighborIndices[eLine] = pNeighbor->m_iIndex; }

	public:
		HRESULT Initialize(const Vector3& vPoint0, const Vector3& vPoint1, const Vector3& vPoint2, _uint iCellIndex);

		_bool Compare_Points(const Vector3& vSrcPoint, const Vector3& vDstPoint);
		_bool isIn(Vector3 vLocalPos, _int& iNeighborIndex);

#ifdef _DEBUG
	public:
		HRESULT Render();
#endif

	private:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

	private:
		Vector3					m_vPoints[POINT_END];
		_uint					m_iIndex = 0;
		Vector3					m_vNormals[LINE_END];

		_int					m_iNeighborIndices[LINE_END] = { -1, -1, -1 };

#ifdef _DEBUG
	private:
		shared_ptr<CCellBufferProp> m_pBufferProp= nullptr;
#endif

	public:
		static shared_ptr<CCell> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const Vector3& vPoint0, const Vector3& vPoint1, const Vector3& vPoint2, _uint iCellIndex);
	};
}
