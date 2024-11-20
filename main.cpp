#include "capturer.h"
#include "window.h"

#include <iostream>
#include <chrono>

int main()
{
    // 获取主显示器尺寸
    auto [捕获宽度, 捕获高度] = 获取主显示器尺寸();
    auto 窗口句柄 = 创建窗口();
    auto [图形上下文, 图形设备, 交换链] = 创建设备和交换链(窗口句柄, 捕获宽度, 捕获高度);

    // 获取后缓冲
    Microsoft::WRL::ComPtr<ID3D11Texture2D> 后缓冲;
    HRESULT 结果 = 交换链->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                     reinterpret_cast<void **>(后缓冲.GetAddressOf()));
    if (FAILED(结果))
    {
        std::cerr << "无法访问交换链的后缓冲\n";
    }

    // 创建渲染目标视图
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> 渲染目标视图;
    结果 = 图形设备->CreateRenderTargetView(后缓冲.Get(), nullptr, 渲染目标视图.GetAddressOf());
    if (FAILED(结果))
    {
        std::cerr << "创建渲染目标视图失败\n";
    }

    图形上下文->OMSetRenderTargets(1, 渲染目标视图.GetAddressOf(), nullptr);

    定义视口(图形上下文, 捕获宽度, 捕获高度);

    auto 复制器 = 获取屏幕复制器(图形设备);

    // 设置背景色
    auto 设置背景 = [&]()
    {
        static float 红 = 0.0f;
        static float 绿 = 0.2f;
        static float 蓝 = 0.4f;
        const float 清除颜色[4] = {红, 绿, 蓝, 1.0f};
        图形上下文->ClearRenderTargetView(渲染目标视图.Get(), 清除颜色);
        交换链->Present(1, 0);
        static float 增量 = 0.02f;
        红 += 增量;
        if (红 > 1.0f || 红 < 0.0f)
        {
            增量 *= -1;
        }
    };
    设置背景();

    // 捕获帧
    auto 捕获帧 = [&]() -> ComPtr<ID3D11Texture2D>
    {
        DXGI_OUTDUPL_FRAME_INFO 帧信息;
        ComPtr<IDXGIResource> 资源;
        复制器->ReleaseFrame();
        auto 结果 = 复制器->AcquireNextFrame(20, &帧信息, &资源);
        DXGI_OUTDUPL_DESC 描述;
        复制器->GetDesc(&描述);
        if (FAILED(结果))
            return nullptr;
        ComPtr<ID3D11Texture2D> 纹理;
        结果 = 资源.As(&纹理);
        if (FAILED(结果))
            return nullptr;
        return 纹理;
    };

    // 获取高精度时间
    auto 获取高精度时间 = [&]() -> double
    {
        static auto 开始时间 = std::chrono::high_resolution_clock::now();
        auto 当前时间 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> 经过时间 = 当前时间 - 开始时间;
        return 经过时间.count();
    };

    bool 运行中 = true;
    const double 目标帧时间 = 1.0 / 120.0; // fps
    double 上一帧时间 = 获取高精度时间();
    MSG 消息 = {};

    while (运行中)
    {
        while (PeekMessage(&消息, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&消息);
            DispatchMessage(&消息);

            if (消息.message == WM_QUIT || 消息.message == WM_CLOSE)
            {
                运行中 = false;
                break;
            }
        }

        double 当前时间 = 获取高精度时间();
        double 时间差 = 当前时间 - 上一帧时间;

        if (时间差 >= 目标帧时间)
        {
            auto 纹理 = 捕获帧();
            图形上下文->OMSetRenderTargets(1, 渲染目标视图.GetAddressOf(), nullptr);
            图形上下文->CopyResource(后缓冲.Get(), 纹理.Get());
            交换链->Present(0, 0);

            上一帧时间 = 当前时间;
        }
        else
        {
            Sleep(1);
        }
    }

    return 0;
}