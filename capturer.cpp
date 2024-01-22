#include "capturer.h"

DXGI_SWAP_CHAIN_DESC createSwapChainDesc(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 1920;
    sd.BufferDesc.Height = 1080;
    // sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    return sd;
}

ComPtr<ID3D11Device> createDevice()
{
    const std::vector<D3D_FEATURE_LEVEL> featureLevels = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1 
    };

    ComPtr<ID3D11Device> device;
    D3D_FEATURE_LEVEL lvl;
    ComPtr<ID3D11DeviceContext> context;

    auto hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        featureLevels.data(), featureLevels.size(), D3D11_SDK_VERSION,
        &device, &lvl, &context);

    return device;
}

std::tuple<ComPtr<ID3D11DeviceContext>, ComPtr<ID3D11Device>, ComPtr<IDXGISwapChain>> createDeviceAndSwapChain(HWND hwnd)
{
    const std::vector<D3D_FEATURE_LEVEL> featureLevels = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1 
    };

    ComPtr<ID3D11Device> device;
    D3D_FEATURE_LEVEL lvl;
    ComPtr<ID3D11DeviceContext> context;
    
    ComPtr<IDXGISwapChain> swapChain;
    auto swapChainDesc = createSwapChainDesc(hwnd);

    auto hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        featureLevels.data(), featureLevels.size(), D3D11_SDK_VERSION,
        &swapChainDesc, &swapChain,
        &device, &lvl, &context);

    return std::make_tuple(context, device, swapChain);
}

void defineTheViewPort(ComPtr<ID3D11DeviceContext> context, int width, int height)
{
    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<float>(1920);  // width of the render target
    viewport.Height = static_cast<float>(1080);  // height of the render target
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

    context->RSSetViewports(1, &viewport);
}

ComPtr<IDXGIOutputDuplication> GetDuplicatorFromDevice(ComPtr<ID3D11Device> device)
{
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    HRESULT hr = device.As(&dxgiDevice);
    if (FAILED(hr))
        return nullptr;
    Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
    hr = dxgiDevice->GetAdapter(&dxgiAdapter);
    if (FAILED(hr))
        return nullptr;
    Microsoft::WRL::ComPtr<IDXGIOutput> dxgiOutput;
    hr = dxgiAdapter->EnumOutputs(0, &dxgiOutput);
    if (FAILED(hr))
        return nullptr;
    Microsoft::WRL::ComPtr<IDXGIOutput1> dxgiOutput1;
    hr = dxgiOutput.As(&dxgiOutput1);
    if (FAILED(hr))
        return nullptr;
    Microsoft::WRL::ComPtr<IDXGIOutputDuplication> dxgiOutputDuplication;
    hr = dxgiOutput1->DuplicateOutput(device.Get(), &dxgiOutputDuplication);
    if (FAILED(hr))
        return nullptr;
    return dxgiOutputDuplication;
}

