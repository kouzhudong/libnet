#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


__declspec(dllexport)
void WINAPI HttpReadData();

__declspec(dllexport)
int WINAPI IWinHttpRequestSend();

__declspec(dllexport)
void WINAPI GetDefaultProxyConfiguration();

__declspec(dllexport)
void WINAPI GetIEProxyConfigForCurrentUser();

__declspec(dllexport)
void WINAPI DetectAutoProxyConfigUrl();

__declspec(dllexport)
void WINAPI GetProxyForUrl();

__declspec(dllexport)
void WINAPI Sessions();


EXTERN_C_END


//////////////////////////////////////////////////////////////////////////////////////////////////
