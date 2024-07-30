#include "PointParticle.h"
#include "GameInstance.h"

Engine::CPointParticle::CPointParticle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CVIBuffer(pDevice, pContext)
{
}

void Engine::CPointParticle::Start()
{
	m_isFinished = false;
	m_fTimeAcc = 0.f;
}

HRESULT Engine::CPointParticle::Initialize(_uint iNumInstance, const INSTANCE_DESC& initData)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_InstanceData = initData;

	m_vecInstanceSpeed.resize(iNumInstance);
	m_vecInstanceLifeTime.resize(iNumInstance);
	m_vecInstanceOriginLifeTime.resize(iNumInstance);

	for (_uint i = 0; i < iNumInstance; ++i)
	{
		m_vecInstanceSpeed[i] = m_pGameInstance.lock()->Rand_Num(initData.vSpeed.x, initData.vSpeed.y);
		m_vecInstanceLifeTime[i] = m_pGameInstance.lock()->Rand_Num(initData.vLifeTime.x, initData.vLifeTime.y);
	}

	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;

	m_iNumInstance = iNumInstance;
	m_iIndexCountPerInstance = 1;
	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	HRESULT hr;

#pragma region VERTEX_BUFFER
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices[0].vPosition = Vector3(0.f, 0.f, 0.f);
	pVertices[0].vPSize = Vector2(1.f, 1.f);

	m_InitialData.pSysMem = pVertices;
	
	hr = __super::Create_Buffer(m_pVB.GetAddressOf());
	Safe_Delete_Array(pVertices);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	m_InitialData.pSysMem = pIndices;

	hr = __super::Create_Buffer(m_pIB.GetAddressOf());
	Safe_Delete_Array(pIndices);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

	#pragma region INSTANCE_BUFFER
		m_iInstanceStride = sizeof(VTXINSTANCE);
	
		m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iVertexStride;

		VTXINSTANCE* pInstance = new VTXINSTANCE[m_iNumInstance];
		ZeroMemory(pInstance, sizeof(VTXINSTANCE) * m_iNumInstance);
	
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			// 인스턴스 데이터
			_float fSizeX = m_pGameInstance.lock()->Rand_Num(initData.vMinSize.x, initData.vMaxSize.x);
			_float fSizeY = m_pGameInstance.lock()->Rand_Num(initData.vMinSize.y, initData.vMaxSize.y);
			_float widthRange = m_pGameInstance.lock()->Rand_Num(initData.vRange.x * -0.5f, initData.vRange.x * 0.5f);
			_float heightRange = m_pGameInstance.lock()->Rand_Num(initData.vRange.y * -0.5f, initData.vRange.y * 0.5f);
			_float depthRange = m_pGameInstance.lock()->Rand_Num(initData.vRange.z * -0.5f, initData.vRange.z * 0.5f);
	
			pInstance[i].vRight		= Vector4(fSizeX,0.f,0.f,0.f);
			pInstance[i].vUp		= Vector4(0.f, fSizeY,0.f,0.f);
			pInstance[i].vLook		= Vector4(0.f,0.f, 1.f,0.f);
			pInstance[i].vTranslation = Vector4(
				initData.vCenter.x + widthRange,
				initData.vCenter.y + heightRange,
				initData.vCenter.z + depthRange,
				1.f);
	
			pInstance[i].vColor = initData.vColor;
	
			// 속도, 라이프타임
			_float fSpeed = m_pGameInstance.lock()->Rand_Num(initData.vSpeed.x, initData.vSpeed.y);
			_float fLifeTime = m_pGameInstance.lock()->Rand_Num(initData.vLifeTime.x, initData.vLifeTime.y);
			
			m_vecInstanceSpeed[i] = fSpeed;
			m_vecInstanceLifeTime[i] = fLifeTime;
			m_vecInstanceOriginLifeTime[i] = fLifeTime;
		}
	
		m_InitialData.pSysMem = pInstance;
	
		hr = __super::Create_Buffer(m_pVBInstance.GetAddressOf());
		Safe_Delete_Array(pInstance);
	
		if (FAILED(hr))
			return E_FAIL;
	
	#pragma endregion


	return S_OK;
}

