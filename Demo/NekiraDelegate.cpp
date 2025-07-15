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
    std::cout << "GlobalFunc called with: " << a << ", " << str << "\n";
}

auto LambdaFunc = [ ]( float a, const std::string& str )
    {
        std::cout << "LambdaFunc called with: " << a << ", " << str << "\n";
    };

class TestClass
{
public:
    void Func( float a, const std::string& str )
    {
        std::cout << "TestClass::Func called with: " << a << ", " << str << "\n";
    }

    void ConstFunc( float a, const std::string& str ) const
    {
        std::cout << "TestClass::ConstFunc called with: " << a << ", " << str << "\n";
    }

    void VolatileFunc( float a, const std::string& str ) volatile
    {
        std::cout << "TestClass::VolatileFunc called with: " << a << ", " << str << "\n";
    }
};

struct FuncObject
{
    void operator()( float x, const std::string& str ) const
    {
        std::cout << "FuncObject called with: " << x << ", " << str << "\n";
    }
};

std::function<void( float, const std::string& )> StdFunc = [ ]( float x, const std::string& str )
    {
        std::cout << "StdFunc called with: " << x << ", " << str << "\n";
    };


// 声明多播委托类型
DECLARE_MULTICAST_DELEGATE( MultiSignature, void, float, const std::string& )

// 声明单播委托类型
DECLARE_DELEGATE( SingleSignature, void, float, const std::string& )

int main()
{
    TestClass ClassObj;
    FuncObject FuncObj;

    // 创建单播委托实例
    SingleSignature SingleDelegate;
    std::cout << "单播委托测试：\n";
    // 普通函数绑定
    SingleDelegate.Bind( GlobalFunc );
    SingleDelegate.Invoke( 1.0f, "Tokira" );
    // 成员函数绑定
    SingleDelegate.Bind( &ClassObj, &TestClass::ConstFunc );
    SingleDelegate.Invoke( 2.0f, "Tokira" );
    // Lambda绑定
    SingleDelegate.Bind( LambdaFunc );
    SingleDelegate.Invoke( 3.0f, "Tokira" );

    // 创建多播委托实例
    MultiSignature MultiDelegate;
    std::cout << "\n";
    std::cout << "多播委托测试：\n";
    // 添加普通函数
    MultiDelegate.Add( GlobalFunc );
    // 添加成员函数
    MultiDelegate.Add( &ClassObj, &TestClass::Func );
    MultiDelegate.Add( &ClassObj, &TestClass::VolatileFunc );
    MultiDelegate.Add( &ClassObj, &TestClass::ConstFunc );
    // 添加 Lambda
    MultiDelegate.Add( LambdaFunc );
    // 添加函数对象
    MultiDelegate.Add( FuncObj );
    // 添加 std::function
    MultiDelegate.Add( StdFunc );

    // 广播调用
    MultiDelegate.Broadcast( 10.0f, "Nekira" );

    return 0;
}

