<h1 align = "center">
    <b>NekiraDelegateLib</b>
</h1>

![GitHub License](https://img.shields.io/github/license/TokiraNeo/NekiraDelegate?style=flat-square)
![GitHub top language](https://img.shields.io/github/languages/top/TokiraNeo/NekiraDelegate?style=flat-square)
![C++ Std](https://img.shields.io/badge/C%2B%2B_std-%3E%3D20-%23F761AE?style=flat-square)
![CMake Version](https://img.shields.io/badge/CMake-%3E%3D3.20-%2366F59F?style=flat-square)
![GitHub commit activity](https://img.shields.io/github/commit-activity/m/TokiraNeo/NekiraDelegate?style=flat-square)

[![README CN](https://img.shields.io/badge/README-%E4%B8%AD%E6%96%87-%2331EDA8?style=for-the-badge)](/Documents/README/README.CN.MD)
[![README EN](https://img.shields.io/badge/README-EN-%2331D4ED?style=for-the-badge)](/Documents/README/README.EN.MD)

## 📃 介绍

NekiraDelegateLib 是基于信号/槽机制实现的 C++ 委托库，包含单播委托与多播委托。

## 📝 特性

|                  特性                   |
| :-------------------------------------: |
| 基于信号/槽机制实现对成员函数的安全绑定 |
|           对象销毁时自动解绑            |
|      支持 Lambda 表达式，函数对象       |
|           单播委托与多播委托            |

## 🛠️ 安装

克隆源码至本地后，进入根目录，运行如下 cmake 命令：

```powershell
cmake -S . -B build -G "Ninja"
```

```powershell
cmake --install build [--prefix] [install_dir]
```

---

## 🔗 使用

- 在 NekiraDelegateLib 的安装目录中，bin 文件夹里可以看到 NekiraDelegateLib 构建产出的两个动态库：`libCore.dll`和`libSignalSlot.dll`。在实际使用时，需要将这两个动态库拷贝到你的可执行目标同目录。

- 在 cmake 中配置使用 NekiraDelegateLib：

```cmake
find_package(NekiraDelegateLib REQUIRED)

target_link_libraries(YourTarget PRIVATE NekiraDelegateLib::Core)

target_include_directories(YourTarget PRIVATE ${NekiraDelegate_INCLUDE_DIRS})
```

## 🔖 案例

[![MultiDelegate](https://img.shields.io/badge/Example-Multi_Delegate-38E5CB?style=for-the-badge)](/Documents/NekiraDelegate/MultiDelegate.MD)

[![SingleDelegate](https://img.shields.io/badge/Example-Single_Delegate-38A8E5?style=for-the-badge)](/Documents/NekiraDelegate/SingleDelegate.MD)

---

## 📜 License

[![License](https://img.shields.io/badge/License-MIT-38E575?style=for-the-badge)](/LICENSE)
