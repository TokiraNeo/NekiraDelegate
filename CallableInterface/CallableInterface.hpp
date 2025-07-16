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


namespace NekiraDelegate
{
    // 可调用接口的基类
    template <typename RT, typename... Args>
    struct ICallableBase
    {
        virtual ~ICallableBase() = default;

        // 统一的调用接口
        virtual RT Invoke( Args... args ) = 0;

        // 检查是否有效
        virtual bool IsValid() const = 0;
    };
} // namespace NekiraDelegate



namespace NekiraDelegate
{

    // 可调用接口的默认实现
    template <typename Callable>
    struct ICallable
    {
    };

    // 特化：普通函数指针
    template <typename RT, typename... Args>
    struct ICallable<RT( * )( Args... )> : ICallableBase<RT, Args...>
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

        virtual bool IsValid() const override
        {
            return FuncPtr != nullptr;
        }
    private:
        FuncSignature FuncPtr;
    };

    // 特化：成员函数指针
    template <typename ClassType, typename RT, typename... Args>
    struct ICallable<RT( ClassType::* )( Args... )> : ICallableBase<RT, Args...>
    {
        using FuncSignature = RT( ClassType::* )( Args... );

        ICallable( std::shared_ptr<ClassType> objPtr, FuncSignature funcPtr )
            : ObjectPtr( objPtr ), FuncPtr( funcPtr )
        {
        }

        RT Invoke( Args... args ) override
        {
            if ( auto obj = ObjectPtr.lock() )
            {
                return ( obj.get()->*FuncPtr )( std::forward<Args>( args )... );
            }
            return RT{};
        }

        virtual bool IsValid() const override
        {
            return !ObjectPtr.expired() && FuncPtr != nullptr;
        }
    private:
        std::weak_ptr<ClassType> ObjectPtr;
        FuncSignature FuncPtr;
    };

    // 特化：const成员函数指针
    template <typename ClassType, typename RT, typename... Args>
    struct ICallable<RT( ClassType::* )( Args... ) const> : ICallableBase<RT, Args...>
    {
        using FuncSignature = RT( ClassType::* )( Args... ) const;


        ICallable( std::shared_ptr<const ClassType> objPtr, FuncSignature funcPtr )
            : ObjectPtr( objPtr ), FuncPtr( funcPtr )
        {
        }

        RT Invoke( Args... args ) override
        {
            if ( auto obj = ObjectPtr.lock() )
            {
                return ( obj.get()->*FuncPtr )( std::forward<Args>( args )... );
            }
            return RT{};
        }

        virtual bool IsValid() const override
        {
            return !ObjectPtr.expired() && FuncPtr != nullptr;
        }
    private:
        std::weak_ptr<const ClassType> ObjectPtr;
        FuncSignature FuncPtr;
    };

    // 特化：volatile成员函数
    template <typename ClassType, typename RT, typename... Args>
    struct ICallable<RT( ClassType::* )( Args... ) volatile> : ICallableBase<RT, Args...>
    {
        using FuncSignature = RT( ClassType::* )( Args... ) volatile;


        ICallable( std::shared_ptr<volatile ClassType> objPtr, FuncSignature funcPtr )
            : ObjectPtr( objPtr ), FuncPtr( funcPtr )
        {
        }

        RT Invoke( Args... args ) override
        {
            if ( auto obj = ObjectPtr.lock() )
            {
                return ( obj.get()->*FuncPtr )( std::forward<Args>( args )... );
            }
            return RT{};
        }

        virtual bool IsValid() const override
        {
            return !ObjectPtr.expired() && FuncPtr != nullptr;
        }
    private:
        std::weak_ptr<volatile ClassType> ObjectPtr;
        FuncSignature FuncPtr;
    };

    // 特化：std::function
    template <typename RT, typename... Args>
    struct ICallable<std::function<RT( Args... )>> : ICallableBase< RT, Args... >
    {
        using FuncSignature = std::function<RT( Args... )>;

        ICallable( const FuncSignature& function )
            : Function( function )
        {
        }

        ICallable( FuncSignature&& function )
            : Function( std::move( function ) )
        {
        }

        RT Invoke( Args... args ) override
        {
            return Function( std::forward<Args>( args )... );
        }

        virtual bool IsValid() const override
        {
            return static_cast< bool >( Function );
        }

    private:
        FuncSignature Function;
    };

} // namespace NekiraDelegate



namespace NekiraDelegate
{
    // ============================================ 支持函数对象、Lambda表达式 ============================================ //
    template <typename Callable, typename CallableSignature = decltype( &Callable::operator() )>
        requires std::is_class_v<Callable>
    struct ICallableWrapper
    {
    };

