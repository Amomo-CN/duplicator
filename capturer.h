#include "d3d11.h"
#include "wrl.h"
#include "dxgi1_2.h"

#pragma once

#include <vector>
#include <tuple>

using Microsoft::WRL::ComPtr;

// 创建交换链描述符
DXGI_SWAP_CHAIN_DESC 创建交换链描述(HWND 窗口句柄, size_t 捕获宽度, size_t 捕获高度);

// 创建D3D设备
ComPtr<ID3D11Device> 创建设备();

// 创建设备和交换链
std::tuple<ComPtr<ID3D11DeviceContext>, ComPtr<ID3D11Device>, ComPtr<IDXGISwapChain>>
创建设备和交换链(HWND 窗口句柄, size_t 捕获宽度, size_t 捕获高度);

// 定义视口
void 定义视口(ComPtr<ID3D11DeviceContext> 上下文, int 宽度, int 高度);

// 从设备获取屏幕复制器
ComPtr<IDXGIOutputDuplication> 获取屏幕复制器(ComPtr<ID3D11Device> 设备);

// 获取主显示器尺寸
std::pair<UINT, UINT> 获取主显示器尺寸();