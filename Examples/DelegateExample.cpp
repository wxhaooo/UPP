#include<iostream>
#include<vector>
#include "Delegate.h"

template<typename T>
class TestElementClass
{
public:
	TestElementClass() = default;
	TestElementClass(T e) :element(e) {}

	TestElementClass& operator + (TestElementClass& b)
	{
		TestElementClass res;
		res.element = this->element + b.element;
		return res;
	}

	TestElementClass& operator = (const TestElementClass& b)
	{
		element = b.element;
		return *this;
	}

	T element;
};

template<typename T>
TestElementClass<T> TestElementAdd(TestElementClass<T>& t1,TestElementClass<T>& t2)
{
	return t1 + t2;
}

class TestClassAdd
{
public:
	TestClassAdd(int tmpIn) :tmp(tmpIn) {}
	TestClassAdd() = default;

	TestElementClass<float> MixAdd(int a, int b, float c, TestElementClass<float> d) const
	{
		TestElementClass<float> res;
		res.element = a + b + c + d.element + tmp;
		return res;
	}

	int TestFunc(int a, int b) { return a + b; }

	int TestFuncFour(int a, int b, int c, int d) const { return tmp + a + b + c + d; }

	static void StaticTestFunc(const int& a, int b) { std::cout << b - a << "\n"; }

	void TestMulticastFunc(const int& a, int b,int c ,int d)
	{
		std::cout << a * b + c + d<< "\n";
	}

private:
	int tmp;
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

int TestFuncAddFour(int a,int b,int c,int d)
{
	return a + b + c * d;
}

float TestFunc1(int a,int b,float c)
{
	return a + b * c;
}

void TestMulticastFunc(const int& a,int b)
{
	std::cout << a + b << "\n";
}

void TestMulticastFunc1(const int& a, int b)
{
	std::cout << a - b << "\n";
}

DECLARE_DELEGATE(TestDelegateClass, TestElementClass<float>, int, int)
DECLARE_MULTICAST_DELEGATE(TestMulticastDelegateClass, const int&, int)

int main()
{
	TestClassAdd addClass(5);
	TestClassAdd addClass1(6);
	TestElementClass<float> t1(3);
	TestElementClass<float> t2(5);

	TestMulticastDelegateClass multicastDelegate;

	multicastDelegate.AddStatic(&TestMulticastFunc);
	multicastDelegate.AddStatic(&TestMulticastFunc1);
	multicastDelegate.AddStatic(&TestClassAdd::StaticTestFunc);
	multicastDelegate.AddRaw(&addClass, &TestClassAdd::TestMulticastFunc, 5, 6);

	multicastDelegate.Broadcast(3, 4);

	// TestDelegateClass delegateTest;
	// delegateTest.BindRaw(&addClass1, &TestClassAdd::MixAdd, 3.f, t2);
	//
	// std::cout << delegateTest.Execute(3, 4).element << "\n";
	return 0;
}