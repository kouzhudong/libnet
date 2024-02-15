#include "pch.h"


LPWSTR UTF8ToWideChar(IN PCHAR utf8)
/*
得到的内存有调用者释放。
*/
{
    if (NULL == utf8) {
        return NULL;
    }

    if (0 == lstrlenA(utf8)) {
        return NULL;
    }

    int cchWideChar = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, 0, 0);

    LPWSTR pws = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)cchWideChar * 4);
    if (pws) {
        int ret = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, pws, cchWideChar);//utf8->Unicode
        _ASSERTE(ret);
    } else {
        //LOGA(ERROR_LEVEL, "申请内存失败");
    }

    return pws;
}