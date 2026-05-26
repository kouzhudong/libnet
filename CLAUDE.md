# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Windows网络应用层库（libnet），收集本机各种网络操作的API用法、枚举和信息收集。不是扫描器，是本地网络信息查看器。

## Build Commands

C/C++ 使用 MSBuild（需要 Visual Studio，当前为 VS 2026）：
```powershell
# 构建整个解决方案（Debug x64）
msbuild /m /p:Configuration=Debug /p:Platform=x64 libnet.sln

# 构建单个项目
msbuild /m /p:Configuration=Debug /p:Platform=x64 libnet\libnet.vcxproj
msbuild /m /p:Configuration=Debug /p:Platform=x64 NetTool\NetTool.vcxproj
```

Rust FFI 绑定（在 `rust/` 目录下）：
```powershell
cd rust && cargo build --verbose
```
Rust 构建依赖 libnet.lib 已存在于 `x64\Debug\`，需先构建 C++ 项目。

CI 在 GitHub Actions 中运行：`github/workflows/msbuild.yml`（注意：不是 `.github/`）。

## Solution Structure

四个 vcxproj 项目，Solution 文件为 `libnet.sln`，支持 Debug/Release × x86/x64：

- **libnet** (`libnet/`) — 核心 DLL。导出函数均为 `EXTERN_C` + `NTAPI` 调用约定。公共头文件在 `inc/libnet.h`（C 接口，UTF-8 编码，供外部和 Rust bindgen 使用）。
- **NetTool** (`NetTool/`) — 命令行工具 EXE，依赖 libnet.lib。子命令包括 ping、pathping、tracert、whois、arp、route、ipconfig、wfp、spi、nslookup、wlan、finger、net、sock 等。
- **NetMon** (`NetMon/`) — 网络变化监控程序，使用 INetworkListManager 和各种 Notify 回调监听网络连接/断开事件。
- **test** (`test/`) — 测试/示例程序 EXE，依赖 libnet.lib。

## Key Dependencies

- Git submodule: `wil/`（Microsoft Windows Implementation Libraries），clone 后需 `git submodule update --init --recursive`。
- 仅依赖 Windows SDK 系统库，不依赖第三方动态库（CRT 和 wil 除外）。
- Rust bindgen 用于从 `inc/libnet.h` 自动生成 Rust FFI 绑定。

## Code Conventions

- 导出函数：`EXTERN_C` + `__stdcall`（NTAPI），参数只用基本类型和指针，无类/模板/引用。
- 使用 SAL 注解（`_In_`、`_Out_` 等）。
- 警告级别 4，警告视为错误；启用 Microsoft Code Analysis，关闭 Clang-Tidy。
- C 标准：C17；C++ 标准：C++17 或 C++latest。
- 函数失败通过返回值传递详细信息，不主动抛异常，禁止使用断言，尽量不调用日志函数。
- 中文注释和日志使用代码页 936（GBK）。`inc/libnet.h` 为 UTF-8 编码（供 Rust bindgen）。
- **禁止出现乱码**。编辑含中文注释的文件时，必须确认文件编码（GBK 或 UTF-8）并以正确编码读写。若工具不支持该编码（如 GBK/UTF-16 LE），应改用 PowerShell 按正确编码处理，绝不可将中文变为乱码。
- **不要轻易删除注释**。修改代码时应保留原有注释，除非注释内容已明确过时或与代码矛盾。
- **不要轻易删除函数**。即使函数当前未被调用，也可能是预留接口或示例代码，不要擅自删除。

## 代码格式

- 代码格式遵循根目录下的 `.clang-format` 文件，开始编码工作前应先读取该文件。

## 控制流

- **禁用 `goto`**。错误处理改用：
  - 提前 `return` / 嵌套 `if`
  - `do { ... } while (0)` + `break`
  - RAII（C++）或 cleanup 函数
