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

#include <functional>
#include <memory>
#include "../FunctionTraits/FunctionTraits.hpp"

namespace NekiraDelegate
{
    // 可调用接口的基类
    template <typename RT, typename... Args>
    struct ICallableBase
    {
        virtual ~ICallableBase() = default;

        // 统一的调用接口
        virtual RT Invoke( Args... args ) = 0;
    };
} // namespace NekiraDelegate

namespace NekiraDelegate
{
    // 用于将Tuple展开，构造ICallable<RT, Args...>类型
    template <typename RT, typename Tuple>
    struct ICallableBaseFromTuple;
}

namespace NekiraDelegate
{

    // 可调用接口的默认实现
    template <typename Callable, typename = void>
    struct ICallable
    {
    };

    // 特化：普通函数指针
    template <typename RT, typename... Args>
    struct ICallable<RT( * )( Args... ), void> : ICallableBase<RT, Args...>
    {
        using FuncSignature = RT( * )( Args... );

        ICallable( FuncSignature funcPtr )
            : FuncPtr( funcPtr )
        {
        }

        RT Invoke( Args... args ) override
        {
            return FuncPtr( std::forward<Args>( args )... );
        }

    private:
        FuncSignature FuncPtr;
    };

    // 特化：成员函数指针
    template <typename ClassType, typename RT, typename... Args>
    struct ICallable<RT( ClassType::* )( Args... ), void> : ICallableBase<RT, Args...>
    {
        using FuncSignature = RT( ClassType::* )( Args... );

        ICallable( ClassType* objPtr, FuncSignature funcPtr )
            : ObjectPtr( objPtr ), FuncPtr( funcPtr )
        {
            static_assert( std::is_class_v<ClassType>, "ClassType must be a valid class" );

            static_assert( std::is_member_function_pointer_v<FuncSignature>,
                "FuncSignature must be a valid member function pointer" );
        }

        RT Invoke( Args... args ) override
        {
            return ( ObjectPtr->*FuncPtr )( std::forward<Args>( args )... );
        }

    private:
        ClassType* ObjectPtr;
        FuncSignature FuncPtr;
    };

    // 特化：const成员函数指针
    template <typename ClassType, typename RT, typename... Args>
    struct ICallable<RT( ClassType::* )( Args... ) const, void> : ICallableBase<RT, Args...>
    {
        using FuncSignature = RT( ClassType::* )( Args... ) const;

        ICallable( const ClassType* objPtr, FuncSignature funcPtr )
            : ObjectPtr( objPtr ), FuncPtr( funcPtr )
        {
            static_assert( std::is_class_v<ClassType>, "ClassType must be a valid class" );

            static_assert( std::is_member_function_pointer_v<FuncSignature>,
                "FuncSignature must be a valid member function pointer" );
        }

        RT Invoke( Args... args ) override
        {
            return ( ObjectPtr->*FuncPtr )( std::forward<Args>( args )... );
        }

    private:
        const ClassType* ObjectPtr;
        FuncSignature FuncPtr;
    };

    // 特化：volatile成员函数
    template <typename ClassType, typename RT, typename... Args>
    struct ICallable<RT( ClassType::* )( Args... ) volatile, void> : ICallableBase<RT, Args...>
    {
        using FuncSignature = RT( ClassType::* )( Args... ) volatile;

        ICallable( volatile ClassType* objPtr, FuncSignature funcPtr )
            : ObjectPtr( objPtr ), FuncPtr( funcPtr )
        {
            static_assert( std::is_class_v<ClassType>, "ClassType must be a valid class" );

            static_assert( std::is_member_function_pointer_v<FuncSignature>,
                "FuncSignature must be a valid member function pointer" );
        }

        RT Invoke( Args... args ) override
        {
            return ( ObjectPtr->*FuncPtr )( std::forward<Args>( args )... );
        }

    private:
        volatile ClassType* ObjectPtr;
        FuncSignature FuncPtr;
    };

