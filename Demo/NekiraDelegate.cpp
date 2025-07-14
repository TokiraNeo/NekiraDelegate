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


#include <iostream>
#include <string>
#include "../Delegate/Delegate.hpp"

using namespace NekiraDelegate;


static void GlobalFunc( float a, const std::string& str )
{
    std::cout << "GlobalFunc called with: " << a << ", " << str << std::endl;
}

auto LambdaFunc = [ ]( float a, const std::string& str )
    {
        std::cout << "LambdaFunc called with: " << a << ", " << str << std::endl;
    };

class TestClass
{
public:
    void Func( float a, const std::string& str )
    {
        std::cout << "TestClass::Func called with: " << a << ", " << str << std::endl;
    }

    void ConstFunc( float a, const std::string& str ) const
    {
        std::cout << "TestClass::ConstFunc called with: " << a << ", " << str << std::endl;
    }

    void VolatileFunc( float a, const std::string& str ) volatile
    {
        std::cout << "TestClass::VolatileFunc called with: " << a << ", " << str << std::endl;
    }
};

struct FuncObject
{
    void operator()( float x, const std::string& str ) const
    {
        std::cout << "FuncObject called with: " << x << ", " << str << std::endl;
    }
};

std::function<void( float, const std::string& )> StdFunc = [ ]( float x, const std::string& str )
    {
        std::cout << "StdFunc called with: " << x << ", " << str << std::endl;

    };


DECLARE_MULTICAST_DELEGATE( MultiSignature, void, float, const std::string& );




int main()
{
    TestClass ClassObj;
    FuncObject FuncObj;


    MultiSignature MultiDelegate;

    // 普通函数绑定
    MultiDelegate.Add( GlobalFunc );

    // Lambda 绑定
    MultiDelegate.Add( LambdaFunc );

    // 成员函数绑定
    MultiDelegate.Add( &ClassObj, &TestClass::Func );
    MultiDelegate.Add( &ClassObj, &TestClass::ConstFunc );
    MultiDelegate.Add( &ClassObj, &TestClass::VolatileFunc );

    // 函数对象绑定
    MultiDelegate.Add( FuncObj );

    // std::function 绑定
    MultiDelegate.Add( StdFunc );

    // 广播调用
    MultiDelegate.Broadcast( 35.2f, { "Tokira" } );

    return 0;
}