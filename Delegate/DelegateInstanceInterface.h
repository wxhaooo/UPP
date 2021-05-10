#pragma once

template<typename FuncType>
struct IBaseDelegateInstance;

template<typename FuncType>
struct IBaseDelegateInstanceCommon;

template<typename RetType, typename... ArgTypes>
struct IBaseDelegateInstanceCommon<RetType(ArgTypes...)>
{
	virtual ~IBaseDelegateInstanceCommon() {}
	
	virtual RetType Execute(ArgTypes...) const = 0;
};

template<typename FuncType>
struct IBaseDelegateInstance: public IBaseDelegateInstanceCommon<FuncType>
{
	
};