#include "pch.h"
#include "Tool_GamePlayLevel.h"

#include "Tool_Camera.h"
#include "ToolMain.h"

Tool::CTool_GamePlayLevel::CTool_GamePlayLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CLevel(pDevice, pContext)
{
}

HRESULT Tool::CTool_GamePlayLevel::Initialize()
{
	if (FAILED(Ready_Reource()))
		return E_FAIL;
	if (FAILED(Ready_Lights()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Add_Imgui(L"ToolMain", CToolMain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("게임 플레이 레벨입니다.");

	return S_OK;
}

void Tool::CTool_GamePlayLevel::Tick(_float fTimeDelta)
{
}

HRESULT Tool::CTool_GamePlayLevel::Render()
{
	return S_OK;
}

HRESULT Tool::CTool_GamePlayLevel::Ready_Reource()
{

	return S_OK;
}

HRESULT Tool::CTool_GamePlayLevel::Ready_Lights()
{
	LIGHT_DESC LightDesc = {};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = Vector4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = Vector4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = Vector4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = Vector4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vPosition = Vector4(10.f, 10.f, 10.f, 1.f);
	LightDesc.vAt = Vector4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(m_pGameInstance.lock()->Add_Light(LightDesc))) return E_FAIL;

	return S_OK;
}

HRESULT Tool::CTool_GamePlayLevel::Ready_Layer_Camera(const wstring& strLayerTag)
{
	m_strLoadingText = TEXT("카메라를 생성 중 입니다.");
	//if (FAILED(m_pGameInstance.lock()->Add_Object(LEVEL_STATIC, strLayerTag, CTool_Camera::Create(XMConvertToRadians(60.f), 1.f, 1000.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, Vector3(0.f, 5.f, -6.f)), &g_TimeScale)))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Object(LEVEL_STATIC, strLayerTag, CTool_Camera::Create(XMConvertToRadians(60.f), 0.1f, 1000.f, (_float)g_iWinSizeX / (_float)g_iWinSizeY, Vector3(0.f, 5.f, -6.f)), &g_TimeScale)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CTool_GamePlayLevel> Tool::CTool_GamePlayLevel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CTool_GamePlayLevel> pInstance = make_shared<CTool_GamePlayLevel>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CTool_GamePlayLevel");
		pInstance.reset();
	}

	return pInstance;
}
