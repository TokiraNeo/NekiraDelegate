```c++
//@[INFO] Declare the SingleDelegate Type
//@[INFO] NEKIRA_SINGLE_DELEGATE(DelegateTypeName, ReturnType, ...)
NEKIRA_SINGLE_DELEGATE(SingleSignature, void, int, float)

auto Lambda = [](int a, float b) { std::cout << "Lambda called with int: " << a << " and float: " << b << std::endl; };

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

};

struct FuncObject
{
    void operator()(int a, float b)
    {
        std::cout << "FuncObject called with int: " << a << " and float: " << b << std::endl;
    }
};

int main()
{
    SingleSignature delegate;

    // Lambda
    delegate.BindFunctionObject(Lambda);
    delegate.Invoke(42, 3.14f);

    // Free function
    delegate.BindFunction(FreeFunction);
    delegate.Invoke(24, 1.59f);

    // Function object
    FuncObject Obj;
    delegate.BindFunctionObject(Obj);
    delegate.Invoke(7, 1.41f);

    // Member function
    {
        SampleStruct sample;
        delegate.BindMemberFunction(&sample, &SampleStruct::Func);
        delegate.Invoke(8, 1.73f);
    }
    //@[INFO] Here will not call the member function as sample object has been destroyed
    delegate.Invoke(9, 33.2f);

    // Const member function
    {
        SampleStruct sample;
        delegate.BindMemberFunction(&sample, &SampleStruct::ConstFunc);
        delegate.Invoke(10, 2.23f);
    }
    //@[INFO] Here will not call the member function as sample object has been destroyed
    delegate.Invoke(11, 4.56f);


    return 0;

}
```
