# NekiraDelegate

## 🟩

- [中文](README.md)

## 📃 Introduction

```
NekiraDelegate is a C++ delegate system that includes both singlecast and multicast delegates.
```

## 🚀 Current Progress

- [x] Function Traits
- [x] Type Erasure
- [x] Factory Functions
- [x] Singlecast Delegate
- [x] Multicast Delegate
- [x] Quick Macro Declaration
- [ ] Library Packaging

## 🟠 C++ Standard

```
This library uses C++20 concepts. Ensure your compiler supports at least C++20.
```

## 🔖 Namespace

```
The namespace for this library is NekiraDelegate. All methods and types, except macros, are within this namespace.
```

## ✨ Features

|                    Feature                     |
| :--------------------------------------------: |
|                Function Traits                 |
|                  Type Erasure                  |
|       Singlecast and Multicast Delegates       |
|         Support for Regular Functions          |
| Support for Member Functions (const, volatile) |
|  Support for Functors and Lambda Expressions   |
|         Support for `std::function<>`          |
|  Signature Checking for Functors and Lambdas   |
|   Memory Safety (Managed by Smart Pointers)    |

## 📂 File Structure

The file structure is divided into four parts:

- `FunctionTraits/`:
  Implementation of function traits, assists with `static_assert()` for static checks.

- `CallableInterface/CallableInterface.hpp`:
  Type erasure implementation, supports regular functions, member functions (including const, volatile), functors, lambdas, and std::function<>.

- `CallableInterface/CallableFactory.hpp`:
  Provides factory functions for wrapping various callable objects and some helper functions.

- `Delegate/`:
  Delegate implementation, including singlecast delegate (`Delegate`) and multicast delegate (`MulticastDelegate`). Provides quick declaration macros for delegates.

- `Demo/NekiraDelegate.cpp`:
  Demo showcasing the usage of the NekiraDelegate system.

## 📜 License

```
MIT License

Copyright (c) 2025 TokiraNeo (https://github.com/TokiraNeo)
```
