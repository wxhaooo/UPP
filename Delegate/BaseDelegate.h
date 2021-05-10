#pragma once
#include "DelegateInstance.h"

// ParamsType is Function Params
// VarTypes is Function Payload
// ATTENTION: VatTypes must be value-type
// Don't use reference type in payload position of delegate function

// Example:
// TestElementClass<float> MixAdd(int a, int b, float c, TestElementClass<float> d)
// {
// TestElementClass<float> res;
// res.element = a + b + c + d.element + tmp;
// return res;
// }
// TestElementClass<float> d, d is payload, so d must not be reference type!!!

template<typename RetType, typename... ParamsTypes>
class BaseDelegate :public DelegateBase
{
public:
	using FuncType = RetType(ParamsTypes...);
	using DelegateInstanceInterface = IBaseDelegateInstance<FuncType>;

	template<typename... VarTypes>
	static BaseDelegate<RetType, ParamsTypes...>
		CreateStatic(typename Identity<RetType(*)(ParamsTypes..., VarTypes...)>::Type func, VarTypes... vars)
	{
		BaseDelegate<RetType, ParamsTypes...> result;
		StaticDelegateInstance<FuncType, VarTypes...>::Create(result, func, vars...);
		return result;
	}

	template<typename ClassType,typename... VarTypes>
	static BaseDelegate<RetType, ParamsTypes...>
	CreateRaw(ClassType* methodOwner,
		typename MemFuncPtrType<false, ClassType, RetType(ParamsTypes..., VarTypes...)>::Type func,
		VarTypes... vars)
	{
		BaseDelegate<RetType, ParamsTypes...> result;
		RawMethodDelegateInstance<false, ClassType, FuncType, VarTypes...>::Create(result, methodOwner, func, vars...);
		return result;
	}

	template<typename ClassType, typename... VarTypes>
	static BaseDelegate<RetType, ParamsTypes...>
		CreateRaw(ClassType* methodOwner,
			typename MemFuncPtrType<true, ClassType, RetType(ParamsTypes..., VarTypes...)>::Type func, 
			VarTypes... vars)
	{
		BaseDelegate<RetType, ParamsTypes...> result;
		RawMethodDelegateInstance<true, ClassType, FuncType, VarTypes...>::Create(result, methodOwner, func, vars...);
		return result;
	}

	template<typename... VarTypes>
	void BindStatic(typename StaticDelegateInstance<FuncType, VarTypes...>::FuncPtr func, VarTypes... vars)
	{
		*this = CreateStatic(func, vars...);
	}

	template<typename ClassType, typename... VarTypes>
	void BindRaw(ClassType* classObject,
		typename MemFuncPtrType<false, ClassType, RetType(ParamsTypes..., VarTypes...)>::Type func,
		VarTypes... vars)
	{
		*this = CreateRaw(classObject, func, vars...);
	}

	template<typename ClassType, typename... VarTypes>
	void BindRaw(ClassType* classObject,
		typename MemFuncPtrType<true, ClassType, RetType(ParamsTypes..., VarTypes...)>::Type func,
		VarTypes... vars)
	{
		*this = CreateRaw(classObject, func, vars...);
	}

	RetType Execute(ParamsTypes... params) const
	{
		auto* delegateInstanceInterface = 
			static_cast<DelegateInstanceInterface*>(DelegateBase::GetAllocation());
		return delegateInstanceInterface->Execute(std::forward<ParamsTypes>(params)...);
	}
};

