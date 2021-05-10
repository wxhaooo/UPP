#pragma once

template<typename T>
struct Identity
{
	using Type = T;
};

template<bool Const, typename ClassType, typename FuncType>
struct MemFuncPtrType;

template<typename ClassType, typename RetType, typename... ArgTypes>
struct MemFuncPtrType<false, ClassType, RetType(ArgTypes...)>
{
	using Type = RetType(ClassType::*)(ArgTypes...);
};

template<typename ClassType, typename RetType, typename... ArgTypes>
struct MemFuncPtrType<true, ClassType, RetType(ArgTypes...)>
{
	using Type = RetType(ClassType::*)(ArgTypes...) const;
};