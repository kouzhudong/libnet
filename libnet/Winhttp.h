#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////


//C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\um\httprequest.idl
typedef long HTTPREQUEST_PROXY_SETTING;
const HTTPREQUEST_PROXY_SETTING HTTPREQUEST_PROXYSETTING_DEFAULT = 0x00000000;
const HTTPREQUEST_PROXY_SETTING HTTPREQUEST_PROXYSETTING_PRECONFIG = 0x00000000;
const HTTPREQUEST_PROXY_SETTING HTTPREQUEST_PROXYSETTING_DIRECT = 0x00000001;
const HTTPREQUEST_PROXY_SETTING HTTPREQUEST_PROXYSETTING_PROXY = 0x00000002;


const IID IID_IWinHttpRequest = {0x06f29373,0x5c5a,0x4b54,{0xb0, 0x25, 0x6e, 0xf1, 0xbf, 0x8a, 0xbf, 0x0e}};


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


__declspec(dllexport)
void WINAPI HttpReadData();


EXTERN_C_END


//////////////////////////////////////////////////////////////////////////////////////////////////
