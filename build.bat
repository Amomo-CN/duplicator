@echo off
chcp 65001

:: 检查 CMake 是否已安装
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo 错误：未找到 CMake
    echo 请从 https://cmake.org/download/ 下载并安装 CMake
    echo 安装时请选择 "Add CMake to the system PATH for all users"
    pause
    exit /b 1
)

:: 确保在项目根目录
if not exist CMakeLists.txt (
    echo 错误：请在项目根目录运行此脚本
    pause
    exit /b 1
)

:: 尝试结束可能占用输出文件的进程
taskkill /F /IM duplicator.exe 2>nul

:: 等待一会儿让系统释放文件
timeout /t 2 >nul

:: 尝试删除 build 目录
if exist build (
    echo 正在清理 build 目录...
    rd /s /q build 2>nul
    if exist build (
        echo 警告：无法删除 build 目录，将尝试直接使用
    ) else (
        echo 清理完成
    )
)

:: 创建 build 目录
if not exist build mkdir build
cd build

:: 生成 Visual Studio 项目文件
echo 正在生成项目文件...
cmake .. -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% NEQ 0 (
    echo CMake 配置失败
    cd ..
    pause
    exit /b 1
)

:: 编译项目
echo 正在编译项目...
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo 编译失败
    cd ..
    pause
    exit /b 1
)

:: 检查编译结果
if exist Release\duplicator.exe (
    echo 编译成功！
    echo 可执行文件位置：%CD%\Release\duplicator.exe
    :: 复制到更方便的位置
    copy Release\duplicator.exe ..\duplicator.exe >nul
    echo 已复制可执行文件到项目根目录
) else (
    echo 警告：编译可能成功，但未找到可执行文件
)

cd ..
pause 