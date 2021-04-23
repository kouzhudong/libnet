#include "pch.h"
#include "WinhttpExamples.h"

#include <Winhttp.h> //Winhttp.h��Wininet.h���ܰ�����ͬһ���ļ��б��룬�ҽ��鲻Ҫ��ͷ�ļ������á�
#pragma comment(lib, "Winhttp.lib")


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
__declspec(dllexport)
void WINAPI HttpReadData()
/*
��ǰ�Ӵ������ʱ��,��Ϊ�û�����socket����(��ʵ���б�socket���ײ��),��Ķ�����,����ֹͣ�˼���.
����������һЩ����,�����ø߼��ĺ�.
���֪����.WinINet��WinHTTP��.
����WinHTTP��WinINet���Ӽ�(IWinHttpRequest����WinHTTP���Ӽ�),����WinINetЧ�ʸ�,���������ڷ�����.
��ǰҲ֪����,��û�о�������.
ǰ�������ǿ�ʼ��,����һ�����µ�ʱ��Ű������С����㶨.

����Ĵ�������:
http://msdn.microsoft.com/en-us/library/windows/desktop/aa384104(v=vs.85).aspx
https://docs.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpreaddata?redirectedfrom=MSDN
��.
����һЩע��.û�о���.

��˵һ��:
���Ƽ򵥵ļ�������,��������������ǳ����.
ԭ������ʾ�������������,�޸ı�ľͲ�����.

ע�����������:��˿�,Э���.һ��Ҫ���.

����Ĵ��뾭���򵥵��޸�,���Գɹ��ķ���:
www.microsoft.com
www.baidu.com
www.google.com.hk ע��:����www.google.com,�õ������ݲ���������Ҫ��,������ת����:www.google.com.hk
www.126.com
*/
{
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    BOOL  bResults = FALSE;
    HINTERNET  hSession = NULL, hConnect = NULL, hRequest = NULL;

    // Use WinHttpOpen to obtain a session handle.
    hSession = WinHttpOpen(L"WinHTTP Example/1.0",
                           WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, //WINHTTP_ACCESS_TYPE_NO_PROXY  WINHTTP_ACCESS_TYPE_DEFAULT_PROXY
                           WINHTTP_NO_PROXY_NAME, //#define WINHTTP_NO_PROXY_NAME     NULL
                           WINHTTP_NO_PROXY_BYPASS,
                           0);//#define WINHTTP_NO_PROXY_BYPASS   NULL                       
    if (hSession) {// Specify an HTTP server.
        DWORD data;
        DWORD dwSize = sizeof(DWORD);

        //����̨�����ϵ�ԭʼֵ��:60000ms,���ô��,��ֹ��Ϊ��������ִ���.
        WinHttpSetTimeouts(hSession, 60000000, 60000000, 60000000, 60000000);

        // Use WinHttpQueryOption to retrieve internet options.
        if (WinHttpQueryOption(hSession, WINHTTP_OPTION_CONNECT_TIMEOUT, &data, &dwSize)) {
            printf("Connection timeout: %u ms\n\n", data); //60000ms
        } else {
            printf("Error %u in WinHttpQueryOption.\n", GetLastError());
        }

        // When finished, release the HINTERNET handle.
        // WinHttpCloseHandle(hSession);
        //�����⼸�д���Ҳժ����msdn .

        hConnect = WinHttpConnect(hSession,
                                  L"correy.webs.com", //www.microsoft.com www.baidu.com www.google.com.hk ֧��L"220.181.112.143"��ʽ.��Ҫ��http://��https://
                                  INTERNET_DEFAULT_HTTP_PORT,
                                  0); //INTERNET_DEFAULT_HTTP_PORT INTERNET_DEFAULT_HTTPS_PORT  //���ö˿�,ע��Ҫ��Э��ƥ��.
    }

    // Create an HTTP request handle.
    if (hConnect)
        hRequest = WinHttpOpenRequest(hConnect,
                                      L"GET",
                                      NULL,//���Ը�Ϊ"POST","HEAD".��.
                                      NULL, //Pointer to a string that contains the HTTP version. If this parameter is NULL, the function uses HTTP/1.1.
                                      WINHTTP_NO_REFERER, //������д��������λ��.���û�У���������ΪWINHTTP_NO_REFERER
                                      WINHTTP_DEFAULT_ACCEPT_TYPES, //see Media Types defined by IANA at http://www.iana.org/assignments/media-types/.
                                      WINHTTP_FLAG_REFRESH);//WINHTTP_FLAG_REFRESH  WINHTTP_FLAG_SECURE ����Э��, ע��Ҫ�Ͷ˿�ƥ��.

                                  // Send a request.
    if (hRequest)
        bResults = WinHttpSendRequest(hRequest,//����ܷ�ʱ��.
                                      WINHTTP_NO_ADDITIONAL_HEADERS,
                                      0,
                                      WINHTTP_NO_REQUEST_DATA,
                                      0,
                                      0,
                                      0); //��GetLastError���صĴ�����,��Ϻ���˵����ͷ�ļ������,msdn��Ҳ�е�.
    int x = ERROR_WINHTTP_CANNOT_CONNECT; //goto definition�õ�.
    //����Ĳ鿴��Ϣ��:http://msdn.microsoft.com/en-us/library/windows/desktop/aa383770(v=vs.85).aspx

    // End the request.
    if (bResults)
        bResults = WinHttpReceiveResponse(hRequest, NULL); //ERROR_WINHTTP_CANNOT_CONNECT

    // Keep checking for data until there is nothing left.
    if (bResults) {
        do {
            // Check for available data.
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
                break;
            }

            // No more available data.
            if (!dwSize)
                break;

            // Allocate space for the buffer.
            pszOutBuffer = new char[(size_t)dwSize + 1];
            if (!pszOutBuffer) {
                printf("Out of memory\n");
                break;
            }

            // Read the Data.
            ZeroMemory(pszOutBuffer, (size_t)dwSize + 1);
            if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
                printf("Error %u in WinHttpReadData.\n", GetLastError());
            } else {
                printf("%s", pszOutBuffer);
            }

            delete[] pszOutBuffer;// Free the memory allocated to the buffer.

            // This condition should never be reached since WinHttpQueryDataAvailable
            // reported that there are bits to read.
            if (!dwDownloaded)
                break;
        } while (dwSize > 0);
    } else {
        printf("Error %d has occurred.\n", GetLastError());// Report any errors.
    }

    // Close any open handles.
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


