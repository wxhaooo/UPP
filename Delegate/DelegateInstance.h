#pragma once

#include "DelegateInstanceInterface.h"
#include "DelegateBase.h"
#include <utility>

template<typename FuncType, typename... VarTypes>
class StaticDelegateInstance;

template<typename RetType, typename... ParamTypes, typename... VarTypes>
class StaticDelegateInstance<RetType(ParamTypes...), VarTypes...>: public IBaseDelegateInstance<RetType(ParamTypes...)>
{
public:
	using FuncPtr = RetType(*)(ParamTypes..., VarTypes...);

	StaticDelegateInstance(FuncPtr staticFuncPtrIn, VarTypes... vars)
		:staticFuncPtr(staticFuncPtrIn), payload(vars...)
	{
		assert(staticFuncPtr != nullptr);
	}

	static void Create(DelegateBase& base, FuncPtr func, VarTypes... vars)
	{
		new (base) StaticDelegateInstance<RetType(ParamTypes...), VarTypes...>(func, vars...);
	}

	virtual RetType Execute(ParamTypes... params) const override final
	{
		assert(staticFuncPtr != nullptr);
		// simple solution to combine params and payload, need more efficient solution 
		auto arguments = std::tuple_cat(std::make_tuple(params...), payload);
		return std::apply(staticFuncPtr, arguments);
	}
private:
	FuncPtr staticFuncPtr;
	std::tuple<VarTypes...> payload;
};

// const version

template<bool Const, class ClassType, typename FuncType, typename... VarTypes>
class RawMethodDelegateInstanceCommon;

template<bool Const, class ClassType, typename RetType, typename... ParamTypes, typename... VarTypes>
class RawMethodDelegateInstanceCommon<Const, ClassType, RetType(ParamTypes...), VarTypes...> : public IBaseDelegateInstance<RetType(ParamTypes...)>
{
public:
	using MethodPtr = typename MemFuncPtrType<Const, ClassType, RetType(ParamTypes..., VarTypes...)>::Type;
	
	template<std::size_t... Is>
	RetType ApplyTuple(ClassType* methodOwner,
		MethodPtr methodPtr,
		// un-reference
		std::tuple<std::_Unrefwrap_t<ParamTypes>..., std::_Unrefwrap_t<VarTypes>...>& tuple,
		std::index_sequence<Is...>) const
	{
		return (methodOwner->*methodPtr)(std::get<Is>(tuple)...);
	}
};

template<bool Const, class ClassType, typename FuncType, typename... VarTypes>
class RawMethodDelegateInstance;

template<bool Const, class ClassType, typename RetType, typename... ParamTypes, typename... VarTypes>
class RawMethodDelegateInstance<Const, ClassType, RetType(ParamTypes...), VarTypes...> : public RawMethodDelegateInstanceCommon<Const, ClassType, RetType(ParamTypes...), VarTypes...>
{
public:
	using Super = RawMethodDelegateInstanceCommon<Const, ClassType, RetType(ParamTypes...), VarTypes...>;
	using MethodPtr = typename MemFuncPtrType<Const, ClassType, RetType(ParamTypes..., VarTypes...)>::Type;

	RawMethodDelegateInstance(ClassType* methodOwnerIn,MethodPtr methodPtrIn,VarTypes... vars)
		:methodOwner(methodOwnerIn),methodPtr(methodPtrIn),payload(vars...)
	{
		assert(methodOwner != nullptr && methodPtr != nullptr);
	}

	static void Create(DelegateBase& base, ClassType* methodOwner, MethodPtr func, VarTypes... vars)
	{
		new (base) RawMethodDelegateInstance<Const, ClassType, RetType(ParamTypes...), VarTypes...>(methodOwner, func, vars...);
	}

	virtual RetType Execute(ParamTypes... params) const override final
	{
		assert(methodOwner != nullptr && methodPtr != nullptr);
		// simple solution to combine params and payload, need more efficient solution 
		auto arguments = std::tuple_cat(std::make_tuple(params...), payload);
		constexpr auto tupleSize =std::tuple_size<decltype(arguments)>::value;

		return Super::ApplyTuple(methodOwner, methodPtr, arguments, std::make_index_sequence<tupleSize>());

		// return std::invoke(methodPtr, methodOwner, std::get<Is>(arguments)...);
		// return (methodOwner->*methodPtr)(params...);
	}

private:
	ClassType* methodOwner;
	MethodPtr methodPtr;
	std::tuple<VarTypes...> payload;
};

