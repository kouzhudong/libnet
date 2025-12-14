// SO_REUSEADDR 和 SO_REUSEPORT 是针对远端不同的IP和PORT的，这个是前提，然后才可重复利用本地的IP和PORT。四元组唯一性还是要保证的。

/*
数据包流向

刚才说了那么多情况，有一个问题还没有解决，那就是如果两个功能完全不同的进程监听在完全相同的ip：port上，那么流量应当给谁呢？

实际上这也要分情况讨论：

在windows上，如果两个进程同时监听在完全相同的ip：port上，那么遵守先来先得的规则，外部请求只会被第一个监听在此ip：port的socket所接收，第二个socket将完全无法收到外部的请求

在linux上，同样情况下，由于linux内核对此的实现是尝试将外部请求平均分配，实现负载均衡，所以两个进程均有近似相同的几率可以获得来自外部的请求

在BSD/MAC上，需要分两种情况，当监听的ip地址为通配符地址时，与windows相同，遵守先来先得的规则，外部请求只会被第一个监听在此ip：port的socket所接收，第二个socket将无法接收外部的请求，而当监听的地址为特定地址时，遵守后来先得的规则，外部请求只会被第二个监听在此ip：port的socket所接收，第一个socket将无法接收外部的请求

以上摘自：https://ph4ntonn.github.io/Port-reuse

这里说的，和我Windows上测试的效果是一致的。
*/


#pragma once

#include "pch.h"

int PortReuse();
int __cdecl EXCLUSIVEADDRUSE(int argc, wchar_t ** argv);

int PortReuseServer();
int PortReuseClient();
