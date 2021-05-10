#pragma once
#include<memory>
#include<tuple>
#include<cassert>
#include "DelegateCommon.h"

// for delegate memory manager
class DelegateBase
{
public:
	void* GetAllocation() const { return memPtr; }

private:
	friend void* operator new(size_t size, DelegateBase& base);
	friend void operator delete(void* ptr, size_t size, DelegateBase& base);

	void* Allocate(size_t size)
	{
		memPtr = std::malloc(size);

		if (memPtr != nullptr)
			delegateSize = size;

		return memPtr;
	}

private:
	void* memPtr;
	size_t delegateSize;
};

inline void* operator new(size_t size, DelegateBase& base)
{
	return base.Allocate(size);
}