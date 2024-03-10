#include "Renderer.h"
#include "GameObject.h"
#include "Shader.h"
#include "GameInstance.h"
#include "RectProp.h"

Engine::CRenderer::CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice),
    m_pContext(pContext),
    m_pGameInstance(CGameInstance::GetInstance())
{
}

void Engine::CRenderer::DarkWindowActivate(_float fDarkDuration)
{
    if (fDarkDuration <= 0.f)
        return;

    m_fDarkWindowDurationOrigin = fDarkDuration;
    m_fDarkWindowDurationCheck = 0.f;

    m_bUseDarkMode = true;
}

void Engine::CRenderer::ScreenBlurActivate(_float fScreenBlurDuration, _float fBlurPower)
{
    if (fScreenBlurDuration <= 0.f)
        return;

    m_fScreenBlurDurationOrigin = fScreenBlurDuration;
    m_fScreenBlurDurationCheck = 0.f;
    m_fBlurPower = fBlurPower;

    m_bUseScreenBlur = true;
}

HRESULT Engine::CRenderer::Initialize(const INIT_DESC& initDesc)
{
    m_fUpScalingSizeX = initDesc.iUpScalingDepthStencilSizeX;
    m_fUpScalingSizeY = initDesc.iUpScalingDepthStencilSizeY;

    D3D11_VIEWPORT			ViewPortDesc{};

    _uint		iNumViewports = 1;

    m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);
    
    m_fScreenSizeX = ViewPortDesc.Width;
    m_fScreenSizeY = ViewPortDesc.Height;

    /* 내 게임에 필요한 렌더타겟들을 추가한다.*/

    /* For.Target_Diffuse */
    if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Diffuse"), m_fScreenSizeX, m_fScreenSizeY, DXGI_FORMAT_B8G8R8A8_UNORM, Vector4(1.f, 1.f, 1.f, 0.f))))
        return E_FAIL;

    /* For.Target_Normal */
    if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Normal"), m_fScreenSizeX, m_fScreenSizeY, DXGI_FORMAT_R16G16B16A16_UNORM, Vector4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    /* For.Target_Shade*/
    if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Shade"), m_fScreenSizeX, m_fScreenSizeY, DXGI_FORMAT_R16G16B16A16_UNORM, Vector4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Depth*/
    if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Depth"), m_fScreenSizeX, m_fScreenSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Shadow*/
    if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Shadow"), m_fUpScalingSizeX, m_fUpScalingSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;
    
    /* For.Target_Distortion*/
    if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Distortion"), m_fScreenSizeX, m_fScreenSizeY, DXGI_FORMAT_B8G8R8A8_UNORM, Vector4(0.f,0.f,0.f, 1.f))))
        return E_FAIL;
    
    /* For.Target_Outline*/
    if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Outline"), m_fScreenSizeX, m_fScreenSizeY, DXGI_FORMAT_B8G8R8A8_UNORM, Vector4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    /* For.Target_DarkMode*/
    if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_DarkMode"), m_fScreenSizeX, m_fScreenSizeY, DXGI_FORMAT_B8G8R8A8_UNORM, Vector4(0.f,0.f,0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Effect & Blur */
    if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Effect"), m_fScreenSizeX, m_fScreenSizeY, DXGI_FORMAT_B8G8R8A8_UNORM, Vector4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_BlurX"), m_fScreenSizeX, m_fScreenSizeY, DXGI_FORMAT_B8G8R8A8_UNORM, Vector4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_BlurY"), m_fScreenSizeX, m_fScreenSizeY, DXGI_FORMAT_B8G8R8A8_UNORM, Vector4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_Fog*/
    if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Fog"), m_fScreenSizeX, m_fScreenSizeY, DXGI_FORMAT_B8G8R8A8_UNORM, Vector4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    

    /* For.MRT_GameObjects */
    if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
        return E_FAIL;



    /* For.MRT_LightAcc */
    if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
        return E_FAIL;

    /* For.MRT_Shadow */
    if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_Shadow"))))
        return E_FAIL;

    /* For.MRT_Distortion */
    if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
        return E_FAIL;

    /* For.MRT_Outline */
    if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_Outline"), TEXT("Target_Outline"))))
        return E_FAIL;

    /* For.MRT_DarkMode */
    if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_DarkMode"), TEXT("Target_DarkMode"))))
        return E_FAIL;
   
    /* For.MRT_Effect */
    if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect"))))
        return E_FAIL;
    /* For.MRT_BlurX */
    if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_BlurX"), TEXT("Target_BlurX"))))
        return E_FAIL;
    /* For.MRT_BluY */
    if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_BlurY"), TEXT("Target_BlurY"))))
        return E_FAIL;

    /* For.MRT_Fog */
    if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_Fog"), TEXT("Target_Fog"))))
        return E_FAIL;

    m_pRectProp = m_pGameInstance.lock()->Get_RectProp();

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
    m_pPostProcessShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcess.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    m_WorldMatrix = XMMatrixIdentity();
    m_WorldMatrix._11 = m_fScreenSizeX;
    m_WorldMatrix._22 = m_fScreenSizeY;

    m_ViewMatrix = XMMatrixIdentity();
    m_ProjMatrix = XMMatrixOrthographicLH(m_fScreenSizeX, m_fScreenSizeY, 0.f, 1.f);