    // 特化：std::function
    template <typename RT, typename... Args>
    struct ICallable<std::function<RT( Args... )>, void> : ICallableBase< RT, Args... >
    {
        using FuncSignature = std::function<RT( Args... )>;

        ICallable( FuncSignature function )
            : Function( std::move( function ) )
        {
        }

        RT Invoke( Args... args ) override
        {
            return Function( std::forward<Args>( args )... );
        }

    private:
        FuncSignature Function;
    };

} // namespace NekiraDelegate



namespace NekiraDelegate
{
    // ============================================ 支持函数对象、Lambda表达式 ============================================ //
    template <typename Callable, typename CallableSignature = decltype( &Callable::operator() ),
        typename = std::enable_if_t< std::is_class_v<Callable> > >
    struct ICallableWrapper
    {
    };

    template <typename ClassType, typename RT, typename... Args>
    struct ICallableWrapper< ClassType, RT( ClassType::* )( Args... ) > : ICallableBase<RT, Args...>
    {
        ICallableWrapper( ClassType Obj )
            : CallableObj( Obj )
        {
            static_assert( std::is_class_v<ClassType>, "ClassType must be a valid class" );
        }

        RT Invoke( Args... args ) override
        {
            CallableObj( std::forward<Args>( args )... );
        }
    private:
        ClassType CallableObj;
    };

    template <typename ClassType, typename RT, typename... Args>
    struct ICallableWrapper< ClassType, RT( ClassType::* )( Args... ) const> : ICallableBase<RT, Args...>
    {
        ICallableWrapper( ClassType Obj )
            : CallableObj( Obj )
        {
            static_assert( std::is_class_v<ClassType>, "ClassType must be a valid class" );
        }

        RT Invoke( Args... args ) override
        {
            CallableObj( std::forward<Args>( args )... );
        }
    private:
        ClassType CallableObj;
    };

    template <typename ClassType, typename RT, typename... Args>
    struct ICallableWrapper< ClassType, RT( ClassType::* )( Args... ) volatile> : ICallableBase<RT, Args...>
    {
        ICallableWrapper( ClassType Obj )
            : CallableObj( Obj )
        {
            static_assert( std::is_class_v<ClassType>, "ClassType must be a valid class" );
        }

        RT Invoke( Args... args ) override
        {
            CallableObj( std::forward<Args>( args )... );
        }
    private:
        ClassType CallableObj;
    };


} // namespace NekiraDelegate


namespace NekiraDelegate
{
    // =============================================== 辅助函数 =============================================== //
    // 创建ICallableBase 实例的静态方法

    // Normal Function
    template < typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( RT( *FuncPtr )( Args... ) )
    {
        return std::make_shared< ICallable<RT( * )( Args... )> >( FuncPtr );
    }

    // Member Function
    template < typename ClassType, typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( ClassType* Object, RT( ClassType::* FuncPtr )( Args... ) )
    {
        return std::make_shared < ICallable< RT( ClassType::* )( Args... ) > >( Object, FuncPtr );
    }

    // const Member Function
    template < typename ClassType, typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( const ClassType* Object, RT( ClassType::* FuncPtr )( Args... ) const )
    {
        return std::make_shared < ICallable< RT( ClassType::* )( Args... ) const > >( Object, FuncPtr );
    }

    // volatile Member Funciton
    template < typename ClassType, typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( volatile ClassType* Object, RT( ClassType::* FuncPtr )( Args... ) volatile )
    {
        return std::make_shared < ICallable< RT( ClassType::* )( Args... ) volatile > >( Object, FuncPtr );
    }

    // std::function
    template < typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( std::function< RT( Args... ) > Function )
    {
        return std::make_shared < ICallable < std::function<RT( Args... )> > >( std::move( Function ) );
    }

    // Function Object、Lambda
    template <typename Callable, typename = std::enable_if_t< std::is_class_v<Callable> > >
    static auto MakeCallableBase( Callable callable )
    {
        return std::make_shared< ICallableWrapper<Callable> >( std::move( callable ) );
    }

} // namespace NekiraDelegate