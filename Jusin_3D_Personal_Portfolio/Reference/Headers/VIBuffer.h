#pragma once
#include "Property.h"

namespace Engine
{
	class ENGINE_DLL CVIBuffer abstract : public CProperty
	{
	public:
		CVIBuffer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CVIBuffer() = default;

	public:
		virtual HRESULT Initialize() override;
		virtual HRESULT Render();

	public:
		virtual HRESULT Bind_Buffers();
		_bool CheckRayPick(const Ray& ray, _float& fDist);

	protected:
		HRESULT Create_Buffer(ID3D11Buffer** ppBuffer);

	protected:
		ComPtr<ID3D11Buffer>	m_pVB = nullptr;
		ComPtr<ID3D11Buffer>	m_pIB = nullptr;

		vector<Vector3> m_vecVertexPos;
		vector<_uint> m_vecIndex;

	protected:
		D3D11_BUFFER_DESC			m_BufferDesc = {};
		D3D11_SUBRESOURCE_DATA		m_InitialData = {};
		_uint						m_iVertexStride = 0; // 정점 구조체의 바이트크기
		_uint						m_iNumVertices = 0;  // 정점의 개수
		_uint						m_iIndexStride = 0;  // 인덱스 자료형 바이트크기 2 or 4
		_uint						m_iNumIndices = 0;
		_uint						m_iNumVertexBuffers = 0; // 정점버퍼의 개수(인스턴싱)
		DXGI_FORMAT					m_eIndexFormat = {};	 // 인덱스 자료형
		D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitiveTopology = {}; // 그리기 형식
	};
}