#pragma prefast(push)
#pragma prefast(disable: 33005, "XXX")
#pragma prefast(disable: 26812, "XXX")
#import "C:\\Windows\\SysWOW64\\winhttpcom.dll"  no_namespace //OK
#pragma prefast(pop)

/*��������ļ�������ʵ�ֵİ취��
1������������Ӧ���ļ���http://www.koders.com�����С�
2��������Ӧ��dll,�磺#import <winhttp.dll>
3��������Ӧ��idl�ļ����ȵ���idl,���룬�ٵ���.h�ļ���
*/
//#include "httprequest.h"

const IID IID_IWinHttpRequest = {0x06f29373,0x5c5a,0x4b54,{0xb0, 0x25, 0x6e, 0xf1, 0xbf, 0x8a, 0xbf, 0x0e}};


EXTERN_C
__declspec(dllexport)
int WINAPI IWinHttpRequestSend()
/*
���ĸı���:
http://msdn.microsoft.com/en-us/library/aa384045(VS.85).aspx
https://docs.microsoft.com/zh-cn/windows/win32/winhttp/iwinhttprequest-send?redirectedfrom=MSDN

��vbs�������£�
Set WinHttpReq = CreateObject("WinHttp.WinHttpRequest.5.1")
WinHttpReq.open "get", "http://correy.webs.com", false
WinHttpReq.Send
WScript.Echo WinHttpReq.ResponseText
'����PUT��post���ܣ���δ���ϡ�

made at 2012.04.16
*/
{
    HRESULT    hr;// variable for return value

    hr = CoInitialize(NULL);// initialize COM

    IWinHttpRequest * pIWinHttpRequest = NULL;
    BSTR            bstrResponse = NULL;
    VARIANT         varFalse;
    VARIANT         varEmpty;
    CLSID           clsid;

    VariantInit(&varFalse);
    V_VT(&varFalse) = VT_BOOL;
    V_BOOL(&varFalse) = VARIANT_FALSE;

    VariantInit(&varEmpty);
    V_VT(&varEmpty) = VT_ERROR;

    hr = CLSIDFromProgID(L"WinHttp.WinHttpRequest.5.1", &clsid);
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(clsid,
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IWinHttpRequest,
                              (void **)&pIWinHttpRequest);
    }
    if (SUCCEEDED(hr)) {    // Open WinHttpRequest.
        BSTR bstrMethod = SysAllocString(L"GET");
        BSTR bstrUrl = SysAllocString(L"https://microsoft.com");
        hr = pIWinHttpRequest->Open(bstrMethod, bstrUrl, varFalse);
        SysFreeString(bstrMethod);
        SysFreeString(bstrUrl);
    }
    if (SUCCEEDED(hr)) {    // Send Request.
        hr = pIWinHttpRequest->Send(varEmpty);//����ܷ�ʱ��.
    }
    if (SUCCEEDED(hr)) {    // Get Response text.
        hr = pIWinHttpRequest->get_ResponseText(&bstrResponse);
    }

    wprintf(L"%.256s", bstrResponse);// Print response to console.

    // Release memory.
    if (pIWinHttpRequest)
        pIWinHttpRequest->Release();
    if (bstrResponse)
        SysFreeString(bstrResponse);

    CoUninitialize();
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
__declspec(dllexport)
void WINAPI GetDefaultProxyConfiguration()
/*
The WinHttpGetDefaultProxyConfiguration function retrieves the default WinHTTP proxy configuration from the registry.

https://docs.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpgetdefaultproxyconfiguration

�ڿ�����ǽ������£���������Ĳ��Ծ�Ȼ���Ϊ�ա�
*/
{
    WINHTTP_PROXY_INFO proxyInfo;

    WinHttpGetDefaultProxyConfiguration(&proxyInfo);// Retrieve the default proxy configuration.

    // Display the proxy servers and free memory allocated to this string.
    if (proxyInfo.lpszProxy != NULL) {
        printf("Proxy server list: %S\n", proxyInfo.lpszProxy);
        GlobalFree(proxyInfo.lpszProxy);
    }

    // Display the bypass list and free memory allocated to this string.
    if (proxyInfo.lpszProxyBypass != NULL) {
        printf("Proxy bypass list: %S\n", proxyInfo.lpszProxyBypass);
        GlobalFree(proxyInfo.lpszProxyBypass);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
__declspec(dllexport)
void WINAPI GetIEProxyConfigForCurrentUser()
/*
The WinHttpGetIEProxyConfigForCurrentUser function retrieves the Internet Explorer proxy configuration for the current user.

In Internet Explorer, the proxy settings are found on the Connections tab of the Tools / Internet Options menu option.
Proxy settings are configured on a per-connection basis; that is,
the proxy settings for a LAN connection are separate from those for a dial-up or VPN connection.
WinHttpGetIEProxyConfigForCurrentUser returns the proxy settings for the current active connection.

This function is useful in client applications running in network environments in which the Web Proxy Auto-Discovery (WPAD) protocol is not implemented (meaning that no Proxy Auto-Configuration file is available).
If a PAC file is not available, then the WinHttpGetProxyForUrl function fails.
The WinHttpGetIEProxyConfigForCurrentUser function can be used as a fall-back mechanism to discover a workable proxy configuration by retrieving the user's proxy configuration in Internet Explorer.

This function should not be used in a service process that does not impersonate a logged-on user.
If the caller does not impersonate a logged on user, WinHTTP attempts to retrieve the Internet Explorer settings for the current service process:
for example, the local service or the network service.
If the Internet Explorer settings are not configured for these system accounts, the call to WinHttpGetIEProxyConfigForCurrentUser will fail.

The caller must free the lpszProxy, lpszProxyBypass and lpszAutoConfigUrl strings in the WINHTTP_CURRENT_USER_IE_PROXY_CONFIG structure if they are non-NULL.
Use GlobalFree to free the strings.

https://docs.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpgetieproxyconfigforcurrentuser

������£�
AutoDetect:false.
AutoConfigUrl:(null).
Proxy:http=127.0.0.1:19394;https=127.0.0.1:19394;socks=127.0.0.1:19393.
ProxyBypass:<local>.

������������뻹�е��ô��������������Ϣ��Ҫ������
*/
{
    WINHTTP_CURRENT_USER_IE_PROXY_CONFIG pProxyConfig;
    BOOL ret = WinHttpGetIEProxyConfigForCurrentUser(&pProxyConfig);
    _ASSERTE(ret);

    printf("AutoDetect:%s.\n", pProxyConfig.fAutoDetect ? "true" : "false");
    printf("AutoConfigUrl:%ls.\n", pProxyConfig.lpszAutoConfigUrl);
    printf("Proxy:%ls.\n", pProxyConfig.lpszProxy);
    printf("ProxyBypass:%ls.\n", pProxyConfig.lpszProxyBypass);

    if (pProxyConfig.lpszAutoConfigUrl) {
        GlobalFree(pProxyConfig.lpszAutoConfigUrl);
    }

    if (pProxyConfig.lpszProxy) {
        GlobalFree(pProxyConfig.lpszProxy);
    }

    if (pProxyConfig.lpszProxyBypass) {
        GlobalFree(pProxyConfig.lpszProxyBypass);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
__declspec(dllexport)
void WINAPI DetectAutoProxyConfigUrl()
/*
�����ԣ��˴����������
*/
{
    LPWSTR ppwstrAutoConfigUrl = NULL;
    BOOL ret = WinHttpDetectAutoProxyConfigUrl(WINHTTP_AUTO_DETECT_TYPE_DHCP, &ppwstrAutoConfigUrl);
    if (!ret) {
        printf("LastError:%#x.\n", GetLastError());//LastError:0x2f94.
    }

    if (ppwstrAutoConfigUrl) {
        GlobalFree(ppwstrAutoConfigUrl);
    }

    ret = WinHttpDetectAutoProxyConfigUrl(WINHTTP_AUTO_DETECT_TYPE_DNS_A, &ppwstrAutoConfigUrl);
    if (!ret) {
        printf("LastError:%#x.\n", GetLastError());//LastError:0x2f94.
    }

    if (ppwstrAutoConfigUrl) {
        GlobalFree(ppwstrAutoConfigUrl);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
__declspec(dllexport)
void WINAPI GetProxyForUrl()
/*
The following example code uses autoproxy.
It sets up an HTTP GET request by first creating the WinHTTP session connect and request handles.
The WinHttpOpen call specifies WINHTTP_ACCESS_TYPE_NO_PROXY for the initial proxy configuration,
to indicate that requests are sent directly to the target server by default.
Using autoproxy, it then sets the proxy configuration directly on the request handle.

https://docs.microsoft.com/en-us/windows/win32/winhttp/winhttp-autoproxy-api
*/
{
    HINTERNET hHttpSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    WINHTTP_AUTOPROXY_OPTIONS  AutoProxyOptions;
    WINHTTP_PROXY_INFO         ProxyInfo;
    DWORD                      cbProxyInfoSize = sizeof(ProxyInfo);

    ZeroMemory(&AutoProxyOptions, sizeof(AutoProxyOptions));
    ZeroMemory(&ProxyInfo, sizeof(ProxyInfo));

    // Create the WinHTTP session.
    hHttpSession = WinHttpOpen(L"WinHTTP AutoProxy Sample/1.0",
                               WINHTTP_ACCESS_TYPE_NO_PROXY,
                               WINHTTP_NO_PROXY_NAME,
                               WINHTTP_NO_PROXY_BYPASS,
                               0);
    if (!hHttpSession)
        goto Exit;// Exit if WinHttpOpen failed.

    // Create the WinHTTP connect handle.
    hConnect = WinHttpConnect(hHttpSession, L"www.microsoft.com", INTERNET_DEFAULT_HTTP_PORT, 0);
    if (!hConnect)
        goto Exit;// Exit if WinHttpConnect failed.

    // Create the HTTP request handle.
    hRequest = WinHttpOpenRequest(hConnect,
                                  L"GET",
                                  L"ms.htm",
                                  L"HTTP/1.1",
                                  WINHTTP_NO_REFERER,
                                  WINHTTP_DEFAULT_ACCEPT_TYPES,
                                  0);
    if (!hRequest)
        goto Exit;// Exit if WinHttpOpenRequest failed.

    // Set up the autoproxy call.

    // Use auto-detection because the Proxy Auto-Config URL is not known.
    AutoProxyOptions.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;

    // Use DHCP and DNS-based auto-detection.
    AutoProxyOptions.dwAutoDetectFlags =
        WINHTTP_AUTO_DETECT_TYPE_DHCP | WINHTTP_AUTO_DETECT_TYPE_DNS_A;

    // If obtaining the PAC script requires NTLM/Negotiate
    // authentication, then automatically supply the client domain credentials.
    AutoProxyOptions.fAutoLogonIfChallenged = TRUE;

    // Call WinHttpGetProxyForUrl with our target URL.
    // If auto-proxy succeeds, then set the proxy info on the request handle. 
    // If auto-proxy fails, ignore the error 
    // and attempt to send the HTTP request directly to the 
    // target server (using the default WINHTTP_ACCESS_TYPE_NO_PROXY 
    // configuration, which the requesthandle will inherit from the session).
    if (WinHttpGetProxyForUrl(hHttpSession,
                              L"https://www.microsoft.com/ms.htm",
                              &AutoProxyOptions,
                              &ProxyInfo)) {
        // A proxy configuration was found, set it on the request handle.
        if (!WinHttpSetOption(hRequest, WINHTTP_OPTION_PROXY, &ProxyInfo, cbProxyInfoSize)) {
            goto Exit;// Exit if setting the proxy info failed.
        }
    }

    // Send the request.
    if (!WinHttpSendRequest(hRequest,
                            WINHTTP_NO_ADDITIONAL_HEADERS,
                            0,
                            WINHTTP_NO_REQUEST_DATA,
                            0,
                            0,
                            NULL)) {
        goto Exit;// Exit if WinHttpSendRequest failed.
    }

    // Wait for the response.
    if (!WinHttpReceiveResponse(hRequest, NULL))
        goto Exit;

    // A response has been received, then process it.
    // (omitted)

Exit:
    // Clean up the WINHTTP_PROXY_INFO structure.
    if (ProxyInfo.lpszProxy != NULL)
        GlobalFree(ProxyInfo.lpszProxy);

    if (ProxyInfo.lpszProxyBypass != NULL)
        GlobalFree(ProxyInfo.lpszProxyBypass);

    // Close the WinHTTP handles.
    if (hRequest != NULL)
        WinHttpCloseHandle(hRequest);

    if (hConnect != NULL)
        WinHttpCloseHandle(hConnect);

    if (hHttpSession != NULL)
        WinHttpCloseHandle(hHttpSession);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
__declspec(dllexport)
void WINAPI Sessions()
/*
* WinHTTP Sessions Overview
*
* ����ʵ���Ǵ�һ����ҳ��
*
The following sample code shows how to download a resource with secure transaction semantics.
The sample code initializes the WinHTTP application programming interface (API), selects a target HTTPS server,
and then opens and sends a request for this secure resource.
WinHttpQueryDataAvailable is used with the request handle to determine how much data is available for download,
and then WinHttpReadData is used to read that data.
This process is repeated until the entire document has been retrieved and displayed.

https://docs.microsoft.com/en-us/windows/win32/winhttp/winhttp-sessions-overview
*/
{
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    BOOL  bResults = FALSE;
    HINTERNET  hSession = NULL, hConnect = NULL, hRequest = NULL;

    // Use WinHttpOpen to obtain a session handle.
    hSession = WinHttpOpen(L"WinHTTP Example/1.0",
                           WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                           WINHTTP_NO_PROXY_NAME,
                           WINHTTP_NO_PROXY_BYPASS,
                           0);

    // Specify an HTTP server.
    if (hSession)
        hConnect = WinHttpConnect(hSession, L"www.microsoft.com", INTERNET_DEFAULT_HTTPS_PORT, 0);

    // Create an HTTP request handle.
    if (hConnect)
        hRequest = WinHttpOpenRequest(hConnect,
                                      L"GET",
                                      NULL,
                                      NULL,
                                      WINHTTP_NO_REFERER,
                                      WINHTTP_DEFAULT_ACCEPT_TYPES,
                                      WINHTTP_FLAG_SECURE);

    // Send a request.
    if (hRequest)
        bResults = WinHttpSendRequest(hRequest,
                                      WINHTTP_NO_ADDITIONAL_HEADERS,
                                      0,
                                      WINHTTP_NO_REQUEST_DATA,
                                      0,
                                      0, 
                                      0);


    // End the request.
    if (bResults)
        bResults = WinHttpReceiveResponse(hRequest, NULL);

    // Keep checking for data until there is nothing left.
    if (bResults) {
        do {
            // Check for available data.
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
                printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());

            // Allocate space for the buffer.
            pszOutBuffer = new char[(size_t)dwSize + 1];
            if (!pszOutBuffer) {
                printf("Out of memory\n");
                dwSize = 0;
            } else {
                // Read the data.
                ZeroMemory(pszOutBuffer, (size_t)dwSize + 1);

                if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
                    printf("Error %u in WinHttpReadData.\n", GetLastError());
                else
                    printf("%s", pszOutBuffer);

                delete[] pszOutBuffer;// Free the memory allocated to the buffer.
            }
        } while (dwSize > 0);
    }

    // Report any errors.
    if (!bResults)
        printf("Error %d has occurred.\n", GetLastError());

    // Close any open handles.
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
