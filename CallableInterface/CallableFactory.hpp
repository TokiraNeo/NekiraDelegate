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

#include <type_traits>
#include <concepts>
#include "CallableInterface.hpp"


namespace NekiraDelegate
{
    // =============================================== concept =============================================== //

    // Check if the Callable is valid
    // 检查函数对象、Lambda是否符合条件
    template <typename Callable, typename RT, typename... Args>
    concept IsValidCallable = std::is_class_v< std::remove_reference_t<Callable> > &&
        std::is_invocable_r_v<RT, Callable, Args...>;

}



namespace NekiraDelegate
{
    // ============================================== Helper Function ============================================== //

    // Covert `shared_ptr<T>` To `shared_ptr<const T>`
    // 将shared_ptr<T> 转换为 shared_ptr<const T>
    template <typename T>
    static std::shared_ptr<const T> ConstSharedPtr( const std::shared_ptr<T>& ptr )
    {
        return std::static_pointer_cast< const T >( ptr );
    }

    // Covert `shared_ptr<T>` To `shared_ptr<volatile T>`
    // 将shared_ptr<T> 转换为 shared_ptr<volatile T>
    template <typename T>
    static std::shared_ptr<volatile T> VolatileSharedPtr( const std::shared_ptr<T>& ptr )
    {
        return std::static_pointer_cast< volatile T >( ptr );
    }

}



namespace NekiraDelegate
{
    // =============================================== Factory Function =============================================== //
    // Factory Method to create ICallableBase instances
    // 创建ICallableBase 实例的静态工厂方法

    // Normal Function
    // 普通函数
    template < typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( RT( *FuncPtr )( Args... ) )
    {
        return std::make_shared< ICallable<RT( * )( Args... )> >( FuncPtr );
    }

    // Member Function
    // 成员函数
    template < typename ClassType, typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) )
    {
        return std::make_shared < ICallable< RT( ClassType::* )( Args... ) > >( Object, FuncPtr );
    }

    // const Member Function (const Object)
    // const 成员函数 (const 对象)
    template < typename ClassType, typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( std::shared_ptr<const ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) const )
    {
        return std::make_shared < ICallable< RT( ClassType::* )( Args... ) const > >( Object, FuncPtr );
    }

    // const Member Function (non const Object)
    // const 成员函数 (非 const 对象)
    template < typename ClassType, typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) const )
    {
        return std::make_shared < ICallable< RT( ClassType::* )( Args... ) const > >( ConstSharedPtr( Object ), FuncPtr );
    }

    // volatile Member Funciton (volatile Object)
    // volatile 成员函数 (volatile 对象)
    template < typename ClassType, typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( std::shared_ptr<volatile ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) volatile )
    {
        return std::make_shared < ICallable< RT( ClassType::* )( Args... ) volatile > >( Object, FuncPtr );
    }

    // volatile Member Funciton (non volatile Object)
    // volatile 成员函数 (非 volatile 对象)
    template < typename ClassType, typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) volatile )
    {
        return std::make_shared < ICallable< RT( ClassType::* )( Args... ) volatile > >( VolatileSharedPtr( Object ), FuncPtr );
    }

    // std::function (Left Reference)
    // std::function (左值引用)
    template < typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( const std::function< RT( Args... ) >& Function )
    {
        return std::make_shared < ICallable < std::function<RT( Args... )> > >( Function );
    }

    // std::function (Right Reference)
    // std::function (右值引用)
    template < typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( std::function< RT( Args... ) >&& Function )
    {
        return std::make_shared < ICallable < std::function<RT( Args... )> > >( std::move( Function ) );
    }

    // Function Object、Lambda
    // 函数对象、Lambda
    template <typename RT, typename... Args, typename Callable>
        requires IsValidCallable<Callable, RT, Args...>
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( Callable&& callable )
    {
        using CallableType = std::remove_reference_t<Callable>;
        using WrapperType = ICallableWrapper<CallableType, RT, Args...>;

        return std::make_shared< WrapperType >( std::forward<Callable>( callable ) );
    }

} // namespace NekiraDelegate

