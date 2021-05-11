<!-- TOC -->

- [Delegate](#delegate)
	- [Why use Delegate?](#why-use-delegate)
	- [Delegate Implementation](#delegate-implementation)
		- [Expression Divergence about Function Pointer](#expression-divergence-about-function-pointer)
		- [First Simple Delegate Sample](#first-simple-delegate-sample)
		- [Bind Any Class With Delegate](#bind-any-class-with-delegate)
		- [Delegate For Different Functions](#delegate-for-different-functions)
		- [Conclusion](#conclusion)

<!-- /TOC -->
# Delegate

## Why use Delegate?

试想有一个游戏，__当击败某个怪物领袖的时候其他怪物的攻击力会降低，玩家的攻击力会短暂上升。这时候该如何实现这个需求？__

一个简单的想法是让 __这个怪物领袖的类去存储所有其他怪物以及玩家对象，当击败这个事件触发的时候，调用所有对象相关的方法。__

这似乎解决了问题，但仔细思考会发现几个问题：

1. __怪物领袖类和其他怪物对象以及玩家之间逻辑上没有包含关系，在怪物领袖类中存储其他怪物对象和玩家对象逻辑上不合理。__
2. __加入这时候策划想加入另一种机制，当怪物领袖被击败时，场景中会出现几种以前不能刷出的物品。此时使用上面的机制必须去修改怪物领袖类。__
3. __更麻烦的是，如果怪物领袖里存储的这些对象本身已经被destory了，调用destory对象的对应方法会造成未知的问题。__

那么该如何解决这个问题呢？我们可以把怪物领袖被击败这个事情抽象成一个 __事件__。当这个事件发生时我们希望这个事件的观察者能够做出 __响应__。这样的抽象就使问题变成了 __事件-响应机制__ 问题。如果有一种方法能够实现 __事件-响应机制__，上面的问题就迎刃而解了。

另外从 __事件-响应机制__ 的角度来看会发现， __上面的方法要求事件的发出者去主动调用事件的观察者的对应方法。而正确的逻辑应该是事件的观察者去主动侦听事件是否发生。__

这里有些同学可能会马上想到那能不能让观察者每隔一段时间去检测一下事件是否发生？这种思路有两个问题：

1. 每隔一段时间去检测会大量消耗CPU资源
2. 间隔时间不确定，可能会错过时间发生

再仔细思考第一种方法存在的问题，我们会发现 __这种方法最大的痛点在于我们必须保存每一个观察这个事件的观察者这个对象本身。__ 如果我们 __不保存观察者对象，而是能够统一一个所有观察者的接口，然后当事件发生时我们调用所有的观察者接口不就可以解决问题了吗？__

针对上面的问题， __观察者模式__ 就应运而生了。 所谓 __观察者模式是一种应用比较广泛的设计模式，它通过约定观察者的统一抽象接口，然后让事件发出者保存这些接口。在事件发生时依次调用统一的接口来实现我们期待的功能。__

说了这么半天为什么没有提到Delegate， __因为Delegate就是观察者模式的一种实现而已__。 由于C++缺乏事件驱动机制，Delegate就是用来补足这种功能，实现代码逻辑与实现逻辑统一的。

废话不多说，我们接下来一步步来实现UE中部分delegate的一些功能以便加深对Delegate的理解。

## Delegate Implementation

在上面我们提到需要一个统一的抽象接口，那这个抽象接口在C++中应该对应什么呢？ 答案是 __函数指针__ 。 __如果能够保证每个函数的签名都一致，那么自然而言有一个统一的抽象接口。 使用的时候直接调用即可。__ 

不幸的是，C++的世界没有那么简单，我们遇到的第一个问题是 普通函数指针和成员函数指针的调用是不一致的。具体的

```cpp
// f is common function pointer
f(...);
// mf is member function pointer
// obj is associated class object ptr
// f and mf have similar function signature
(obj->*mf)(...);
```

给一个具体的例子：

```cpp
```cpp
#include<iostream>

class TestClass
{
public:
	TestClass() = default;

    // Member Function
	int TestFunc(int a, int b) { return a + b; }
    // Static Member Function
	static int StaticTestFunc(int a, int b) { return a + b; }
};

// Common Function
int TestFunc(int a, int b)
{
	return a + b;
}

int main()
{
    // Although the function signatures of member functions and ordinary functions are the same, but form of their function pointer are different.
    // The function signatures of static member functions and ordinary functions are the same and form of their function pointer are the same.
	TestClass testClass;
	int (*fPtr)(int, int) = &TestFunc;
    // Call Common Function by Function Pointer
	std::cout << fPtr(2, 3) << "\n";
	int (TestClass:: * mfPtr)(int, int) = &TestClass::TestFunc;
    // Call Member Function by Member Function Pointer
	std::cout << (testClass.*mfPtr)(3, 4) << "\n";
	fPtr = TestClass::StaticTestFunc;
    // Call Static Member Function by Function Pointer
	std::cout << fPtr(8, 9) << "\n";
	return 0;
}
```

接下来我们首先来处理这个问题。

### Expression Divergence about Function Pointer

从上面的代码可以看出，为了使用委托，我们需要处理普通函数与成员函数使用的函数指针形式不同的问题。处理这个问题UE中使用的定义类的形式。也就是说 __不同的函数类型定义不同的类（函数类型在这里只普通函数，成员函数，UFUNCTIUON标记的成员函数等等）__，比如静态和普通函数一个类（TBaseStaticDelegateInstance），UFunction一个类（TBaseUFunctionDelegateInstance）,普通成员函数与UObject的方法各自一个类（TBaseRawMethodDelegateInstance与TBaseUObjectMethodDelegateInstance）等等，本文的后面我们会看到UE使用的这种方法。现在我们使用另一个方式来处理这个问题。

```cpp
#include<iostream>

class TestClass
{
public:
	TestClass() = default;

	int TestFunc(int a, int b) { return a + b; }

	static int StaticTestFunc(int a, int b) { return a + b; }
};

int TestFunc(int a, int b)
{
	return a + b;
}

using FuncType = int(*)(int, int);
using MemberFuncType = int(TestClass::*)(int, int);

// Common Function Wrapper
template<FuncType Func>
static int CommonFuncWrapper(void* objPtr, int a, int b)
{
	return Func(a, b);
}

// Member Function Wrapper
template<MemberFuncType Func>
static int MemberFuncWrapper(void* objPtr, int a, int b)
{
	TestClass* testClass = (TestClass*)(objPtr);
	return (testClass->*Func)(a, b);
}

// define a New Function Pointer as Interface
using FuncWrapper = int(*)(void* p, int a, int b);

int main()
{
	TestClass testClass;
    // Common Function and Member Function use same function pointer 
	FuncWrapper CommonFuncPtr = &(CommonFuncWrapper<&TestFunc>);
	std::cout << (CommonFuncPtr)(nullptr, 3, 4) << "\n";
	FuncWrapper MemberFuncPtr = &(MemberFuncWrapper<&TestClass::TestFunc>);
	std::cout << (MemberFuncPtr)(&testClass, 5, 6) << "\n";
	return 0;
}
```

在这段代码中我们使用一个wrapper封装了普通函数和成员函数参数不同的问题。这种方法比较简单，不太适合应付UE中比较复杂的环境，但用于实现简单的Delegate是足够的了。再次提醒，在UE中使用不同的Instance类区分普通函数和成员函数以及Lambda表达式等函数对象。这里只是希望大家明白我们实现过程中需要解决的问题有哪些。

### First Simple Delegate Sample

```cpp
#include<iostream>

class TestClass
{
public:
	TestClass() = default;

	int TestFunc(int a, int b) { return a + b; }

	static int StaticTestFunc(int a, int b) { return a + b; }
};

int TestFunc(int a, int b)
{
	return a + b;
}

// Delegate Example Class to Delegate Test Func
class DelegateExample
{
public:
	DelegateExample() = default;
	~DelegateExample() = default;

	using FuncType = int(*)(int, int);
	using MemberFuncType = int(TestClass::*)(int, int);
	using FuncWrapper = int(*)(void* p, int a, int b);

    // function wrapper for unify interface
	template<FuncType Func>
	static int CommonFuncWrapper(void* objPtr, int a, int b)
	{
		return Func(a, b);
	}

	template<MemberFuncType Func>
	static int MemberFuncWrapper(void* objPtr, int a, int b)
	{
		TestClass* testClass = (TestClass*)(objPtr);
		return (testClass->*Func)(a, b);
	}

	static DelegateExample Create(void* objPtr,FuncWrapper func)
	{
		DelegateExample delegateExample;
		delegateExample.objPtr = objPtr;
		delegateExample.func = func;
		
		return delegateExample;
	}

    // delegate interface for common function and static function
	template<FuncType Func>
	static DelegateExample BindStatic()
	{
		return Create(nullptr, &CommonFuncWrapper<Func>);
	}
    // delegate interface for member function
	template<MemberFuncType Func>
	static DelegateExample BindMember(void* objPtr)
	{
		return Create(objPtr, &MemberFuncWrapper<Func>);
	}
    // execute delegate
	int Execute(int a, int b)
	{
		return (*func)(objPtr, a, b);
	}
private:
	void* objPtr;
	FuncWrapper func;
};

int main()
{
	TestClass testClass;
	DelegateExample delegateExample1 = DelegateExample::BindStatic<&TestFunc>();
	DelegateExample delegateExample2 = DelegateExample::BindMember<&TestClass::TestFunc>(&testClass);
	std::cout << delegateExample1.Execute(3, 4) << '\n';
	std::cout << delegateExample2.Execute(5, 6) << '\n';
	return 0;
}
```
通过把上一段中的warpper封装在DelegateExample中，我们得到了delegate的第一个版本。通过DelegateExample，我们看到 __delegate实际上就是想办法保存下来函数指针并在需要地方调用__。但上面版本遇到的问题是

1. DelegateExample能够绑定的函数类型是被指定的，只能绑定(int)(int,int)签名的函数。
2. DelegateExample只能绑定TestClass中的(int)(int,int)签名的函数，其他类同样签名的函数无法绑定。
3. 绑定函数的参数是有限的，但我们希望能够绑定的参数是可变的。
4. 能够delegate的对象只有一个objPtr，我们希望能够delegate多个对象从而实现多播。

1，3问题类似，我们后面再来解决它，接下来我们先实现2，4两个需求。

###  Bind Any Class With Delegate

分析需求2，4，为了使有相同函数签名的不同类的不同成员函数能绑定到同一个delegate，解决方法 __只有使用C++的类模板来指定函数签名和类的类型。__

```cpp
#include<iostream>

class TestClassAdd
{
public:
	TestClassAdd() = default;

	int TestFunc(int a, int b) { return a + b; }

	static int StaticTestFunc(int a, int b) { return a + b; }
};

class TestClassSub
{
public:
	TestClassSub() = default;

	int TestFunc(int a, int b) { return a - b; }

	static int StaticTestFunc(int a, int b) { return a - b; }
};

int TestFuncAdd(int a, int b)
{
	return a + b;
}

class DelegateExample
{
public:
	DelegateExample() = default;
	~DelegateExample() = default;

	using FuncType = int(*)(int, int);
	using FuncWrapper = int(*)(void* p, int a, int b);

	template<FuncType Func>
	static int CommonFuncWrapper(void* objPtr, int a, int b)
	{
		return Func(a, b);
	}

    // Use the class template parameter to specify which class type the Wrapper is
	template<typename ClassType, int(ClassType::* Func)(int, int)>
	static int MemberFuncWrapper(void* objPtr, int a, int b)
	{
		ClassType* ownerClass = (ClassType*)objPtr;
		return (ownerClass->*Func)(a, b);
	}

	static DelegateExample Create(void* objPtr,FuncWrapper func)
	{
		DelegateExample delegateExample;
		
		delegateExample.objPtr = objPtr;
		delegateExample.func = func;
		
		return delegateExample;
	}

	template<FuncType Func>
	static DelegateExample BindStatic()
	{
		return Create(nullptr, &CommonFuncWrapper<Func>);
	}

    // Use the class template parameter to specify which class type the Wrapper is
	template<typename ClassType, int(ClassType::*Func)(int, int)>
	static DelegateExample BindMember(void* objPtr)
	{
		return Create(objPtr, &MemberFuncWrapper<ClassType, Func>);
	}

	int Execute(int a, int b)
	{
		return (*func)(objPtr, a, b);
	}
private:
	void* objPtr;
	FuncWrapper func;
};


int main()
{
	TestClassAdd testClassAdd;
	TestClassSub testClassSub;
    // You can bind functions of different classes with the same signature, but the function signature is determined
	DelegateExample delegateExample1 = DelegateExample::BindMember<TestClassAdd, &TestClassAdd::TestFunc>(&testClassAdd);
	DelegateExample delegateExample2 = DelegateExample::BindMember<TestClassSub, &TestClassSub::TestFunc>(&testClassSub);
	std::cout << delegateExample1.Execute(5, 6) << '\n';
	std::cout << delegateExample2.Execute(5, 6) << '\n';
	return 0;
}
```

上面的代码我们使用类模板参数指定类来实现了DelegateExample可以绑定到不同拥有同样函数签名的类的成员函数上这个需求。这一步UE4使用的方法本质上是与上面的想法相同的。接下来我们实现一个Delegate可以绑定不同的函数。 __思路仍然是使用类模板以及可变参数。__

### Delegate For Different Functions

```cpp
#include<iostream>

class TestClassAdd
{
public:
	TestClassAdd() = default;

	int TestFunc(int a, int b) { return a + b; }

	static int StaticTestFunc(int a, int b) { return a + b; }
};

class TestClassSub
{
public:
	TestClassSub() = default;

	int TestFunc(int a, int b) { return a - b; }

	static int StaticTestFunc(int a, int b) { return a - b; }
};

int TestFuncAdd(int a, int b)
{
	return a + b;
}

float TestFunc1(int a,int b,float c)
{
	return a + b * c;
}

template<typename RetType, typename ...ArgsType>
class DelegateExample
{
public:
	DelegateExample() = default;
	~DelegateExample() = default;

	using FuncType = RetType(*)(ArgsType...);
	using FuncWrapper = RetType(*)(void*, ArgsType...);

	template<FuncType Func>
	static RetType CommonFuncWrapper(void* objPtr, ArgsType... args)
	{
		return Func(std::forward<ArgsType>(args)...);
	}

	template<typename ClassType, RetType(ClassType::* Func)(ArgsType...)>
	static int MemberFuncWrapper(void* objPtr, ArgsType... args)
	{
		ClassType* ownerClass = (ClassType*)objPtr;
		return (ownerClass->*Func)(std::forward<ArgsType>(args)...);
	}

	static DelegateExample Create(void* objPtr, FuncWrapper func)
	{
		DelegateExample delegateExample;
		
		delegateExample.objPtr = objPtr;
		delegateExample.func = func;
		
		return delegateExample;
	}

	template<FuncType Func>
	static DelegateExample BindStatic()
	{
		return Create(nullptr, &CommonFuncWrapper<Func>);
	}

	template<typename ClassType, RetType(ClassType::*Func)(ArgsType...) >
	static DelegateExample BindMember(void* objPtr)
	{
		return Create(objPtr, &MemberFuncWrapper<ClassType, Func>);
	}

	RetType Execute(ArgsType... args)
	{
		return (*func)(objPtr, std::forward<ArgsType>(args)...);
	}
private:
	void* objPtr;
	FuncWrapper func;
};

// UE4 Style Delegate Macro
#define DECLARE_DELEGATE(DelegateName,RetType,...) \
using DelegateName = DelegateExample<RetType, __VA_ARGS__>;

DECLARE_DELEGATE(FDelegateTest1, float, int, int, float)
DECLARE_DELEGATE(FDelegateTest2, int, int, int)

int main()
{
	TestClassAdd testClassAdd;
	TestClassSub testClassSub;

	// Bind Static Function
	FDelegateTest1 delegate_test1 = FDelegateTest1::BindStatic<&TestFunc1>();
	std::cout << delegate_test1.Execute(3, 3, 4.f) << "\n";

	// Bind Different Function
	FDelegateTest2 delegate_test2 = FDelegateTest2::BindStatic<&TestFuncAdd>();
	std::cout << delegate_test2.Execute(3, 3) << "\n";

	// Bind Member Function with class TestClassSub
	delegate_test2 = FDelegateTest2::BindMember<TestClassSub, &TestClassSub::TestFunc>(&testClassSub);
	std::cout << delegate_test2.Execute(6, 5) << "\n";

	// Bind Member Function with class TestClassAdd
	delegate_test2 = FDelegateTest2::BindMember<TestClassAdd, &TestClassAdd::TestFunc>(&testClassAdd);
	std::cout << delegate_test2.Execute(6, 5) << "\n";
	return 0;
}
```

上面的这份代码我们使用类模板和可变参数来实现了绑定任意函数任意类到delegate并使用宏来封装一个UE4风格的delegate。如果对类模板和可变参数不熟悉，可以先看一下这方面的东西再来看这个。最后我们一个可以多播的Delegate的思路是非常简单的，只要把所有接口用一个数组都存下来，然后broadcast的时候依次调用即可实现多播。这里不再赘述，留给读者自己去实现扩展。

到此为止，我们已经实现了一个简单明晰的Delegate的demo，这个Delegate demo虽小但是已经覆盖到了UE中非Dynamic Delegate的机制。（Dynamic Delegate需要反射的支持，又是一个C++没有直接支持的机制。）

### Conclusion

这一节希望读者能明晰:

1. __为什么使用Delegate？__
2. __Delegate使用的场合__
3. __Delegate实现中需要注意的问题（统一不同的函数类型，使用模板参数来完成绑定到Delegate函数和类的泛化，使用数组来保存不同多个函数对象以便实现多播）__

最后提供一个[UE4简化版的实现](https://github.com/wxntech/UPP)。这份代码的Delegate部分实现了UE中使用的Delegate的简化版实现。 实现参考了UE4源码重新造了一遍轮子。如果想进一步阅读UE4 Delegate相关的代码，可以先阅读这个简化版代码。

