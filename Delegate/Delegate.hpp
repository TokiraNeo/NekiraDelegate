/*
MIT License

Copyright (c) 2025 TokiraNeo (https://github.com/TokiraNeo)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <algorithm>
#include "../CallableInterface/CallableFactory.hpp"



namespace NekiraDelegate
{
    // ============================================== 单播委托 ============================================== //

    template <typename RT, typename... Args>
    class Delegate
    {
    public:
        // Check if the delegate is bound
        // 委托是否有效
        bool IsValid() const
        {
            return ( CallableObj != nullptr );
        }

        // Invoke Delegate
        // 调用委托
        RT Invoke( Args... args )
        {
            return ( CallableObj ? CallableObj->Invoke( std::forward<Args>( args )... ) : RT{} );

        }

        // Bind a Normal Function
        // 绑定普通函数
        void Bind( RT( *FuncPtr )( Args... ) )
        {


            CallableObj = MakeCallableBase( FuncPtr );
        }

        // Bind a Member Function
        // 绑定成员函数
        template <typename ClassType>
        void Bind( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) )
        {
            CallableObj = MakeCallableBase( Object, FuncPtr );
        }

        // Bind a const Member Function (const Object)
        // 绑定 const 成员函数 (const 对象)
        template <typename ClassType>
        void Bind( std::shared_ptr<const ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) const )
        {
            CallableObj = MakeCallableBase( Object, FuncPtr );
        }

        // Bind a const Member Function (non const Object)
        // 绑定 const 成员函数 (非 const 对象)
        template <typename ClassType>
        void Bind( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) const )
        {
            CallableObj = MakeCallableBase( Object, FuncPtr );
        }

        // Bind a volatile Member Function (volatile Object)
        // 绑定 volatile 成员函数 (volatile 对象)
        template <typename ClassType>
        void Bind( std::shared_ptr<volatile ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) volatile )
        {
            CallableObj = MakeCallableBase( Object, FuncPtr );
        }

        // Bind a volatile Member Function (non volatile Object)
        // 绑定 volatile 成员函数 (非 volatile 对象)
        template <typename ClassType>
        void Bind( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) volatile )
        {
            CallableObj = MakeCallableBase( Object, FuncPtr );
        }

        // Bind a std::function (Left Reference)
        // 绑定 std::function (左值引用)
        void Bind( const std::function<RT( Args... )>& Function )
        {
            CallableObj = MakeCallableBase( Function );
        }

        // Bind a std::function (Right Reference)
        // 绑定 std::function (右值引用)
        void Bind( std::function<RT( Args... )>&& Function )
        {
            CallableObj = MakeCallableBase( std::move( Function ) );
        }

        // Bind a Function Object or Lambda
        // 绑定函数对象或 Lambda
        template <typename Callable> requires IsValidCallable<Callable, RT, Args...>
        void Bind( Callable&& callable )
        {
            CallableObj = MakeCallableBase<RT, Args...>( std::forward<Callable>( callable ) );
        }


    private:
        std::shared_ptr< ICallableBase<RT, Args...> > CallableObj;
    };

} // namespace NekiraDelegate



namespace NekiraDelegate
{
    // ============================================== Delegate Handle ============================================== //

    struct DelegateHandle
    {
        // Used to identify the owner context of the delegate, usually a multicast delegate
        // 用于标识委托的拥有者上下文,通常是多播委托
        void* OwnerContext;

        std::size_t Id;

        bool operator==( const DelegateHandle& Other ) const
        {
            return ( OwnerContext == Other.OwnerContext && Id == Other.Id );
        }
    };

} // namespace NekiraDelegate



namespace NekiraDelegate
{
    // ============================================== 多播委托 ============================================== //

    template <typename RT, typename... Args>
    class MulticastDelegate
    {
    public:
        // If the delegate has any bound delegates
        // 检查是否有绑定的委托
        bool IsBound() const
        {
            return !Delegates.empty();
        }

        // Get the number of bound delegates
        // 获取绑定的委托数量
        std::size_t GetBoundCount() const
        {
            return Delegates.size();
        }

        // Broadcast to all bound delegates
        // 广播到所有绑定的委托
        void Broadcast( Args... args )
        {
            // Clean up invalid delegates before broadcasting
            CleanupInvalidDelegates();

            for ( auto& delegatePair : Delegates )
            {
                if ( delegatePair.second.IsValid() )
                {
                    delegatePair.second.Invoke( std::forward<Args>( args )... );
                }
            }
        }

        // Remove a delegate by its handle
        // 通过句柄移除委托
        void Remove( const DelegateHandle& handle )
        {
            auto it = std::remove_if
            (
                Delegates.begin(), Delegates.end(),
                [ &handle ]( const DelegatePair& pair ) { return pair.first == handle; }
            );

            if ( it != Delegates.end() )
            {
                Delegates.erase( it, Delegates.end() );
            }
        }

        // Remove all delegates
        // 移除所有委托
        void RemoveAll()
        {
            Delegates.clear();
        }

        // Add a Normal Function
        // 添加普通函数
        DelegateHandle Add( RT( *FuncPtr )( Args... ) )
        {
            Delegate<RT, Args...> delegate;
            delegate.Bind( FuncPtr );
            DelegateHandle handle{ this, DelegateIDCounter + 1 };
            Delegates.emplace_back( handle, std::move( delegate ) );
            return handle;
        }

        // Add a Member Function
        // 添加成员函数
        template <typename ClassType>
        DelegateHandle Add( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) )
        {
            Delegate<RT, Args...> delegate;
            delegate.Bind( Object, FuncPtr );
            DelegateHandle handle{ this, DelegateIDCounter + 1 };
            Delegates.emplace_back( handle, std::move( delegate ) );
            return handle;
        }

        // Add a const Member Function (const Object)
        // 添加 const 成员函数 (const 对象)
        template <typename ClassType>
        DelegateHandle Add( std::shared_ptr<const ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) const )
        {
            Delegate<RT, Args...> delegate;
            delegate.Bind( Object, FuncPtr );
            DelegateHandle handle{ this, DelegateIDCounter + 1 };
            Delegates.emplace_back( handle, std::move( delegate ) );
            return handle;
        }

        // Add a const Member Function (non const Object)
        // 添加 const 成员函数 (非 const 对象)
        template <typename ClassType>
        DelegateHandle Add( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) const )
        {
            Delegate<RT, Args...> delegate;
            delegate.Bind( Object, FuncPtr );
            DelegateHandle handle{ this, DelegateIDCounter + 1 };
            Delegates.emplace_back( handle, std::move( delegate ) );
            return handle;
        }

        // Add a volatile Member Function (volatile Object)
        // 添加 volatile 成员函数 (volatile 对象)
        template <typename ClassType>
        DelegateHandle Add( std::shared_ptr<volatile ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) volatile )
        {
            Delegate<RT, Args...> delegate;
            delegate.Bind( Object, FuncPtr );
            DelegateHandle handle{ this, DelegateIDCounter + 1 };
            Delegates.emplace_back( handle, std::move( delegate ) );
            return handle;
        }

        // Add a volatile Member Function (non volatile Object)
        // 添加 volatile 成员函数 (非 volatile 对象)
        template <typename ClassType>
        DelegateHandle Add( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) volatile )
        {
            Delegate<RT, Args...> delegate;
            delegate.Bind( Object, FuncPtr );
            DelegateHandle handle{ this, DelegateIDCounter + 1 };
            Delegates.emplace_back( handle, std::move( delegate ) );
            return handle;
        }

        // Add a std::function (Left Reference)
        // 添加 std::function (左值引用)
        DelegateHandle Add( const std::function<RT( Args... )>& Function )
        {
            Delegate<RT, Args...> delegate;
            delegate.Bind( Function );
            DelegateHandle handle{ this, DelegateIDCounter + 1 };
            Delegates.emplace_back( handle, std::move( delegate ) );
            return handle;
        }

        // Add a std::function (Right Reference)
        // 添加 std::function (右值引用)
        DelegateHandle Add( std::function<RT( Args... )>&& Function )
        {
            Delegate<RT, Args...> delegate;
            delegate.Bind( std::move( Function ) );
            DelegateHandle handle{ this, DelegateIDCounter + 1 };
            Delegates.emplace_back( handle, std::move( delegate ) );
            return handle;
        }

        // Add a Function Object or Lambda
        // 添加函数对象或 Lambda
        template <typename Callable> requires IsValidCallable<Callable, RT, Args...>
        DelegateHandle Add( Callable&& callable )
        {
            Delegate<RT, Args...> delegate;
            delegate.Bind( std::forward<Callable>( callable ) );
            DelegateHandle handle{ this, DelegateIDCounter + 1 };
            Delegates.emplace_back( handle, std::move( delegate ) );
            return handle;
        }

    private:
        // Clean up Invalid Delegates
        // 清除无效的委托
        void CleanupInvalidDelegates()
        {
            auto it = std::remove_if
            (
                Delegates.begin(), Delegates.end(),
                [ ]( const DelegatePair& pair ) { return !pair.second.IsValid(); }
            );

            if ( it != Delegates.end() )
            {
                Delegates.erase( it, Delegates.end() );
            }
        }

    private:
        using DelegatePair = std::pair< DelegateHandle, Delegate<RT, Args...> >;
        std::vector < DelegatePair > Delegates;

        // Generate a new unique handle for the delegate
        // 用于生成新的唯一委托句柄
        std::size_t DelegateIDCounter = 0;

    };

} // namespace NekiraDelegate


// ================================================== Helper Macros ================================================== //

// Declare a Delegate with a specific name and signature
// 声明一个具有特定名称和签名的单播委托
#ifndef DECLARE_DELEGATE
#define DECLARE_DELEGATE( DelegateName, ReturnType, ... ) \
    using DelegateName = NekiraDelegate::Delegate<ReturnType, __VA_ARGS__>;
#endif // DECLARE_DELEGATE

// Declare a Multicast Delegate with a specific name and signature
// 声明一个具有特定名称和签名的多播委托
#ifndef DECLARE_MULTICAST_DELEGATE
#define DECLARE_MULTICAST_DELEGATE( DelegateName, ReturnType, ... ) \
    using DelegateName = NekiraDelegate::MulticastDelegate<ReturnType, __VA_ARGS__>;
#endif // DECLARE_MULTICAST_DELEGATE


