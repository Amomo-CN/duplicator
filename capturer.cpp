#include "capturer.h"

std::pair<UINT, UINT> 获取主显示器尺寸()
{
    ComPtr<IDXGIFactory1> 工厂;
    ComPtr<IDXGIAdapter1> 适配器;
    CreateDXGIFactory1(__uuidof(IDXGIFactory1), &工厂);
    工厂->EnumAdapters1(0, &适配器);
    ComPtr<IDXGIOutput> 输出;
    适配器->EnumOutputs(0, &输出);
    DXGI_OUTPUT_DESC 输出描述;
    输出->GetDesc(&输出描述);
    return {输出描述.DesktopCoordinates.right,
            输出描述.DesktopCoordinates.bottom};
}

DXGI_SWAP_CHAIN_DESC 创建交换链描述(HWND 窗口句柄, size_t 捕获宽度, size_t 捕获高度)
{
    DXGI_SWAP_CHAIN_DESC 描述;
    ZeroMemory(&描述, sizeof(描述));
    描述.BufferCount = 1;
    描述.BufferDesc.Width = 捕获宽度;
    描述.BufferDesc.Height = 捕获高度;
    描述.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    描述.BufferDesc.RefreshRate.Numerator = 60;
    描述.BufferDesc.RefreshRate.Denominator = 1;
    描述.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    描述.OutputWindow = 窗口句柄;
    描述.SampleDesc.Count = 1;
    描述.SampleDesc.Quality = 0;
    描述.Windowed = TRUE;

    return 描述;
}

ComPtr<ID3D11Device> 创建设备()
{
    const std::vector<D3D_FEATURE_LEVEL> 特性级别 = {
        D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1};

    ComPtr<ID3D11Device> 设备;
    D3D_FEATURE_LEVEL 级别;
    ComPtr<ID3D11DeviceContext> 上下文;

    auto 结果 = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
                                  特性级别.data(), 特性级别.size(), D3D11_SDK_VERSION,
                                  &设备, &级别, &上下文);

    return 设备;
}

std::tuple<ComPtr<ID3D11DeviceContext>, ComPtr<ID3D11Device>, ComPtr<IDXGISwapChain>>
创建设备和交换链(HWND 窗口句柄, size_t 捕获宽度, size_t 捕获高度)
{
    const std::vector<D3D_FEATURE_LEVEL> 特性级别 = {
        D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1};

    ComPtr<ID3D11Device> 设备;
    D3D_FEATURE_LEVEL 级别;
    ComPtr<ID3D11DeviceContext> 上下文;

    ComPtr<IDXGISwapChain> 交换链;
    auto 交换链描述 = 创建交换链描述(窗口句柄, 捕获宽度, 捕获高度);

    auto 结果 = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
                                              特性级别.data(), 特性级别.size(), D3D11_SDK_VERSION,
                                              &交换链描述, &交换链,
                                              &设备, &级别, &上下文);

    return std::make_tuple(上下文, 设备, 交换链);
}

void 定义视口(ComPtr<ID3D11DeviceContext> 上下文, int 宽度, int 高度)
{
    D3D11_VIEWPORT 视口;
    视口.Width = static_cast<float>(宽度);
    视口.Height = static_cast<float>(高度);
    视口.MinDepth = 0.0f;
    视口.MaxDepth = 1.0f;
    视口.TopLeftX = 0;
    视口.TopLeftY = 0;

    上下文->RSSetViewports(1, &视口);
}

ComPtr<IDXGIOutputDuplication> 获取屏幕复制器(ComPtr<ID3D11Device> 设备)
{
    Microsoft::WRL::ComPtr<IDXGIDevice> DXGI设备;
    HRESULT 结果 = 设备.As(&DXGI设备);
    if (FAILED(结果))
        return nullptr;
    Microsoft::WRL::ComPtr<IDXGIAdapter> DXGI适配器;
    结果 = DXGI设备->GetAdapter(&DXGI适配器);
    if (FAILED(结果))
        return nullptr;
    Microsoft::WRL::ComPtr<IDXGIOutput> DXGI输出;
    结果 = DXGI适配器->EnumOutputs(0, &DXGI输出);
    if (FAILED(结果))
        return nullptr;
    Microsoft::WRL::ComPtr<IDXGIOutput1> DXGI输出1;
    结果 = DXGI输出.As(&DXGI输出1);
    if (FAILED(结果))
        return nullptr;
    Microsoft::WRL::ComPtr<IDXGIOutputDuplication> DXGI输出复制;
    结果 = DXGI输出1->DuplicateOutput(设备.Get(), &DXGI输出复制);
    if (FAILED(结果))
        return nullptr;
    return DXGI输出复制;
}
