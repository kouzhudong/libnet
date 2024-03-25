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
        int ret = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, pws, cchWideChar); //utf8->Unicode
        _ASSERTE(ret);
    } else {
        //LOGA(ERROR_LEVEL, "申请内存失败");
    }

    return pws;
}


BOOL WINAPI AdjustCurrentProcessPrivilege(PCTSTR szPrivilege, BOOL fEnable)
/*
功能：本进程的特权开启的开关。

细节：
叫Set不如叫Get，叫Get不如叫Adjust（调整）。

如：
EnablePrivilege(SE_DEBUG_NAME, TRUE);
EnablePrivilege(SE_DEBUG_NAME, FALSE);
*/
{
    // Enabling the debug privilege allows the application to see information about service applications
    BOOL fOk = FALSE; // Assume function fails
    HANDLE hToken;

    // Try to open this process's access token
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
        // Attempt to modify the given privilege
        TOKEN_PRIVILEGES tp;

        tp.PrivilegeCount = 1;
        LookupPrivilegeValue(nullptr, szPrivilege, &tp.Privileges[0].Luid);

        tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
        AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), nullptr, nullptr);
        fOk = (GetLastError() == ERROR_SUCCESS);

        CloseHandle(hToken); // Don't forget to close the token handle
    }

    return (fOk);
}


DWORD WINAPI EnumerateProcess(_In_ EnumerateProcessCallBack CallBack, _In_opt_ PVOID Context)
/*

*/
{
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return GetLastError();
    }

    // Retrieve information about the first process
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return GetLastError();
    }

    do { // Now walk the snapshot of processes
        if (CallBack) {
            if (CallBack(&pe32, Context)) {
                break;
            }
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return GetLastError();
}


BOOL WINAPI FindOnePidByName(_In_ LPPROCESSENTRY32W lppe, _In_opt_ PVOID Context)
{
    ProcessInfo * Process = (ProcessInfo *)Context;

    if (!lppe || !Process) {
        return FALSE;
    }

    if (_wcsicmp(Process->ProcessName, lppe->szExeFile) == 0) {
        Process->ProcessID = lppe->th32ProcessID;
        return TRUE;
    }

    return FALSE;
}


DWORD FindOnePidByName(PCWCHAR ProcessName)
{
    ProcessInfo Process{};
    lstrcpy(Process.ProcessName, ProcessName);

    EnumerateProcess(FindOnePidByName, &Process);

    return Process.ProcessID;
}