#ifdef _DEBUG
    if (FAILED(m_pGameInstance.lock()->Init_Debug(TEXT("Target_Diffuse"), 50.f, 50.f, 100.0f, 100.0f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Init_Debug(TEXT("Target_Normal"), 50.f, 150.f, 100.0f, 100.0f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Init_Debug(TEXT("Target_Shade"), 50.f, 250.f, 100.0f, 100.0f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Init_Debug(TEXT("Target_Depth"), 50.f, 350.f, 100.0f, 100.0f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Init_Debug(TEXT("Target_Distortion"), 150.f, 50.f, 100.0f, 100.0f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Init_Debug(TEXT("Target_Outline"), 150.f, 150.f, 100.0f, 100.0f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Init_Debug(TEXT("Target_DarkMode"), 150.f, 250.f, 100.0f, 100.0f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Init_Debug(TEXT("Target_Shadow"), m_fScreenSizeX - 150.0f, 150.0f, 300.0f, 300.0f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Init_Debug(TEXT("Target_Effect"), 50.f, 450.f, 100.0f, 100.0f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Init_Debug(TEXT("Target_BlurX"), 150.f, 350.f, 100.0f, 100.0f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Init_Debug(TEXT("Target_BlurY"), 150.f, 450.f, 100.0f, 100.0f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Init_Debug(TEXT("Target_Fog"), 50.f, 550.f, 100.0f, 100.0f)))
        return E_FAIL;

#endif // _DEBUG

#pragma region ForShadow
    // upscaling용 깊이 스텐실 버퍼 생성
    ID3D11Texture2D* pDepthStencil = nullptr;
    D3D11_TEXTURE2D_DESC TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = m_fUpScalingSizeX;
    TextureDesc.Height = m_fUpScalingSizeY;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencil)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencil, nullptr, m_pShadowDepthStencilView.GetAddressOf())))
        return E_FAIL;

    Safe_Release(pDepthStencil);
#pragma endregion

#pragma region ForBackBufferSRV

    ID3D11RenderTargetView* pBackBufferRTV = nullptr;
    m_pContext->OMGetRenderTargets(1, &pBackBufferRTV, nullptr);

    D3D11_RENDER_TARGET_VIEW_DESC pBackBufferDesc;
    pBackBufferRTV->GetDesc(&pBackBufferDesc);

    D3D11_TEXTURE2D_DESC Tex2DDesc;
    ZeroMemory(&Tex2DDesc, sizeof(D3D11_TEXTURE2D_DESC));

    Tex2DDesc.Width = (_uint)m_fScreenSizeX;
    Tex2DDesc.Height = (_uint)m_fScreenSizeY;
    Tex2DDesc.MipLevels = 1;
    Tex2DDesc.ArraySize = 1;
    Tex2DDesc.Format = pBackBufferDesc.Format;
    Tex2DDesc.SampleDesc.Quality = 0;
    Tex2DDesc.SampleDesc.Count = 1;
    Tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
    Tex2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    Tex2DDesc.CPUAccessFlags = 0;
    Tex2DDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&Tex2DDesc, nullptr, m_pBackBufferCopyTex2D.GetAddressOf())))
        return E_FAIL;

    Safe_Release(pBackBufferRTV);

#pragma endregion

    return S_OK;
}

void Engine::CRenderer::Tick(_float fTimeDelta)
{
    if (m_bUseDarkMode)
    {
        m_fDarkWindowDurationCheck += fTimeDelta;
        if (m_fDarkWindowDurationCheck >= m_fDarkWindowDurationOrigin)
            m_bUseDarkMode = false;
    }

    if (m_bUseScreenBlur)
    {
        m_fScreenBlurDurationCheck += fTimeDelta;
        if (m_fScreenBlurDurationCheck >= m_fScreenBlurDurationOrigin)
            m_bUseScreenBlur = false;
    }
}

HRESULT Engine::CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, shared_ptr<CGameObject> pGameObject)
{
    if (eRenderGroup >= RENDER_END || pGameObject == nullptr)
        return E_FAIL;

    m_RenderObjectList[eRenderGroup].push_back(pGameObject);

    return S_OK;
}

HRESULT Engine::CRenderer::Render()
{
    if (FAILED(Render_Priority())) return E_FAIL;
    if (FAILED(Render_Shadow())) return E_FAIL;
    
    // Deferred
    if (FAILED(Render_NonBlend())) return E_FAIL;
    if (FAILED(Render_Lights())) return E_FAIL;
    if (FAILED(Render_Final())) return E_FAIL;
    
    // OutLine(PostProcess)
    if (FAILED(Render_Outline()))
        return E_FAIL;

    // PostProcess
    if (m_bUseFog)
    {
        if (FAILED(Render_Fog())) return E_FAIL;
    }

    if (FAILED(Render_NonLight())) return E_FAIL;
        
    if (FAILED(Render_Blend())) return E_FAIL;
    

    if (m_bUseDarkMode)
    {
        if (FAILED(Render_DarkMode()))
            return E_FAIL;
    }

    if (FAILED(Render_Bloom())) return E_FAIL;

    if (FAILED(Render_Distortion())) return E_FAIL;
    
    if (m_bUseScreenBlur)
    {
        if (FAILED(Render_ScreenZoomBlur()))
            return E_FAIL;
    }

    if (FAILED(Render_UI())) return E_FAIL;

#ifdef _DEBUG
    if (FAILED(Render_Debug()))
        return E_FAIL;
#endif // _DEBUG

    return S_OK;
}

void Engine::CRenderer::Clear()
{
    for (auto& pObjectList : m_RenderObjectList)
        pObjectList.clear();

#ifdef _DEBUG
    m_DebugCom.clear();
#endif
}

#ifdef _DEBUG
HRESULT Engine::CRenderer::Add_DebugComponent(shared_ptr<CComponent> pComponent)
{
    m_DebugCom.push_back(pComponent);

    return S_OK;
}
#endif // _DEBUG

HRESULT Engine::CRenderer::Render_Priority()
{
    for (auto& pGameObject : m_RenderObjectList[RENDER_PRIORITY])
    {
        if (pGameObject != nullptr)
            pGameObject->Render();
    }

    m_RenderObjectList[RENDER_PRIORITY].clear();

    return S_OK;
}

HRESULT Engine::CRenderer::Render_NonBlend()
{
    /* Diffuse + Normal */
    if (FAILED(m_pGameInstance.lock()->Begin_MRT(TEXT("MRT_GameObjects"))))
        return E_FAIL;

    for (auto& pGameObject : m_RenderObjectList[RENDER_NONBLEND])
    {
        if (pGameObject != nullptr)
            pGameObject->Render();
    }

    m_RenderObjectList[RENDER_NONBLEND].clear();

    if (FAILED(m_pGameInstance.lock()->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT Engine::CRenderer::Render_Lights()
{
    Matrix ViewMatrixInv = m_pGameInstance.lock()->Get_ViewMatrixInverse();
    Matrix ProjMatrixInv = m_pGameInstance.lock()->Get_ProjMatrixInverse();
    _float fCamFar = m_pGameInstance.lock()->Get_CamFar();

    // Shade
    if (FAILED(m_pGameInstance.lock()->Begin_MRT(L"MRT_LightAcc")))
        return E_FAIL;

    // 사각형을 직교투영으로 화면에 꽉 채워서 그린다.
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &ProjMatrixInv)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &ViewMatrixInv)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fFar", &fCamFar, sizeof(fCamFar))))
        return E_FAIL;
    //if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(Vector4))))
    //    return E_FAIL;

    // 노말 렌더타겟을 쉐이더에 던진다
    if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Normal", m_pShader, "g_NormalTexture")))
        return E_FAIL;

    // 빛들을 하나씩 그린다(사각형 버퍼를 쉐이드 타겟이 그린다
    m_pGameInstance.lock()->Render_Light(m_pShader);

    if (FAILED(m_pGameInstance.lock()->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT Engine::CRenderer::Render_Final()
{
    // 사각형을 직교투영으로 화면에 꽉 채워서 그린다.
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    // 디퓨즈 렌더타겟을 쉐이더에 던진다
    if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Diffuse", m_pShader, "g_DiffuseTexture")))
        return E_FAIL;
    // 셰이더 렌더타겟을 쉐이더에 던진다
    if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Shade", m_pShader, "g_ShadeTexture")))
        return E_FAIL;
    // 깊이 렌더타겟을 쉐이더에 던진다
    if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Depth", m_pShader, "g_DepthTexture")))
        return E_FAIL;

    const Matrix* LightViewMatrix = m_pGameInstance.lock()->Get_LightViewMatrix(0);
    if (nullptr == LightViewMatrix)
    {
        m_pShader->Begin(3, 0);
    }
    else
    {
        Matrix LightProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

        if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", LightViewMatrix)))
            return E_FAIL;
        if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &LightProjMatrix)))
            return E_FAIL;

        Matrix ViewMatrixInv = m_pGameInstance.lock()->Get_ViewMatrixInverse();
        Matrix ProjMatrixInv = m_pGameInstance.lock()->Get_ProjMatrixInverse();
        _float fCamFar = m_pGameInstance.lock()->Get_CamFar();

        // 그림자 렌더타겟을 쉐이더에 던진다
        if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Shadow", m_pShader, "g_ShadowTexture")))
            return E_FAIL;

        if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &ProjMatrixInv)))
            return E_FAIL;
        if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &ViewMatrixInv)))
            return E_FAIL;
        if (FAILED(m_pShader->Bind_RawValue("g_fFar", &fCamFar, sizeof(fCamFar))))
            return E_FAIL;

        m_pShader->Begin(4, 0);
    }

    m_pRectProp->Bind_Buffers();
    m_pRectProp->Render();

    if (m_bUseFog)
    {
        if (FAILED(m_pGameInstance.lock()->Begin_MRT(L"MRT_Fog")))
            return E_FAIL;

        // 백버퍼 렌더타겟을 쉐이더에 던진다
        Copy_BackBufferSRV(); // 백퍼버 복사해서 SRV화
        if (FAILED(m_pPostProcessShader->Bind_SRV("g_BackBufferTexture", m_pBackBufferCopySRV)))
            return E_FAIL;
        // 뎁스 렌더타겟을 쉐이더에 던진다
        if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Depth", m_pPostProcessShader, "g_DepthTexture")))
            return E_FAIL;

        m_pPostProcessShader->Begin(7, 0);
        m_pRectProp->Bind_Buffers();
        m_pRectProp->Render();

        if (FAILED(m_pGameInstance.lock()->End_MRT()))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT Engine::CRenderer::Render_NonLight()
{
    for (auto& pGameObject : m_RenderObjectList[RENDER_NONLIGHT])
    {
        if (pGameObject != nullptr)
            pGameObject->Render();
    }

    m_RenderObjectList[RENDER_NONLIGHT].clear();

    return S_OK;
}

HRESULT Engine::CRenderer::Render_Blend()
{
    if (FAILED(m_pGameInstance.lock()->Begin_MRT(L"MRT_Effect")))
        return E_FAIL;

    for (auto& pGameObject : m_RenderObjectList[RENDER_BLEND])
    {
        if (pGameObject != nullptr)
            pGameObject->Render();
    }

    m_RenderObjectList[RENDER_BLEND].clear();

    if (FAILED(m_pGameInstance.lock()->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT Engine::CRenderer::Render_UI()
{
    for (auto& pGameObject : m_RenderObjectList[RENDER_UI])
    {
        if (pGameObject != nullptr)
            pGameObject->Render();
    }

    m_RenderObjectList[RENDER_UI].clear();

    return S_OK;
}

HRESULT Engine::CRenderer::Render_Shadow()
{
    D3D11_VIEWPORT			ViewPortDesc;
    ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
    ViewPortDesc.TopLeftX = 0;
    ViewPortDesc.TopLeftY = 0;
    ViewPortDesc.Width = m_fUpScalingSizeX;
    ViewPortDesc.Height = m_fUpScalingSizeY;
    ViewPortDesc.MinDepth = 0.f;
    ViewPortDesc.MaxDepth = 1.f;

    m_pContext->RSSetViewports(1, &ViewPortDesc);

    m_pGameInstance.lock()->Begin_MRT(TEXT("MRT_Shadow"), m_pShadowDepthStencilView);


    for (auto& pGameObject : m_RenderObjectList[RENDER_SHADOW])
    {
        if (nullptr != pGameObject)
            pGameObject->Render_Shadow();
    }

    m_RenderObjectList[RENDER_SHADOW].clear();

    //// *****************************
    //// 그림자 블러
    //// *****************************
    //if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Shadow", m_pShader, "g_ShadowTexture")))
    //    return E_FAIL;
    //if (FAILED(m_pShader->Bind_RawValue("g_fWinSizeX", &m_fUpScalingSizeX, sizeof(m_fUpScalingSizeX))))
    //    return E_FAIL;
    //if (FAILED(m_pShader->Bind_RawValue("g_fWinSizeY", &m_fUpScalingSizeY, sizeof(m_fUpScalingSizeX))))
    //    return E_FAIL;

    //m_pShader->Begin(6, 0);
    //m_pRectProp->Bind_Buffers();
    //m_pRectProp->Render();

    //m_pShader->Begin(7, 0);
    //m_pRectProp->Bind_Buffers();
    //m_pRectProp->Render();

    m_pGameInstance.lock()->End_MRT();

    ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
    ViewPortDesc.TopLeftX = 0;
    ViewPortDesc.TopLeftY = 0;
    ViewPortDesc.Width = m_fScreenSizeX;
    ViewPortDesc.Height = m_fScreenSizeY;
    ViewPortDesc.MinDepth = 0.f;
    ViewPortDesc.MaxDepth = 1.f;

    m_pContext->RSSetViewports(1, &ViewPortDesc);

    return S_OK;
}

HRESULT Engine::CRenderer::Render_Distortion()
{
    if (FAILED(m_pGameInstance.lock()->Begin_MRT(TEXT("MRT_Distortion"))))
        return E_FAIL;

    for (auto& pGameObject : m_RenderObjectList[RENDER_DISTORT])
    {
        if (pGameObject != nullptr)
            pGameObject->Render_Distortion();
    }

    m_RenderObjectList[RENDER_DISTORT].clear();

    if (FAILED(m_pGameInstance.lock()->End_MRT()))
        return E_FAIL;

    // ******************************
    // PostProcess
    // ******************************
    
    // 사각형을 직교투영으로 화면에 꽉 채워서 그린다.
    if (FAILED(m_pPostProcessShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    // 뎁스 렌더타겟을 쉐이더에 던진다
    Copy_BackBufferSRV(); // 백퍼버 복사해서 SRV화
    if (FAILED(m_pPostProcessShader->Bind_SRV("g_BackBufferTexture",m_pBackBufferCopySRV)))
        return E_FAIL;
    // 뎁스 렌더타겟을 쉐이더에 던진다
    if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Depth", m_pPostProcessShader, "g_DepthTexture")))
        return E_FAIL;
    // 디스토션 렌더타겟을 쉐이더에 던진다
    if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Distortion", m_pPostProcessShader, "g_DistortionTexture")))
        return E_FAIL;

    m_pPostProcessShader->Begin(0, 0);
    m_pRectProp->Bind_Buffers();
    m_pRectProp->Render();

    return S_OK;
}

HRESULT Engine::CRenderer::Render_Outline()
{
    //// 사각형을 직교투영으로 화면에 꽉 채워서 그린다.
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fWinSizeX", &m_fScreenSizeX, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fWinSizeY", &m_fScreenSizeY, sizeof(_float))))
        return E_FAIL;
    //if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Depth", m_pShader, "g_DepthTexture")))
    //    return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Normal", m_pShader, "g_NormalTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance.lock()->Begin_MRT(TEXT("MRT_Outline"))))
        return E_FAIL;

    m_pShader->Begin(5, 0);
    m_pRectProp->Bind_Buffers();
    m_pRectProp->Render();

    if (FAILED(m_pGameInstance.lock()->End_MRT()))
        return E_FAIL;

    // ******************************
    // PostProcess
    // ******************************

    //// 사각형을 직교투영으로 화면에 꽉 채워서 그린다.
    if (FAILED(m_pPostProcessShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    //// 뎁스 렌더타겟을 쉐이더에 던진다
    if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Outline", m_pPostProcessShader, "g_OutlineTexture")))
        return E_FAIL;

    m_pPostProcessShader->Begin(1, 0);
    m_pRectProp->Bind_Buffers();
    m_pRectProp->Render();

    return S_OK;
}

HRESULT Engine::CRenderer::Render_DarkMode()
{
    if (FAILED(m_pGameInstance.lock()->Begin_MRT(TEXT("MRT_DarkMode"))))
        return E_FAIL;

    for (auto& pGameObject : m_RenderObjectList[RENDER_EXCEPTDARK])
    {
        if (pGameObject != nullptr)
            pGameObject->Render_ExceptDark();
    }

    m_RenderObjectList[RENDER_EXCEPTDARK].clear();

    if (FAILED(m_pGameInstance.lock()->End_MRT()))
        return E_FAIL;

    // ******************************
    // PostProcess
    // ******************************
    
    // 사각형을 직교투영으로 화면에 꽉 채워서 그린다.
    if (FAILED(m_pPostProcessShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    // 다크모드 렌더타겟을 쉐이더에 던진다
    if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_DarkMode", m_pPostProcessShader, "g_ExceptDarkModeTexture")))
        return E_FAIL;

    _float fTimeRate = m_fDarkWindowDurationCheck / m_fDarkWindowDurationOrigin;
    if (FAILED(m_pPostProcessShader->Bind_RawValue("g_fTimeRate", &fTimeRate, sizeof(_float))))
        return E_FAIL;

    m_pPostProcessShader->Begin(2, 0);
    m_pRectProp->Bind_Buffers();
    m_pRectProp->Render();

    return S_OK;

    return S_OK;
}

HRESULT Engine::CRenderer::Render_Bloom()
{
    //// 사각형을 직교투영으로 화면에 꽉 채워서 그린다.
    if (FAILED(m_pPostProcessShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostProcessShader->Bind_RawValue("g_fTexWidth", &m_fScreenSizeX, sizeof(m_fScreenSizeX))))
        return E_FAIL;
    if (FAILED(m_pPostProcessShader->Bind_RawValue("g_fTexHeight", &m_fScreenSizeY, sizeof(m_fScreenSizeX))))
        return E_FAIL;


    // ******************************
    // XBlur
    // ******************************

    if (FAILED(m_pGameInstance.lock()->Begin_MRT(TEXT("MRT_BlurX"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Effect", m_pPostProcessShader, "g_EffectTexture")))
        return E_FAIL;

    m_pPostProcessShader->Begin(3, 0);
    m_pRectProp->Bind_Buffers();
    m_pRectProp->Render();

    if (FAILED(m_pGameInstance.lock()->End_MRT()))
        return E_FAIL;

    // ******************************
    // YBlur 
    // ******************************

    if (FAILED(m_pGameInstance.lock()->Begin_MRT(TEXT("MRT_BlurY"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_BlurX", m_pPostProcessShader, "g_BlurXTexture")))
        return E_FAIL;

    m_pPostProcessShader->Begin(4, 0);
    m_pRectProp->Bind_Buffers();
    m_pRectProp->Render();

    if (FAILED(m_pGameInstance.lock()->End_MRT()))
        return E_FAIL;

    // ******************************
    // Glow 
    // ******************************
    Copy_BackBufferSRV(); // 백퍼버 복사해서 SRV화`
    if (FAILED(m_pPostProcessShader->Bind_SRV("g_BackBufferTexture", m_pBackBufferCopySRV)))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Effect", m_pPostProcessShader, "g_EffectTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_BlurY", m_pPostProcessShader, "g_BlurYTexture")))
        return E_FAIL;

    m_pPostProcessShader->Begin(5, 0);
    m_pRectProp->Bind_Buffers();
    m_pRectProp->Render();

    return S_OK;
}

HRESULT Engine::CRenderer::Render_ScreenZoomBlur()
{
    _float fTimeRate = m_fScreenBlurDurationCheck / m_fScreenBlurDurationOrigin;

    Copy_BackBufferSRV(); // 백퍼버 복사해서 SRV화`
    if (FAILED(m_pPostProcessShader->Bind_SRV("g_BackBufferTexture", m_pBackBufferCopySRV)))
        return E_FAIL;
    if(FAILED(m_pPostProcessShader->Bind_RawValue("g_fTexWidth", &m_fScreenSizeX, sizeof(m_fScreenSizeX))))
        return E_FAIL;
    if (FAILED(m_pPostProcessShader->Bind_RawValue("g_fTimeRate", &fTimeRate, sizeof(fTimeRate))))
        return E_FAIL;
    if (FAILED(m_pPostProcessShader->Bind_RawValue("g_fBlurSize", &m_fBlurPower, sizeof(m_fBlurPower))))
        return E_FAIL;

    m_pPostProcessShader->Begin(6, 0);
    m_pRectProp->Bind_Buffers();
    m_pRectProp->Render();

    return S_OK;
}

HRESULT Engine::CRenderer::Render_Fog()
{
    if (FAILED(m_pGameInstance.lock()->Bind_SRV(L"Target_Fog", m_pPostProcessShader, "g_FogTexture")))
        return E_FAIL;

    m_pPostProcessShader->Begin(8, 0);
    m_pRectProp->Bind_Buffers();
    m_pRectProp->Render();

    return S_OK;
}

#ifdef _DEBUG
HRESULT Engine::CRenderer::Render_Debug()
{
    //if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
    //    return E_FAIL;
    //if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
    //    return E_FAIL;

    //m_pGameInstance.lock()->Render_MRT(TEXT("MRT_GameObjects"), m_pShader);

    //m_pGameInstance.lock()->Render_MRT(TEXT("MRT_LightAcc"), m_pShader);

    //m_pGameInstance.lock()->Render_MRT(TEXT("MRT_Shadow"), m_pShader);

    //m_pGameInstance.lock()->Render_MRT(TEXT("MRT_Distortion"), m_pShader);

    //m_pGameInstance.lock()->Render_MRT(TEXT("MRT_Outline"), m_pShader);

    //m_pGameInstance.lock()->Render_MRT(TEXT("MRT_DarkMode"), m_pShader);

    //m_pGameInstance.lock()->Render_MRT(TEXT("MRT_Effect"), m_pShader);

    //m_pGameInstance.lock()->Render_MRT(TEXT("MRT_BlurX"), m_pShader);

    //m_pGameInstance.lock()->Render_MRT(TEXT("MRT_BlurY"), m_pShader);

    //m_pGameInstance.lock()->Render_MRT(TEXT("MRT_Fog"), m_pShader);

    //for (auto& pComponent : m_DebugCom)
    //{
    //    pComponent->Render();
    //}

    //m_DebugCom.clear();

    return S_OK;
}
#endif // _DEBUG

HRESULT Engine::CRenderer::Copy_BackBufferSRV()
{
    // 백버퍼를 SRV로 복사받기
    ID3D11RenderTargetView* pBackBufferRTV = nullptr;
    m_pContext->OMGetRenderTargets(1, &pBackBufferRTV, nullptr);

    ID3D11Resource* pBackBufferResource = nullptr;
    pBackBufferRTV->GetResource(&pBackBufferResource);

    m_pContext->CopyResource(m_pBackBufferCopyTex2D.Get(), pBackBufferResource);

    // 백버퍼 리소스 갱신
    m_pBackBufferCopySRV.Reset();

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pBackBufferCopyTex2D.Get(), nullptr, m_pBackBufferCopySRV.GetAddressOf())))
        return E_FAIL;

    Safe_Release(pBackBufferResource);
    Safe_Release(pBackBufferRTV);

    return S_OK;
}

shared_ptr<CRenderer> Engine::CRenderer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc)
{
    shared_ptr<CRenderer> pInstance = make_shared<CRenderer>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(initDesc)))
    {
        MSG_BOX("Failed to Created : CRenderer");
        pInstance.reset();
    }

    return pInstance;
}