    template <typename ClassType, typename RT, typename... Args>
    struct ICallableWrapper< ClassType, RT( ClassType::* )( Args... ) > : ICallableBase<RT, Args...>
    {
        ICallableWrapper( const ClassType& Obj )
            : CallableObj( Obj )
        {
        }

        ICallableWrapper( ClassType&& Obj )
            : CallableObj( std::move( Obj ) )
        {
        }

        RT Invoke( Args... args ) override
        {
            CallableObj( std::forward<Args>( args )... );
        }

        virtual bool IsValid() const override
        {
            return true;
        }

    private:
        ClassType CallableObj;
    };

    template <typename ClassType, typename RT, typename... Args>
    struct ICallableWrapper< ClassType, RT( ClassType::* )( Args... ) const> : ICallableBase<RT, Args...>
    {
        ICallableWrapper( const ClassType& Obj )
            : CallableObj( Obj )
        {
        }

        ICallableWrapper( ClassType&& Obj )
            : CallableObj( std::move( Obj ) )
        {
        }

        RT Invoke( Args... args ) override
        {
            CallableObj( std::forward<Args>( args )... );
        }

        virtual bool IsValid() const override
        {
            return true;
        }
    private:
        ClassType CallableObj;
    };

    template <typename ClassType, typename RT, typename... Args>
    struct ICallableWrapper< ClassType, RT( ClassType::* )( Args... ) volatile> : ICallableBase<RT, Args...>
    {
        ICallableWrapper( const ClassType& Obj )
            : CallableObj( Obj )
        {
        }

        ICallableWrapper( ClassType&& Obj )
            : CallableObj( std::move( Obj ) )
        {
        }

        RT Invoke( Args... args ) override
        {
            CallableObj( std::forward<Args>( args )... );
        }

        virtual bool IsValid() const override
        {
            return true;
        }
    private:
        ClassType CallableObj;
    };


} // namespace NekiraDelegate



namespace NekiraDelegate
{
    // ============================================== 辅助函数 ============================================== //

    // 将shared_ptr<T> 转换为 shared_ptr<const T>
    template <typename T>
    static std::shared_ptr<const T> ConstSharedPtr( const std::shared_ptr<T>& ptr )
    {
        return std::static_pointer_cast< const T >( ptr );
    }

    // 将shared_ptr<T> 转换为 shared_ptr<volatile T>
    template <typename T>
    static std::shared_ptr<volatile T> VolatileSharedPtr( const std::shared_ptr<T>& ptr )
    {
        return std::static_pointer_cast< volatile T >( ptr );
    }

}

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
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) )
    {
        return std::make_shared < ICallable< RT( ClassType::* )( Args... ) > >( Object, FuncPtr );
    }

    // const Member Function (const Object)
    template < typename ClassType, typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( std::shared_ptr<const ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) const )
    {
        return std::make_shared < ICallable< RT( ClassType::* )( Args... ) const > >( Object, FuncPtr );
    }

    // const Member Function (non const Object)
    template < typename ClassType, typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) const )
    {
        return std::make_shared < ICallable< RT( ClassType::* )( Args... ) const > >( ConstSharedPtr( Object ), FuncPtr );
    }

    // volatile Member Funciton (volatile Object)
    template < typename ClassType, typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( std::shared_ptr<volatile ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) volatile )
    {
        return std::make_shared < ICallable< RT( ClassType::* )( Args... ) volatile > >( Object, FuncPtr );
    }

    // volatile Member Funciton (non volatile Object)
    template < typename ClassType, typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) volatile )
    {
        return std::make_shared < ICallable< RT( ClassType::* )( Args... ) volatile > >( VolatileSharedPtr( Object ), FuncPtr );
    }

    // std::function (Left Reference)
    template < typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( const std::function< RT( Args... ) >& Function )
    {
        return std::make_shared < ICallable < std::function<RT( Args... )> > >( Function );
    }

    // std::function (Right Reference)
    template < typename RT, typename... Args >
    static std::shared_ptr< ICallableBase<RT, Args...> > MakeCallableBase( std::function< RT( Args... ) >&& Function )
    {
        return std::make_shared < ICallable < std::function<RT( Args... )> > >( std::move( Function ) );
    }

    // Function Object、Lambda
    template <typename Callable> requires std::is_class_v< std::remove_reference_t<Callable> >
    static auto MakeCallableBase( Callable&& callable )
    {
        using RawType = std::remove_reference_t<Callable>;
        using ICallableWrapperType = ICallableWrapper<RawType>;

        return std::make_shared < ICallableWrapperType >( std::forward<Callable>( callable ) );
    }

} // namespace NekiraDelegate

