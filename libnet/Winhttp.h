#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////


//C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\um\httprequest.idl
typedef long HTTPREQUEST_PROXY_SETTING;
const HTTPREQUEST_PROXY_SETTING HTTPREQUEST_PROXYSETTING_DEFAULT = 0x00000000;
const HTTPREQUEST_PROXY_SETTING HTTPREQUEST_PROXYSETTING_PRECONFIG = 0x00000000;
const HTTPREQUEST_PROXY_SETTING HTTPREQUEST_PROXYSETTING_DIRECT = 0x00000001;
const HTTPREQUEST_PROXY_SETTING HTTPREQUEST_PROXYSETTING_PROXY = 0x00000002;


extern const IID IID_IWinHttpRequest;


struct SWinHttpSampleGet {
    LPCWSTR szServer;
    LPCWSTR szPath;
    BOOL fUseSSL;
    LPCWSTR szServerUsername;
    LPCWSTR szServerPassword;
    LPCWSTR szProxyUsername;
    LPCWSTR szProxyPassword;
};


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


DLLEXPORT
DWORD WINAPI HttpReadData();


EXTERN_C_END


//////////////////////////////////////////////////////////////////////////////////////////////////
