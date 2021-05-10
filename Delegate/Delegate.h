#pragma once
#include "BaseDelegate.h"
#include "MulticastDelegate.h"

#define DECLARE_DELEGATE(DelegateName,RetType,...) \
using DelegateName = BaseDelegate<RetType, __VA_ARGS__>;

#define DECLARE_MULTICAST_DELEGATE(MulticastDelegateName,...)\
using MulticastDelegateName = MulticastDelegate<void,__VA_ARGS__>;