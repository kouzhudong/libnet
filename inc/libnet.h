#pragma once

#pragma warning(disable:28251)
#pragma warning(disable:28301)


/////////////////////////////////////////////////////////////////////////////////////////////////
//һЩϵͳ��ͷ�ļ��Ϳ�İ�����


//#define _WIN32_WINNT 0x0501

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#ifndef UNICODE
#define UNICODE
#endif

#include <Winsock2.h>
#include <windows.h>
#include <strsafe.h>
#include <assert.h>
#include <crtdbg.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <winioctl.h>
#include <string.h>
#include <fltuser.h>
#include <locale.h>
#include <Lmserver.h>
#include <stdarg.h>
#include <wincrypt.h>
#include <intrin.h>
#include <TlHelp32.h>
#include <aclapi.h>
#include <VersionHelpers.h>
#include <ShlDisp.h>
#include <Shlobj.h>
#include <Softpub.h>
#include <mscat.h>
#include <WinUser.h>
#include <direct.h>
#include <sddl.h>
#include <ws2tcpip.h>
#include <fwpsu.h>
#include <atlbase.h>
#include <mbnapi.h>
#include <iostream>
#include <netfw.h>
#include <atlcomcli.h>
#include <objbase.h>
#include <oleauto.h>
#include <atlconv.h>
#define _WS2DEF_
#include <mstcpip.h>
#include <Intshcut.h>
//#include <winternl.h>
#include <SubAuth.h>
//#include <NTSecAPI.h>
//#include <ntdef.h>
//#include <netioapi.h>
//#include <LsaLookup.h>
#include <atlstr.h>
#include <comutil.h>
#include <wbemidl.h>
#include <dbt.h>
#include <lm.h>
#include <winnetwk.h>
#include <ws2spi.h>
#include <comdef.h>

#include <initguid.h> //ע��ǰ��˳�򡣾�̬����UUID�õģ�����error LNK2001��
#include <usbioctl.h>
#include <usbiodef.h>
//#include <usbctypes.h>
#include <intsafe.h>
#include <specstrings.h>
#include <usb.h>
#include <usbuser.h>

#include <wincon.h> 
#include <time.h> 
#include <fwpmu.h>
#include <conio.h>
#include <nb30.h>

#pragma comment(lib, "fwpuclnt.lib") 
#pragma comment(lib, "Rpcrt4.lib")

#pragma comment(lib, "mpr.lib")

#pragma comment( lib, "ole32.lib" )
#pragma comment( lib, "oleaut32.lib" )
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "Version.lib") 
//#pragma comment (lib,"Url.lib")
#pragma comment(lib, "wbemuuid.lib")

#include <bcrypt.h>
#pragma comment (lib, "Bcrypt.lib")

#include <shellapi.h>
#pragma comment (lib, "Shell32.lib")

#include <ncrypt.h>
#pragma comment (lib, "Ncrypt.lib")

#include <wintrust.h>
#pragma comment (lib, "wintrust.lib")

#include <Setupapi.h>
#pragma comment (lib,"Setupapi.lib")

#include <Shlwapi.h>
#pragma comment (lib,"Shlwapi.lib")

#include <DbgHelp.h>
#pragma comment (lib,"DbgHelp.lib")

#include <psapi.h>
#pragma comment(lib, "Psapi.lib")

#include <Sfc.h>
#pragma comment(lib, "Sfc.lib")

//#include <winsock.h>
#pragma comment(lib, "Ws2_32.lib")

#pragma comment(lib,"Netapi32.lib")

#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

#include <Wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")

#include <Userenv.h>
#pragma comment(lib, "Userenv.lib")

#include <Sensapi.h>
#pragma comment (lib,"Sensapi.lib")

#include <string>
#include <list>
#include <regex>
#include <map>
#include <set>

using namespace std;


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_START


__declspec(dllimport)
int WINAPI GetAdaptersAddressesInfo(int argc, char ** argv);

__declspec(dllimport)
void WINAPI RegistersNotifyIpInterfaceChange();

__declspec(dllimport)
void WINAPI DeregisterNotifyIpInterfaceChange();

__declspec(dllimport)
void WINAPI RegistersNotifyNetworkConnectivityHintChange();

__declspec(dllimport)
void WINAPI DeregisterNotifyRouteChange2();

__declspec(dllimport)
void WINAPI RegistersNotifyRouteChange2();

__declspec(dllimport)
void WINAPI DeregisterNotifyStableUnicastIpAddressTable();

__declspec(dllimport)
void WINAPI RegistersNotifyStableUnicastIpAddressTable();

__declspec(dllimport)
void WINAPI DeregisterNotifyTeredoPortChange();

__declspec(dllimport)
void WINAPI RegistersNotifyTeredoPortChange();

__declspec(dllimport)
void WINAPI DeregisterNotifyUnicastIpAddressChange();

__declspec(dllimport)
void WINAPI RegistersNotifyUnicastIpAddressChange();


//////////////////////////////////////////////////////////////////////////////////////////////////


__declspec(dllimport)
void WINAPI HttpReadData();

__declspec(dllimport)
int WINAPI IWinHttpRequestSend();

__declspec(dllimport)
void WINAPI GetDefaultProxyConfiguration();

__declspec(dllimport)
void WINAPI GetIEProxyConfigForCurrentUser();

__declspec(dllimport)
void WINAPI DetectAutoProxyConfigUrl();

__declspec(dllimport)
void WINAPI GetProxyForUrl();

__declspec(dllimport)
void WINAPI Sessions();


//////////////////////////////////////////////////////////////////////////////////////////////////


__declspec(dllimport)
void WINAPI EnumTcpTable();

__declspec(dllimport)
void WINAPI EnumEntityArray();


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C_END


//////////////////////////////////////////////////////////////////////////////////////////////////
