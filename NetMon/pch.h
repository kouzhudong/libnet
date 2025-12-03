#pragma once

#include "framework.h"

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport) // 之所以定义这个，是因为.clang-format下，这个宏可以保持换行。
#endif
