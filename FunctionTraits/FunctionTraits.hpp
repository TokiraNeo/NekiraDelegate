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

#include <tuple>
#include <functional>

namespace NekiraDelegate
{
    // 函数萃取器的公共部分
    template <typename Callable>
    struct Function_Traits_Base
    {
        using FuncSignature = void;

        using ReturnType = void;
        using ObjectType = void;
        using ArgsTuple = std::tuple<>;
    };

    // 基础的函数萃取器
    template <typename Callable, typename = void>
    struct Function_Traits : Function_Traits_Base<Callable>
    {
    };

    // 特化：普通函数
    template <typename RT, typename... Args>
    struct Function_Traits<RT( * )( Args... ), void> : Function_Traits_Base<RT( * )( Args... )>
    {
        using FuncSignature = RT( * )( Args... );
        using ReturnType = RT;
        using ArgsTuple = std::tuple<Args...>;
    };

    // 特化：成员函数
    template <typename ClassType, typename RT, typename... Args>
    struct Function_Traits<RT( ClassType::* )( Args... ), void> : Function_Traits_Base<RT( ClassType::* )( Args... )>
    {
        using FuncSignature = RT( ClassType::* )( Args... );
        using ReturnType = RT;
        using ObjectType = ClassType;
        using ArgsTuple = std::tuple<Args...>;
    };

    // 特化：const成员函数
    template <typename ClassType, typename RT, typename... Args>
    struct Function_Traits<RT( ClassType::* )( Args... ) const, void> : Function_Traits_Base<RT( ClassType::* )( Args... ) const>
    {
        using FuncSignature = RT( ClassType::* )( Args... ) const;
        using ReturnType = RT;
        using ObjectType = ClassType;
        using ArgsTuple = std::tuple<Args...>;
    };

    // 特化：volatile 成员函数
    template <typename ClassType, typename RT, typename... Args>
    struct Function_Traits<RT( ClassType::* )( Args... ) volatile, void> : Function_Traits_Base<RT( ClassType::* )( Args... ) volatile>
    {
        using FuncSignature = RT( ClassType::* )( Args... ) volatile;
        using ReturnType = RT;
        using ObjectType = ClassType;
        using ArgsTuple = std::tuple<Args...>;
    };

    // 特化：函数对象、lambda表达式
    template <typename Callable>
    struct Function_Traits<Callable, std::enable_if_t< std::is_class_v<Callable> > > : Function_Traits<decltype( &Callable::operator() )>
    {
    };

    // 特化：std::function
    template <typename RT, typename... Args>
    struct Function_Traits<std::function<RT( Args... )>, void> : Function_Traits<RT( * )( Args... )>
    {
    };


} // namespace NekiraDelegate




namespace NekiraDelegate
{
    // =============================================== Tuple展开 =============================================== //
    // 将Tuple展开成参数包

    // TupleToArgs_Impl：默认实现
    template <typename Tuple, typename... Indexes>
    struct TupleToArgs_Impl
    {
    };

    // TupleToArgs_Impl：特化实现，接收一个索引序列
    template <typename Tuple, std::size_t... Indexes>
    struct TupleToArgs_Impl < Tuple, std::index_sequence<Indexes...> >
    {
        using Type = ( std::tuple_element_t<Indexes, Tuple>... );
    };

    template <typename Tuple>
    using TupleToArgs = typename TupleToArgs_Impl< Tuple, std::make_index_sequence< std::tuple_size_v<Tuple> > >::Type;

    // [INFO] std::make_index_sequence<3> ，相当于生成一个std::index_sequence<0, 1, 2>
} // namespace NekiraDelegate



namespace NekiraDelegate
{
    // ================================================= 便捷别名和常量 ================================================= //

    // 函数指针类型
    template <typename T>
    using Func_Traits_FuncPtrType = typename Function_Traits<T>::FuncSignature;

    // 返回值类型
    template <typename T>
    using Func_Traits_ReturnType = typename Function_Traits<T>::ReturnType;

    // 参数类型元组
    template <typename T>
    using Func_Traits_ArgsTuple = typename Function_Traits<T>::ArgsTuple;

    // 参数类型（展开后的参数包）
    template <typename T>
    using Func_Traits_ArgsPack = TupleToArgs< typename Function_Traits<T>::ArgsTuple >;

    // 对象类型（成员函数）
    template <typename T>
    using Func_Traits_ObjectType = typename Function_Traits<T>::ObjectType;

} // namespace NekiraDelegate


