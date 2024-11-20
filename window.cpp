#include "window.h"

LRESULT CALLBACK 窗口过程(HWND 窗口句柄, UINT 消息, WPARAM 参数1, LPARAM 参数2)
{
    switch (消息)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(窗口句柄, 消息, 参数1, 参数2);
}

HWND 创建窗口()
{
    WNDCLASSA 窗口类 = {};

    窗口类.lpfnWndProc = 窗口过程;
    窗口类.hInstance = GetModuleHandleA(nullptr);
    窗口类.lpszClassName = "屏幕捕获窗口类";

    RegisterClass(&窗口类);

    HWND 窗口句柄 = CreateWindowExA(
        0,
        "屏幕捕获窗口类",
        "屏幕捕获",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL,
        NULL,
        GetModuleHandle(nullptr),
        NULL);

    if (窗口句柄 == NULL)
    {
        return 0;
    }

    ShowWindow(窗口句柄, SW_SHOW);

    return 窗口句柄;
}