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

## 📃 Introduction

NekiraDelegateLib is a C++ delegate library based on the signal/slot mechanism, supporting both singlecast and multicast delegates.

## 📝 Features

|                          Feature                           |
| :--------------------------------------------------------: |
| Safe binding of member functions via signal/slot mechanism |
|       Automatic unbinding when objects are destroyed       |
|      Supports Lambda expressions and function objects      |
|        Supports singlecast and multicast delegates         |
|                   Static Signature check                   |

## 🛠️ Installation

Clone the source code locally, enter the root directory, and run the following cmake commands:

```powershell
cmake -S . -B build -G "Ninja"
```

```powershell
cmake --install build [--prefix] [install_dir]
```

---

## 🔗 Usage

- Configure NekiraDelegateLib in your cmake project:

```cmake
find_package(NekiraDelegateLib REQUIRED)

target_link_libraries(YourTarget PRIVATE NekiraDelegateLib::DelegateCore)

target_include_directories(YourTarget PRIVATE ${NekiraDelegate_INCLUDE_DIRS})
```

- include header:

```c++
#include <NekiraDelegate/Core/Macro.hpp>
```

## 🔖 Examples

[![MultiDelegate](https://img.shields.io/badge/Example-Multi_Delegate-38E5CB?style=for-the-badge)](/Documents/NekiraDelegate/MultiDelegate.MD)

[![SingleDelegate](https://img.shields.io/badge/Example-Single_Delegate-38A8E5?style=for-the-badge)](/Documents/NekiraDelegate/SingleDelegate.MD)

---

## 📜 License

[![License](https://img.shields.io/badge/License-MIT-38E575?style=for-the-badge)](/LICENSE)
