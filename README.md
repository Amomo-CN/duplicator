# 屏幕捕获器
使用 IDXGISwapChain 进行硬件渲染到普通窗口。使用 IDXGIOutputDuplication 从屏幕复制内容。

构建方法：
cd build
cmake ..
这将创建解决方案/构建文件。
你可以手动构建，或使用 tasks.json 中的 msbuild 命令。

调试：
launch.json 包含调试命令。
