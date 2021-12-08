/*
本文只要收集关于网卡（适配器的代码）。

网卡（适配器的代码）是一个基本的概念，不可忽视的知识点，必须把握。
*/

#pragma once
#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


__declspec(dllexport)
int WINAPI EnumAdaptersAddressesInfo(int argc, char ** argv);

__declspec(dllexport)
int WINAPI EnumAdaptersInfo();


EXTERN_C_END