HRESULT Engine::CPointParticle::Bind_Buffers()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB.Get(),
		m_pVBInstance.Get()
	};

	_uint iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT Engine::CPointParticle::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void Engine::CPointParticle::Tick_Drop(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= m_InstanceData.fDuration)
		m_bDurationEnd = true;

		//m_isFinished = true;
	D3D11_MAPPED_SUBRESOURCE subResource = {};

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	_bool m_bAliveParticleExist = false;
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE* pVertices = reinterpret_cast<VTXINSTANCE*>(subResource.pData);

		_float durationRate = m_vecInstanceLifeTime[i] / m_vecInstanceOriginLifeTime[i];

		pVertices[i].vTranslation.y -= m_vecInstanceSpeed[i] * fTimeDelta;
		pVertices[i].vColor.w = durationRate;

		m_vecInstanceLifeTime[i] -= fTimeDelta;
		if (m_vecInstanceLifeTime[i] <= 0.f)
		{
			if (m_InstanceData.isLoop && false == m_bDurationEnd)
			{
				_float fSizeX = m_pGameInstance.lock()->Rand_Num(m_InstanceData.vMinSize.x, m_InstanceData.vMaxSize.x);
				_float fSizeY = m_pGameInstance.lock()->Rand_Num(m_InstanceData.vMinSize.y, m_InstanceData.vMaxSize.y);
				_float fWidth = m_pGameInstance.lock()->Rand_Num(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
				_float fHeight = m_pGameInstance.lock()->Rand_Num(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
				_float fDepth = m_pGameInstance.lock()->Rand_Num(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);
				_float fTime = m_pGameInstance.lock()->Rand_Num(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);
				_float fSpeed = m_pGameInstance.lock()->Rand_Num(m_InstanceData.vSpeed.x, m_InstanceData.vSpeed.y);
				m_vecInstanceLifeTime[i] = fTime;
				m_vecInstanceOriginLifeTime[i] = fTime;
				m_vecInstanceSpeed[i] = fSpeed;

				reinterpret_cast<VTXINSTANCE*>(subResource.pData)[i].vRight = Vector4(fSizeX, 0.f, 0.f, 0.f);
				reinterpret_cast<VTXINSTANCE*>(subResource.pData)[i].vUp = Vector4(0.f, fSizeY, 0.f, 0.f);
				reinterpret_cast<VTXINSTANCE*>(subResource.pData)[i].vLook = Vector4(0.f, 0.f, 1.f, 0.f);

				reinterpret_cast<VTXINSTANCE*>(subResource.pData)[i].vTranslation = Vector4(
					m_InstanceData.vCenter.x + fWidth,
					m_InstanceData.vCenter.y + fHeight,
					m_InstanceData.vCenter.z + fDepth,
					1.f);
			}
			else
				reinterpret_cast<VTXINSTANCE*>(subResource.pData)[i].vColor.w = 0.f;
		}
		else
			m_bAliveParticleExist = true;
	}

	if (false == m_bAliveParticleExist && m_bDurationEnd)
	{
		m_isFinished = true;
		m_bDurationEnd = false;
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void Engine::CPointParticle::Tick_Spread(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= m_InstanceData.fDuration)
	{
		m_fTimeAcc = 0.f;
		m_isFinished = true;

		for (auto& lifeTime : m_vecInstanceLifeTime)
			lifeTime = 0.f;

		return;
	}

	D3D11_MAPPED_SUBRESOURCE subResource = {};

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE* pVertices = reinterpret_cast<VTXINSTANCE*>(subResource.pData);

		Vector4 vDir = pVertices[i].vTranslation - m_InstanceData.vPivot;
		vDir.w = 0.f;
		vDir.Normalize();

		_float dropSpeed = 0.5f * m_InstanceData.fGravityAcceleration * m_fTimeAcc * m_fTimeAcc;
		dropSpeed = min(dropSpeed, m_InstanceData.fTerminalSpeed);
		Vector4 vPos = pVertices[i].vTranslation;
		vPos.y -= dropSpeed * fTimeDelta;

		m_vecInstanceSpeed[i] *= (1.f - m_InstanceData.fDrag);

		_float durationRate = m_vecInstanceLifeTime[i] / m_vecInstanceOriginLifeTime[i];

		pVertices[i].vTranslation = vPos + vDir * m_vecInstanceSpeed[i] * fTimeDelta;
		pVertices[i].vColor = m_InstanceData.vColor;
		pVertices[i].vColor.w = durationRate;

		m_vecInstanceLifeTime[i] -= fTimeDelta;
		if (m_vecInstanceLifeTime[i] <= 0.f)
		{
			if (m_InstanceData.isLoop)
			{
				_float fSizeX = m_pGameInstance.lock()->Rand_Num(m_InstanceData.vMinSize.x, m_InstanceData.vMaxSize.x);
				_float fSizeY = m_pGameInstance.lock()->Rand_Num(m_InstanceData.vMinSize.y, m_InstanceData.vMaxSize.y);
				_float fWidth = m_pGameInstance.lock()->Rand_Num(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
				_float fHeight = m_pGameInstance.lock()->Rand_Num(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
				_float fDepth = m_pGameInstance.lock()->Rand_Num(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);

				m_vecInstanceLifeTime[i] = m_pGameInstance.lock()->Rand_Num(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);
				m_vecInstanceOriginLifeTime[i] = m_vecInstanceLifeTime[i];
				m_vecInstanceSpeed[i] = m_pGameInstance.lock()->Rand_Num(m_InstanceData.vSpeed.x, m_InstanceData.vSpeed.y);

				reinterpret_cast<VTXINSTANCE*>(subResource.pData)[i].vRight = Vector4(fSizeX, 0.f, 0.f, 0.f);
				reinterpret_cast<VTXINSTANCE*>(subResource.pData)[i].vUp = Vector4(0.f, fSizeY, 0.f, 0.f);
				reinterpret_cast<VTXINSTANCE*>(subResource.pData)[i].vLook = Vector4(0.f, 0.f, 1.f, 0.f);
				reinterpret_cast<VTXINSTANCE*>(subResource.pData)[i].vTranslation = Vector4(
					m_InstanceData.vCenter.x + fWidth,
					m_InstanceData.vCenter.y + fHeight,
					m_InstanceData.vCenter.z + fDepth,
					1.f);

			}
			else
				reinterpret_cast<VTXINSTANCE*>(subResource.pData)[i].vColor.w = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

shared_ptr<CPointParticle> Engine::CPointParticle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumInstance, const INSTANCE_DESC& initData)
{
	shared_ptr<CPointParticle> pInstance = make_shared<CPointParticle>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iNumInstance, initData)))
	{
		MSG_BOX("Failed to Created : CPointParticle");
		pInstance.reset();
	}

	return pInstance;
}