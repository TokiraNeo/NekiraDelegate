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
        bool IsValid() const
        {
            return ( CallableObj != nullptr );
        }

        // Invoke Delegate
        RT Invoke( Args... args )
        {
            return ( CallableObj ? CallableObj->Invoke( std::forward<Args>( args )... ) : RT{} );

        }

        // Bind a Global Function
        void Bind( RT( *FuncPtr )( Args... ) )
        {


            CallableObj = MakeCallableBase( FuncPtr );
        }

        // Bind a Member Function
        template <typename ClassType>
        void Bind( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) )
        {
            CallableObj = MakeCallableBase( Object, FuncPtr );
        }

        // Bind a const Member Function (const Object)
        template <typename ClassType>
        void Bind( std::shared_ptr<const ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) const )
        {
            CallableObj = MakeCallableBase( Object, FuncPtr );
        }

        // Bind a const Member Function (non const Object)
        template <typename ClassType>
        void Bind( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) const )
        {
            CallableObj = MakeCallableBase( Object, FuncPtr );
        }

        // Bind a volatile Member Function (volatile Object)
        template <typename ClassType>
        void Bind( std::shared_ptr<volatile ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) volatile )
        {
            CallableObj = MakeCallableBase( Object, FuncPtr );
        }

        // Bind a volatile Member Function (non volatile Object)
        template <typename ClassType>
        void Bind( std::shared_ptr<ClassType> Object, RT( ClassType::* FuncPtr )( Args... ) volatile )
        {
            CallableObj = MakeCallableBase( Object, FuncPtr );
        }

        // Bind a std::function (Left Reference)
        void Bind( const std::function<RT( Args... )>& Function )
        {
            CallableObj = MakeCallableBase( Function );
        }

        // Bind a std::function (Right Reference)
        void Bind( std::function<RT( Args... )>&& Function )
        {
            CallableObj = MakeCallableBase( std::move( Function ) );
        }

        // Bind a Function Object or Lambda
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
        void* OwnerContext; // 用于标识委托的拥有者上下文,通常是多播委托

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
        // Broadcast to all bound delegates
        void Broadcast( Args... args )
        {
            for ( auto& delegatePair : Delegates )
            {
                if ( delegatePair.second.IsValid() )
                {
                    delegatePair.second.Invoke( std::forward<Args>( args )... );
                }
            }
        }

        // Remove a delegate by its handle
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


        // Add a Normal Function
        DelegateHandle Add( RT( *FuncPtr )( Args... ) )
        {
            Delegate<RT, Args...> delegate;
            delegate.Bind( FuncPtr );
            DelegateHandle handle{ this, DelegateIDCounter + 1 };
            Delegates.emplace_back( handle, std::move( delegate ) );
            return handle;
        }

        // Add a Member Function
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
        DelegateHandle Add( const std::function<RT( Args... )>& Function )
        {
            Delegate<RT, Args...> delegate;
            delegate.Bind( Function );
            DelegateHandle handle{ this, DelegateIDCounter + 1 };
            Delegates.emplace_back( handle, std::move( delegate ) );
            return handle;
        }

        // Add a std::function (Right Reference)
        DelegateHandle Add( std::function<RT( Args... )>&& Function )
        {
            Delegate<RT, Args...> delegate;
            delegate.Bind( std::move( Function ) );
            DelegateHandle handle{ this, DelegateIDCounter + 1 };
            Delegates.emplace_back( handle, std::move( delegate ) );
            return handle;
        }

        // Add a Function Object or Lambda
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
        using DelegatePair = std::pair< DelegateHandle, Delegate<RT, Args...> >;
        std::vector < DelegatePair > Delegates;

        // Generate a new unique handle for the delegate
        std::size_t DelegateIDCounter = 0;

    };

} // namespace NekiraDelegate


// ================================================== Helper Macros ================================================== //

// Declare a Delegate with a specific name and signature
#ifndef DECLARE_DELEGATE
#define DECLARE_DELEGATE( DelegateName, ReturnType, ... ) \
    using DelegateName = NekiraDelegate::Delegate<ReturnType, __VA_ARGS__>;
#endif // DECLARE_DELEGATE

#ifndef DECLARE_MULTICAST_DELEGATE
#define DECLARE_MULTICAST_DELEGATE( DelegateName, ReturnType, ... ) \
    using DelegateName = NekiraDelegate::MulticastDelegate<ReturnType, __VA_ARGS__>;
#endif // DECLARE_MULTICAST_DELEGATE


