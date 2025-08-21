## Code

```c++
//@[INFO] Declare the MultiDelegate Type
// NEKIRA_MULTI_DELEGATE(DelegateTypeName, ...)
NEKIRA_MULTI_DELEGATE(MultiSignature, int, float)

auto Lambda = [](int a, float b) { std::cout << "Lambda called with int: " << a << " and float: " << b << std::endl; };

auto intLambda = [](int a, float b) -> int
{
    std::cout << "Lambda called with int: " << a << " and float: " << b << std::endl;
    return a;
};

void FreeFunction(int a, float b)
{
    std::cout << "FreeFunction called with int: " << a << " and float: " << b << std::endl;
}

//@[INFO] For MemberFunction, the object should driven form NekiraDelegate::IConnectionInterface
struct SampleStruct : public NekiraDelegate::IConnectionInterface
{
    void Func(int a, float b)
    {
        std::cout << "SampleStruct Func called with int: " << a << " and float: " << b << std::endl;
    }

    void ConstFunc(int a, float b) const
    {
        std::cout << "SampleStruct ConstFunc called with int: " << a << " and float: " << b << std::endl;
    }

    void TemporaryFunc(int a, float b)
    {
        std::cout << "SampleStruct TemporaryFunc called with int: " << a << " and float: " << b << std::endl;
    }
};

struct FuncObject
{
    void operator()(int a, float b)
    {
        std::cout << "FuncObject called with int: " << a << " and float: " << b << std::endl;
    }
};

struct RetObject
{
    int operator()(int a, float b)
    {
        std::cout << "RetObject called with int: " << a << " and float: " << b << std::endl;
        return a;
    }
};

int main()
{
    MultiSignature Multidelegate;

    // Lambda and FunctionObject
    FuncObject Obj1;
    RetObject  Obj2;
    Multidelegate.BindFunctionObject(Lambda);
    Multidelegate.BindFunctionObject(intLambda);
    Multidelegate.BindFunctionObject(Obj1);
    Multidelegate.BindFunctionObject(Obj2);

    // Free function
    Multidelegate.BindFunction(FreeFunction);

    // Member function
    SampleStruct sample;
    Multidelegate.BindMemberFunction(&sample, &SampleStruct::Func);
    Multidelegate.BindMemberFunction(&sample, &SampleStruct::ConstFunc);

    // Lifetime test
    {
        SampleStruct sample2;
        //@[INFO] This Binding will not be called as sample2 goes out of scope becoming destroyed
        Multidelegate.BindMemberFunction(&sample2, &SampleStruct::TemporaryFunc);
    }

    // Call the delegate
    Multidelegate.Invoke(15, 9.52f);

    return 0;
}
```

---

## Output

```cmd
Lambda called with int: 15 and float: 9.52
Lambda called with int: 15 and float: 9.52
FuncObject called with int: 15 and float: 9.52
RetObject called with int: 15 and float: 9.52
FreeFunction called with int: 15 and float: 9.52
SampleStruct Func called with int: 15 and float: 9.52
SampleStruct ConstFunc called with int: 15 and float: 9.52
```
