#pragma once

#include "DelegateInstance.h"
#include "BaseDelegate.h"
#include <vector>

// RetType of Multicast Delegate must be void

template<typename RetType,typename... ParamTypes>
class MulticastDelegate;

template<typename... ParamTypes>
class MulticastDelegate<void, ParamTypes...>
{
public:
	using Delegate = BaseDelegate<void, ParamTypes...>;
	using DelegateInstanceInterface = IBaseDelegateInstance<void(ParamTypes...)>;

	template<typename... VarTypes>
	void AddStatic(typename StaticDelegateInstance<void(ParamTypes...), VarTypes...>::FuncPtr func, VarTypes... vars)
	{
		delegates.push_back(
		Delegate::CreateStatic(func, vars...)
		);
	}

	template<typename ClassType,typename... VarTypes>
	void AddRaw(ClassType* methodOwner,
		typename MemFuncPtrType<false, ClassType, void(ParamTypes..., VarTypes...)>::Type func,
		VarTypes... vars)
	{
		delegates.push_back(
			Delegate::CreateRaw(methodOwner, func, vars...)
		);
	}

	template<typename ClassType, typename... VarTypes>
	void AddRaw(ClassType* methodOwner,
		typename MemFuncPtrType<true, ClassType, void(ParamTypes..., VarTypes...)>::Type func,
		VarTypes... vars)
	{
		delegates.push_back(
			Delegate::CreateRaw(methodOwner, func, vars...)
		);
	}

	void Broadcast(ParamTypes... params)
	{
		for (Delegate& delegateHandle : delegates)
		{
			delegateHandle.Execute(std::forward<ParamTypes>(params)...);
		}
	}

private:
	std::vector<Delegate> delegates;
};