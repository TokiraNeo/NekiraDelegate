# NekiraDelegate

## 📃 介绍

```
NekiraDelegate 是基于 C++的委托系统，包含了单播委托与多播委托两种委托。
```

## 🚀 目前进度

- [x] 函数萃取
- [x] 类型擦除
- [x] 工厂函数
- [x] 单播委托
- [x] 多播委托
- [x] 快捷宏声明
- [ ] 整理成库

## 🟠 C++ 标准

```
本库使用了C++20 的concept，因此请确保最低标准为C++20
```

## 🔖 命名空间

```
本库的命名空间为 NekiraDelegate ，除了宏以外，其余方法和类型均在该命名空间中
```

## ✨ 特性

|                                   |
| :-------------------------------: |
|           函数萃取功能            |
|           函数类型擦除            |
|        单播委托、多播委托         |
|           支持普通函数            |
|  支持成员函数（const、volatile）  |
|    支持函数对象、Lambda 表达式    |
|       支持 std::function<>        |
| 对函数对象、Lambda 的函数签名检查 |
|  内存安全处理(基于智能指针管理)   |

## 📂 文件结构

文件结构分为四个部分：

- `FunctionTraits/` ： 函数萃取相关实现，可辅助 static_assert() 进行静态检查

- `CallableInterface/CallableInterface.hpp` ：类型擦除实现，实现了对普通函数、成员函数(包括 const，volatile)、函数对象、Lambda、std::function<>的支持

- `CallableInterface/CallableFactory.hpp` ：提供工厂函数用于包装各类可调用对象，以及一些辅助函数。

- `Delegate/` ：委托实现，包括单播委托 Delegate 和多播委托 MulticastDelegate。提供委托快捷声明宏

- `Demo/NekiraDelegate.cpp` ：演示 Demo，展示了 NekiraDelegate 系统的使用。

## 📜 License

```
MIT License

Copyright (c) 2025 TokiraNeo (https://github.com/TokiraNeo)
```

---
