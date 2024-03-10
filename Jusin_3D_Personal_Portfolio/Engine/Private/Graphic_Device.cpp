#include "Graphic_Device.h"

Engine::CGraphic_Device::~CGraphic_Device()
{
//	#if defined(DEBUG) || defined(_DEBUG)
//	ID3D11Debug* d3dDebug;
//	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
//	if (SUCCEEDED(hr))
//	{
//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//
//		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
//
//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//	}
//	if (d3dDebug != nullptr)            d3dDebug->Release();
//#endif

}

HRESULT Engine::CGraphic_Device::Init_Graphic_Device(const GRAPHIC_DESC& GraphicDesc, ComPtr<ID3D11Device>& pDeviceOut, ComPtr<ID3D11DeviceContext>& pContextOut)
{
	_uint		iFlag = 0;
#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL			FeatureLV;

	// �׷��� ��ġ �ʱ�ȭ
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, m_pDevice.GetAddressOf(), &FeatureLV, m_pDeviceContext.GetAddressOf())))
		return E_FAIL;

	// ������ ������ ���� Texture2D �����
	if (FAILED(Init_SwapChain(GraphicDesc.hWnd, GraphicDesc.isWindowed, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY)))
		return E_FAIL;

	// ����ü���� ��� �ִ� �ؽ��� 2D�� �����ͼ� �̸� �������� ����� ����Ÿ�� �並 ����� 
	if (FAILED(Init_BackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(Init_DepthStencilRenderTargetView(GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY)))
		return E_FAIL;

	// ��ġ�� ���ε��س��� ����Ÿ�ٵ�� �������ٽǺ並 �����Ѵ�.
	// ��ġ�� ���ÿ� �ִ� 8���� ����Ÿ���� ��� ���� �� �ִ�.
	ID3D11RenderTargetView* pRTVs[1] = {
	m_pBackBufferRTV.Get(),
	};

	m_pDeviceContext->OMSetRenderTargets(1, pRTVs, m_pDepthStencilView.Get());

	D3D11_VIEWPORT viewPortDesc;
	ZeroMemory(&viewPortDesc, sizeof(D3D11_VIEWPORT));
	viewPortDesc.TopLeftX = 0;
	viewPortDesc.TopLeftY = 0;
	viewPortDesc.Width = (_float)GraphicDesc.iWinSizeX;
	viewPortDesc.Height = (_float)GraphicDesc.iWinSizeY;
	viewPortDesc.MinDepth = 0.f;
	viewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &viewPortDesc);

	pDeviceOut = m_pDevice;
	pContextOut = m_pDeviceContext;

	return S_OK;
}

HRESULT Engine::CGraphic_Device::Clear_BackBuffer_View(Color vClearColor)
{
	if (m_pDeviceContext == nullptr)
		return E_FAIL;

	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV.Get(), (_float*)&vClearColor);

	return S_OK;
}

HRESULT Engine::CGraphic_Device::Clear_DepthStencil_View()
{
	if (m_pDeviceContext == nullptr)
		return E_FAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT Engine::CGraphic_Device::Present()
{
	if (m_pSwapChain == nullptr)
		return E_FAIL;

	return m_pSwapChain->Present(0,0);
}

HRESULT Engine::CGraphic_Device::Init_SwapChain(HWND hWnd, _bool isWindowed, _uint iWinCX, _uint iWinCY)
{
	ComPtr<IDXGIDevice>			pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)pDevice.GetAddressOf());

	ComPtr<IDXGIAdapter>		pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pAdapter.GetAddressOf());

	ComPtr<IDXGIFactory>		pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf());

	// ����ü���� �����Ѵ� = �ؽ�ó�� ���� + ����
	DXGI_SWAP_CHAIN_DESC	swapChain;
	ZeroMemory(&swapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	// �ؽ�ó(�����)�� �����ϴ� ����
	swapChain.BufferDesc.Width = iWinCX;
	swapChain.BufferDesc.Height = iWinCY;

	swapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChain.BufferCount = 1;

	// �����ϴ� ����
	swapChain.BufferDesc.RefreshRate.Numerator = 60;
	swapChain.BufferDesc.RefreshRate.Denominator = 1;
	swapChain.SampleDesc.Quality = 0;
	swapChain.SampleDesc.Count = 1;

	swapChain.OutputWindow = hWnd;
	swapChain.Windowed = isWindowed;
	swapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// ����۶�� �ؽ�ó�� �����ߴ�
	if (FAILED(pFactory->CreateSwapChain(m_pDevice.Get(), &swapChain, m_pSwapChain.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::CGraphic_Device::Init_BackBufferRenderTargetView()
{
	if (m_pDevice == nullptr)
		return E_FAIL;

	// ���� ������ ����ϱ����� �뵵�� �ؽ��ĸ� �����ϱ����� ���̽� �����͸� ������ �ִ� ��ü�̴�.
	// ���� ������ ����ϱ����� �뵵�� �ؽ��� : ID3D11RenderTargetView, ID3D11ShaderResoureView, ID3D11DepthStencilView
	ComPtr<ID3D11Texture2D>		pBackBufferTexture = nullptr;

	// ����ü���� ��� �ִ� �ؽ�ó�� �����´�
	if (FAILED(m_pSwapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), (void**)pBackBufferTexture.GetAddressOf())))
		return E_FAIL;

	// ����Ÿ�� �並 �����Ѵ�
	if(FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture.Get(), nullptr, m_pBackBufferRTV.GetAddressOf())))
		return E_FAIL;

	pBackBufferTexture.Reset();

	return S_OK;
}

HRESULT Engine::CGraphic_Device::Init_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY)
{
	if (m_pDevice == nullptr)
		return E_FAIL;

	ComPtr<ID3D11Texture2D>		pDepthStencilTexture = nullptr;
	D3D11_TEXTURE2D_DESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWinCX;
	TextureDesc.Height = iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, pDepthStencilTexture.GetAddressOf())))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture.Get(), nullptr, m_pDepthStencilView.GetAddressOf())))
		return E_FAIL;

	pDepthStencilTexture.Reset();

	return S_OK;
}

shared_ptr<CGraphic_Device> Engine::CGraphic_Device::Create(const GRAPHIC_DESC& GraphicDesc, ComPtr<ID3D11Device>& pDeviceOut, ComPtr<ID3D11DeviceContext>& pContextOut)
{
	shared_ptr<CGraphic_Device> pInstance = make_shared<CGraphic_Device>();

	if (FAILED(pInstance->Init_Graphic_Device(GraphicDesc, pDeviceOut, pContextOut)))
	{
		MSG_BOX("Failed to Created : CGraphic_Device");
		pInstance.reset();
	}

	return pInstance;
}