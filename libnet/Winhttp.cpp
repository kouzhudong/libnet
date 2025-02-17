#include "pch.h"
#include "Winhttp.h"

#include <Winhttp.h> //Winhttp.h与Wininet.h不能包含在同一个文件中编译，且建议不要在头文件里引用。
#pragma comment(lib, "Winhttp.lib")


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
void WINAPI HttpReadData()
/*
以前接触网络的时候,认为用户层用socket足矣(其实还有比socket更底层的),别的都不用,这样停止了几年.
后来发现有一些操作,还是用高级的好.
这就知道了.WinINet和WinHTTP等.
好像WinHTTP是WinINet的子集(IWinHttpRequest又是WinHTTP的子集),但比WinINet效率高,还可以用于服务器.
以前也知道点,但没有具体深入.
前几天算是开始了,费了一个多月的时间才把下面的小问题搞定.

下面的代码来自:
http://msdn.microsoft.com/en-us/library/windows/desktop/aa384104(v=vs.85).aspx
https://docs.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpreaddata?redirectedfrom=MSDN
等.
并加一些注释.没有精简.

再说一下:
看似简单的几个函数,如果不深刻理解总是出错的.
原因在于示例程序可以运行,修改别的就不行了.

注意参数的设置:如端口,协议等.一定要配合.

下面的代码经过简单的修改,可以成功的访问:
www.microsoft.com
www.baidu.com
www.google.com.hk 注释:访问www.google.com,得到的数据不是我们想要的,可能又转到了:www.google.com.hk
www.126.com
*/
{
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer{};
    BOOL bResults = FALSE;
    HINTERNET hSession = nullptr, hConnect = nullptr, hRequest = nullptr;

    // Use WinHttpOpen to obtain a session handle.
    hSession = WinHttpOpen(
        L"WinHTTP Example/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, // WINHTTP_ACCESS_TYPE_NO_PROXY  WINHTTP_ACCESS_TYPE_DEFAULT_PROXY
        WINHTTP_NO_PROXY_NAME,             //#define WINHTTP_NO_PROXY_NAME     NULL
        WINHTTP_NO_PROXY_BYPASS,
        0);         //#define WINHTTP_NO_PROXY_BYPASS   NULL
    if (hSession) { // Specify an HTTP server.
        DWORD data{};
        dwSize = sizeof(DWORD);

        //我这台电脑上的原始值是:60000ms,设置大点,防止因为这个而出现错误.
        WinHttpSetTimeouts(hSession, 60000000, 60000000, 60000000, 60000000);

        // Use WinHttpQueryOption to retrieve internet options.
        if (WinHttpQueryOption(hSession, WINHTTP_OPTION_CONNECT_TIMEOUT, &data, &dwSize)) {
            printf("Connection timeout: %lu ms\n\n", data); // 60000ms
        } else {
            printf("Error %lu in WinHttpQueryOption.\n", GetLastError());
        }

        // When finished, release the HINTERNET handle.
        // WinHttpCloseHandle(hSession);
        //以上这几行代码也摘抄自msdn .

        hConnect = WinHttpConnect(
            hSession,
            L"correy.webs.com", // www.microsoft.com www.baidu.com www.google.com.hk
                                // 支持L"220.181.112.143"格式.不要加http://和https://
            INTERNET_DEFAULT_HTTP_PORT,
            0); // INTERNET_DEFAULT_HTTP_PORT INTERNET_DEFAULT_HTTPS_PORT  //设置端口,注意要和协议匹配.
    }

    // Create an HTTP request handle.
    if (hConnect)
        hRequest = WinHttpOpenRequest(
            hConnect,
            L"GET",
            nullptr, //可以改为"POST","HEAD".等.
            nullptr, // Pointer to a string that contains the HTTP version. If this parameter is NULL, the function
                     // uses HTTP/1.1.
            WINHTTP_NO_REFERER, //还可以写具体的相对位置.如果没有，可以设置为WINHTTP_NO_REFERER
            WINHTTP_DEFAULT_ACCEPT_TYPES, // see Media Types defined by IANA at
                                          // http://www.iana.org/assignments/media-types/.
            WINHTTP_FLAG_REFRESH); // WINHTTP_FLAG_REFRESH  WINHTTP_FLAG_SECURE 设置协议, 注意要和端口匹配.

    // Send a request.
    if (hRequest)
        bResults = WinHttpSendRequest(hRequest, //这个很费时间.
                                      WINHTTP_NO_ADDITIONAL_HEADERS,
                                      0,
                                      WINHTTP_NO_REQUEST_DATA,
                                      0,
                                      0,
                                      0); //用GetLastError返回的错误码,结合函数说明在头文件里面查,msdn上也有的.

    //具体的查看信息是:http://msdn.microsoft.com/en-us/library/windows/desktop/aa383770(v=vs.85).aspx

    // End the request.
    if (bResults)
        bResults = WinHttpReceiveResponse(hRequest, nullptr); // ERROR_WINHTTP_CANNOT_CONNECT

    // Keep checking for data until there is nothing left.
    if (bResults) {
        do {
            // Check for available data.
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                printf("Error %lu in WinHttpQueryDataAvailable.\n", GetLastError());
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
                printf("Error %lu in WinHttpReadData.\n", GetLastError());
            } else {
                printf("%s", pszOutBuffer);
            }

            delete[] pszOutBuffer; // Free the memory allocated to the buffer.

            // This condition should never be reached since WinHttpQueryDataAvailable
            // reported that there are bits to read.
            if (!dwDownloaded)
                break;
        } while (dwSize > 0);
    } else {
        printf("Error %lu has occurred.\n", GetLastError()); // Report any errors.
    }

    // Close any open handles.
    if (hRequest)
        WinHttpCloseHandle(hRequest);
    if (hConnect)
        WinHttpCloseHandle(hConnect);
    if (hSession)
        WinHttpCloseHandle(hSession);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


#pragma prefast(push)
#pragma prefast(disable : 33005, "XXX")
#pragma prefast(disable : 26812, "XXX")
#pragma warning(disable : 4471)
#import "C:\\Windows\\SysWOW64\\winhttpcom.dll" no_namespace //OK
#pragma prefast(pop)

/*关于这个文件有三种实现的办法：
1。网上搜索相应的文件，http://www.koders.com上面有。
2。导入相应的dll,如：#import <winhttp.dll>
3。编译相应的idl文件。先导入idl,编译，再导入.h文件。
*/
//#include "httprequest.h"


EXTERN_C
DLLEXPORT
int WINAPI IWinHttpRequestSend()
/*
本文改编自:
http://msdn.microsoft.com/en-us/library/aa384045(VS.85).aspx
https://docs.microsoft.com/zh-cn/windows/win32/winhttp/iwinhttprequest-send?redirectedfrom=MSDN

‘vbs代码如下：
Set WinHttpReq = CreateObject("WinHttp.WinHttpRequest.5.1")
WinHttpReq.open "get", "http://correy.webs.com", false
WinHttpReq.Send
WScript.Echo WinHttpReq.ResponseText
'还有PUT，post功能，暂未加上。

made at 2012.04.16
*/
{
    HRESULT hr{}; // variable for return value

    hr = CoInitialize(nullptr); // initialize COM

    IWinHttpRequest * pIWinHttpRequest = nullptr;
    BSTR bstrResponse = nullptr;
    VARIANT varFalse{};
    VARIANT varEmpty{};
    CLSID clsid{};

    VariantInit(&varFalse);
    V_VT(&varFalse) = VT_BOOL;
    V_BOOL(&varFalse) = VARIANT_FALSE;

    VariantInit(&varEmpty);
    V_VT(&varEmpty) = VT_ERROR;

    hr = CLSIDFromProgID(L"WinHttp.WinHttpRequest.5.1", &clsid);
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER, IID_IWinHttpRequest, (void **)&pIWinHttpRequest);
    }
    if (SUCCEEDED(hr)) { // Open WinHttpRequest.
        BSTR bstrMethod = SysAllocString(L"GET");
        BSTR bstrUrl = SysAllocString(L"https://microsoft.com");
        hr = pIWinHttpRequest->Open(bstrMethod, bstrUrl, varFalse);
        SysFreeString(bstrMethod);
        SysFreeString(bstrUrl);
    }
    if (SUCCEEDED(hr)) {                       // Send Request.
        hr = pIWinHttpRequest->Send(varEmpty); //这个很费时间.
    }
    if (SUCCEEDED(hr)) { // Get Response text.
        hr = pIWinHttpRequest->get_ResponseText(&bstrResponse);
    }

    wprintf(L"%.256s", bstrResponse); // Print response to console.

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
DLLEXPORT
void WINAPI GetDefaultProxyConfiguration()
/*
The WinHttpGetDefaultProxyConfiguration function retrieves the default WinHTTP proxy configuration from the
registry.

https://docs.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpgetdefaultproxyconfiguration

在开启翻墙的情况下，这个函数的测试竟然输出为空。
*/
{
    WINHTTP_PROXY_INFO proxyInfo;

    WinHttpGetDefaultProxyConfiguration(&proxyInfo); // Retrieve the default proxy configuration.

    // Display the proxy servers and free memory allocated to this string.
    if (proxyInfo.lpszProxy != nullptr) {
        printf("Proxy server list: %S\n", proxyInfo.lpszProxy);
        GlobalFree(proxyInfo.lpszProxy);
    }

    // Display the bypass list and free memory allocated to this string.
    if (proxyInfo.lpszProxyBypass != nullptr) {
        printf("Proxy bypass list: %S\n", proxyInfo.lpszProxyBypass);
        GlobalFree(proxyInfo.lpszProxyBypass);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
void WINAPI GetIEProxyConfigForCurrentUser()
/*
The WinHttpGetIEProxyConfigForCurrentUser function retrieves the Internet Explorer proxy configuration for the
current user.

In Internet Explorer, the proxy settings are found on the Connections tab of the Tools / Internet Options menu
option. Proxy settings are configured on a per-connection basis; that is, the proxy settings for a LAN connection
are separate from those for a dial-up or VPN connection. WinHttpGetIEProxyConfigForCurrentUser returns the proxy
settings for the current active connection.

This function is useful in client applications running in network environments in which the Web Proxy
Auto-Discovery (WPAD) protocol is not implemented (meaning that no Proxy Auto-Configuration file is available). If
a PAC file is not available, then the WinHttpGetProxyForUrl function fails. The
WinHttpGetIEProxyConfigForCurrentUser function can be used as a fall-back mechanism to discover a workable proxy
configuration by retrieving the user's proxy configuration in Internet Explorer.

This function should not be used in a service process that does not impersonate a logged-on user.
If the caller does not impersonate a logged on user, WinHTTP attempts to retrieve the Internet Explorer settings
for the current service process: for example, the local service or the network service. If the Internet Explorer
settings are not configured for these system accounts, the call to WinHttpGetIEProxyConfigForCurrentUser will fail.

The caller must free the lpszProxy, lpszProxyBypass and lpszAutoConfigUrl strings in the
WINHTTP_CURRENT_USER_IE_PROXY_CONFIG structure if they are non-NULL. Use GlobalFree to free the strings.

https://docs.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpgetieproxyconfigforcurrentuser

输出如下：
AutoDetect:false.
AutoConfigUrl:(null).
Proxy:http=127.0.0.1:19394;https=127.0.0.1:19394;socks=127.0.0.1:19393.
ProxyBypass:<local>.

看来，这个代码还有点用处，不过，输出信息需要解析。
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
DLLEXPORT
void WINAPI DetectAutoProxyConfigUrl()
/*
经测试：此代码输出错误。
*/
{
    LPWSTR ppwstrAutoConfigUrl = nullptr;
    BOOL ret = WinHttpDetectAutoProxyConfigUrl(WINHTTP_AUTO_DETECT_TYPE_DHCP, &ppwstrAutoConfigUrl);
    if (!ret) {
        printf("LastError:%#lx.\n", GetLastError()); // LastError:0x2f94.
    }

    if (ppwstrAutoConfigUrl) {
        GlobalFree(ppwstrAutoConfigUrl);
    }

    ret = WinHttpDetectAutoProxyConfigUrl(WINHTTP_AUTO_DETECT_TYPE_DNS_A, &ppwstrAutoConfigUrl);
    if (!ret) {
        printf("LastError:%#lx.\n", GetLastError()); // LastError:0x2f94.
    }

    if (ppwstrAutoConfigUrl) {
        GlobalFree(ppwstrAutoConfigUrl);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
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
    HINTERNET hHttpSession = nullptr;
    HINTERNET hConnect = nullptr;
    HINTERNET hRequest = nullptr;
    WINHTTP_AUTOPROXY_OPTIONS AutoProxyOptions{};
    WINHTTP_PROXY_INFO ProxyInfo{};
    DWORD cbProxyInfoSize = sizeof(ProxyInfo);

    ZeroMemory(&AutoProxyOptions, sizeof(AutoProxyOptions));
    ZeroMemory(&ProxyInfo, sizeof(ProxyInfo));

    // Create the WinHTTP session.
    hHttpSession = WinHttpOpen(L"WinHTTP AutoProxy Sample/1.0", WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hHttpSession)
        goto Exit; // Exit if WinHttpOpen failed.

    // Create the WinHTTP connect handle.
    hConnect = WinHttpConnect(hHttpSession, L"www.microsoft.com", INTERNET_DEFAULT_HTTP_PORT, 0);
    if (!hConnect)
        goto Exit; // Exit if WinHttpConnect failed.

    // Create the HTTP request handle.
    hRequest = WinHttpOpenRequest(hConnect, L"GET", L"ms.htm", L"HTTP/1.1", WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!hRequest)
        goto Exit; // Exit if WinHttpOpenRequest failed.

    // Set up the autoproxy call.

    // Use auto-detection because the Proxy Auto-Config URL is not known.
    AutoProxyOptions.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;

    // Use DHCP and DNS-based auto-detection.
    AutoProxyOptions.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP | WINHTTP_AUTO_DETECT_TYPE_DNS_A;

    // If obtaining the PAC script requires NTLM/Negotiate
    // authentication, then automatically supply the client domain credentials.
    AutoProxyOptions.fAutoLogonIfChallenged = TRUE;

    // Call WinHttpGetProxyForUrl with our target URL.
    // If auto-proxy succeeds, then set the proxy info on the request handle.
    // If auto-proxy fails, ignore the error
    // and attempt to send the HTTP request directly to the
    // target server (using the default WINHTTP_ACCESS_TYPE_NO_PROXY
    // configuration, which the requesthandle will inherit from the session).
    if (WinHttpGetProxyForUrl(hHttpSession, L"https://www.microsoft.com/ms.htm", &AutoProxyOptions, &ProxyInfo)) {
        // A proxy configuration was found, set it on the request handle.
        if (!WinHttpSetOption(hRequest, WINHTTP_OPTION_PROXY, &ProxyInfo, cbProxyInfoSize)) {
            goto Exit; // Exit if setting the proxy info failed.
        }
    } else {
        printf("LastError:%#x.\n", GetLastError()); //无论开启自动检测与否，都返回ERROR_WINHTTP_AUTODETECTION_FAILED

        // If Step 1 fails, with ERROR_WINHTTP_LOGIN_FAILURE,
        // then call WinHttpGetProxyForUrl with the fAutoLogonIfChallenged member set to TRUE.
        // https://learn.microsoft.com/en-us/windows/win32/winhttp/autoproxy-cache
    }

    // Send the request.
    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        goto Exit; // Exit if WinHttpSendRequest failed.
    }

    // Wait for the response.
    if (!WinHttpReceiveResponse(hRequest, nullptr))
        goto Exit;

    // A response has been received, then process it.
    // (omitted)

Exit:
    // Clean up the WINHTTP_PROXY_INFO structure.
    if (ProxyInfo.lpszProxy != nullptr)
        GlobalFree(ProxyInfo.lpszProxy);

    if (ProxyInfo.lpszProxyBypass != nullptr)
        GlobalFree(ProxyInfo.lpszProxyBypass);

    // Close the WinHTTP handles.
    if (hRequest != nullptr)
        WinHttpCloseHandle(hRequest);

    if (hConnect != nullptr)
        WinHttpCloseHandle(hConnect);

    if (hHttpSession != nullptr)
        WinHttpCloseHandle(hHttpSession);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
void WINAPI Sessions()
/*
* WinHTTP Sessions Overview
*
* 这其实就是打开一个网页。
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
    LPSTR pszOutBuffer{};
    BOOL bResults = FALSE;
    HINTERNET hSession = nullptr, hConnect = nullptr, hRequest = nullptr;

    // Use WinHttpOpen to obtain a session handle.
    hSession = WinHttpOpen(L"WinHTTP Example/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    // Specify an HTTP server.
    if (hSession)
        hConnect = WinHttpConnect(hSession, L"www.microsoft.com", INTERNET_DEFAULT_HTTPS_PORT, 0);

    // Create an HTTP request handle.
    if (hConnect)
        hRequest = WinHttpOpenRequest(hConnect, L"GET", nullptr, nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

    // Send a request.
    if (hRequest)
        bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

    // End the request.
    if (bResults)
        bResults = WinHttpReceiveResponse(hRequest, nullptr);

    // Keep checking for data until there is nothing left.
    if (bResults) {
        do {
            // Check for available data.
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
                printf("Error %lu in WinHttpQueryDataAvailable.\n", GetLastError());

            // Allocate space for the buffer.
            pszOutBuffer = new char[(size_t)dwSize + 1];
            if (!pszOutBuffer) {
                printf("Out of memory\n");
                dwSize = 0;
            } else { // Read the data.
                ZeroMemory(pszOutBuffer, (size_t)dwSize + 1);
                if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
                    printf("Error %lu in WinHttpReadData.\n", GetLastError());
                else
                    printf("%s", pszOutBuffer);

                delete[] pszOutBuffer; // Free the memory allocated to the buffer.
            }
        } while (dwSize > 0);
    }

    // Report any errors.
    if (!bResults)
        printf("Error %lu has occurred.\n", GetLastError());

    // Close any open handles.
    if (hRequest)
        WinHttpCloseHandle(hRequest);
    if (hConnect)
        WinHttpCloseHandle(hConnect);
    if (hSession)
        WinHttpCloseHandle(hSession);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


//#include <windows.h>
//#include <winhttp.h>
//#include <stdio.h>
//
//#pragma comment(lib, "winhttp.lib")


DWORD ChooseAuthScheme(DWORD dwSupportedSchemes)
{
    //  It is the server's responsibility only to accept
    //  authentication schemes that provide a sufficient
    //  level of security to protect the servers resources.
    //
    //  The client is also obligated only to use an authentication
    //  scheme that adequately protects its username and password.
    //
    //  Thus, this sample code does not use Basic authentication
    //  becaus Basic authentication exposes the client's username
    //  and password to anyone monitoring the connection.

    if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_NEGOTIATE)
        return WINHTTP_AUTH_SCHEME_NEGOTIATE;
    else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_NTLM)
        return WINHTTP_AUTH_SCHEME_NTLM;
    else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_PASSPORT)
        return WINHTTP_AUTH_SCHEME_PASSPORT;
    else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_DIGEST)
        return WINHTTP_AUTH_SCHEME_DIGEST;
    else
        return 0;
}


void WinHttpAuthSample(IN SWinHttpSampleGet * pGetRequest)
/*
Example: Retrieving a Document

The following sample code attempts to retrieve a specified document from an HTTP server.
The status code is retrieved from the response to determine if authentication is required.
If a 200 status code is found, the document is available.
If a status code of 401 or 407 is found, authentication is required before the document can be retrieved.
For any other status code, an error message is displayed.
See HTTP Status Codes for a list of possible status codes.

https://docs.microsoft.com/en-us/windows/win32/winhttp/authentication-in-winhttp
*/
{
    DWORD dwStatusCode = 0;
    DWORD dwSupportedSchemes{};
    DWORD dwFirstScheme{};
    DWORD dwSelectedScheme{};
    DWORD dwTarget{};
    DWORD dwLastStatus = 0;
    DWORD dwSize = sizeof(DWORD);
    BOOL bResults = FALSE;
    BOOL bDone = FALSE;
    DWORD dwProxyAuthScheme = 0;
    HINTERNET hSession = nullptr, hConnect = nullptr, hRequest = nullptr;

    // Use WinHttpOpen to obtain a session handle.
    hSession = WinHttpOpen(L"WinHTTP Example/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    INTERNET_PORT nPort = (pGetRequest->fUseSSL) ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT;

    // Specify an HTTP server.
    if (hSession)
        hConnect = WinHttpConnect(hSession, pGetRequest->szServer, nPort, 0);

    // Create an HTTP request handle.
    if (hConnect)
        hRequest = WinHttpOpenRequest(hConnect, 
                                      L"GET",
                                      pGetRequest->szPath,
                                      nullptr,
                                      WINHTTP_NO_REFERER,
                                      WINHTTP_DEFAULT_ACCEPT_TYPES,
                                      (pGetRequest->fUseSSL) ? WINHTTP_FLAG_SECURE : 0);

    // Continue to send a request until status code is not 401 or 407.
    if (hRequest == nullptr)
        bDone = TRUE;

    while (!bDone) {
        //  If a proxy authentication challenge was responded to, reset
        //  those credentials before each SendRequest, because the proxy
        //  may require re-authentication after responding to a 401 or
        //  to a redirect. If you don't, you can get into a 407-401-407-401- loop.
        if (dwProxyAuthScheme != 0)
            bResults = WinHttpSetCredentials(hRequest, 
                                             WINHTTP_AUTH_TARGET_PROXY,
                                             dwProxyAuthScheme,
                                             pGetRequest->szProxyUsername,
                                             pGetRequest->szProxyPassword,
                                             nullptr);
        // Send a request.
        bResults =
            WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

        // End the request.
        if (bResults)
            bResults = WinHttpReceiveResponse(hRequest, nullptr);

        // Resend the request in case of
        // ERROR_WINHTTP_RESEND_REQUEST error.
        if (!bResults && GetLastError() == ERROR_WINHTTP_RESEND_REQUEST)
            continue;

        // Check the status code.
        if (bResults)
            bResults = WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, nullptr, &dwStatusCode, &dwSize, nullptr);

        if (bResults) {
            switch (dwStatusCode) {
            case 200:
                // The resource was successfully retrieved.
                // You can use WinHttpReadData to read the contents of the server's response.
                printf("The resource was successfully retrieved.\n");
                bDone = TRUE;
                break;
            case 401:
                // The server requires authentication.
                printf(" The server requires authentication. Sending credentials...\n");

                // Obtain the supported and preferred schemes.
                bResults = WinHttpQueryAuthSchemes(hRequest, &dwSupportedSchemes, &dwFirstScheme, &dwTarget);
                if (bResults) { // Set the credentials before resending the request.
                    dwSelectedScheme = ChooseAuthScheme(dwSupportedSchemes);
                    if (dwSelectedScheme == 0)
                        bDone = TRUE;
                    else
                        bResults = WinHttpSetCredentials(hRequest, dwTarget, dwSelectedScheme, pGetRequest->szServerUsername, pGetRequest->szServerPassword, nullptr);
                }

                // If the same credentials are requested twice, abort the request.
                // For simplicity, this sample does not check for a repeated sequence of status codes.
                if (dwLastStatus == 401)
                    bDone = TRUE;

                break;
            case 407:
                // The proxy requires authentication.
                printf("The proxy requires authentication.  Sending credentials...\n");

                // Obtain the supported and preferred schemes.
                bResults = WinHttpQueryAuthSchemes(hRequest, &dwSupportedSchemes, &dwFirstScheme, &dwTarget);
                if (bResults) // Set the credentials before resending the request.
                    dwProxyAuthScheme = ChooseAuthScheme(dwSupportedSchemes);

                // If the same credentials are requested twice, abort the request.
                // For simplicity, this sample does not check for a repeated sequence of status codes.
                if (dwLastStatus == 407)
                    bDone = TRUE;

                break;
            default:
                // The status code does not indicate success.
                printf("Error. Status code %lu returned.\n", dwStatusCode);
                bDone = TRUE;
            }
        }

        dwLastStatus = dwStatusCode; // Keep track of the last status code.

        if (!bResults)
            bDone = TRUE; // If there are any errors, break out of the loop.
    }

    // Report any errors.
    if (!bResults) {
        DWORD dwLastError = GetLastError();
        printf("Error %lu has occurred.\n", dwLastError);
    }

    // Close any open handles.
    if (hRequest)
        WinHttpCloseHandle(hRequest);
    if (hConnect)
        WinHttpCloseHandle(hConnect);
    if (hSession)
        WinHttpCloseHandle(hSession);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


VOID CALLBACK ProxyChangeNotificationCallback(_In_ ULONGLONG ullFlags, _In_ PVOID pvContext)
/*

这里给出的信息少，看来还得再次获取代理信息。

https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nc-winhttp-winhttp_proxy_change_callback
*/
{
    UNREFERENCED_PARAMETER(pvContext);

    printf("Flags:%llu.\r\n", ullFlags);
}


// WINHTTP_PROXY_CHANGE_REGISTRATION_HANDLE hRegistration;


EXTERN_C
DLLEXPORT
DWORD WINAPI RegisterProxyChangeNotification()
/*
功能：监控代理设置的变化。

Windows 10的Winhttp.dll没有导出这个函数。

既然Windows有这个功能，且实现了，一定有监控的办法，
它的实现，无怪乎存储在文件（注册表，数据库）中。
经promon的监控（写文件，写键值，排除system，保留svchost.exe），发现：
可以排除文件监控。
HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings下的几个键值，如：ProxyServer等。
HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\iphlpsvc\Parameters\ProxyMgr下的几个GUID键。
HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\NlaSvc\Parameters\Internet\ManualProxies。

https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpregisterproxychangenotification
*/
{
    DWORD ret = 0;

#pragma prefast(push)
#pragma prefast(disable : 6387, ""_Param_(3) " 可以是 " 0 "")
    // ret = WinHttpRegisterProxyChangeNotification(WINHTTP_PROXY_NOTIFY_CHANGE,
    //                                             ProxyChangeNotificationCallback,
    //                                             nullptr,
    //                                             &hRegistration);
#pragma prefast(pop)

    return ret;
}


EXTERN_C
DLLEXPORT
DWORD WINAPI UnregisterProxyChangeNotification()
/*

https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpunregisterproxychangenotification
*/
{
    DWORD ret = 0;

    // if (hRegistration) {
    //    ret = WinHttpUnregisterProxyChangeNotification(hRegistration);
    //    hRegistration = nullptr;
    //}

    return ret;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
DWORD WINAPI SetDefaultProxyConfiguration()
/*
Use of WinHttpSetDefaultProxyConfiguration is deprecated on Windows 8.1 and newer.
Most proxy configurations are not supported by WinHttpSetDefaultProxyConfiguration, nor does it support proxy
authentication. Instead, use WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY with WinHttpOpen.

The WinHttpSetDefaultProxyConfiguration function sets the default WinHTTP proxy configuration in the registry.

Remarks
The default proxy configuration set by WinHttpSetDefaultProxyConfiguration can be overridden for an existing
WinHTTP session by calling WinHttpSetOption and specifying the WINHTTP_OPTION_PROXY flag. The default proxy
configuration can be overridden for a new session by specifying the configuration with the WinHttpOpen function.

The dwAccessType member of the WINHTTP_PROXY_INFO structure pointed to by pProxyInfo should be set to
WINHTTP_ACCESS_TYPE_NAMED_PROXY if a proxy is specified. Otherwise, it should be set to
WINHTTP_ACCESS_TYPE_DEFAULT_PROXY.

Any new sessions created after calling this function use the new default proxy configuration.

Even when WinHTTP is used in asynchronous mode (that is, when WINHTTP_FLAG_ASYNC has been set in WinHttpOpen), this
function operates synchronously. The return value indicates success or failure. To get extended error information,
call GetLastError.

https://learn.microsoft.com/zh-cn/windows/win32/api/winhttp/nf-winhttp-winhttpsetdefaultproxyconfiguration
*/
{
    WINHTTP_PROXY_INFO proxyInfo;

    // Allocate memory for string members.
    proxyInfo.lpszProxy = new WCHAR[25];
    proxyInfo.lpszProxyBypass = new WCHAR[25];

    // Set the members of the proxy info structure.
    proxyInfo.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
    swprintf_s(proxyInfo.lpszProxy, 25, L"proxy_server"); // 127.0.0.1:9919
    swprintf_s(proxyInfo.lpszProxyBypass, 25, L"<local>");

    // Set the default proxy configuration.
    if (WinHttpSetDefaultProxyConfiguration(&proxyInfo)) {
        printf("Proxy Configuration Set.\n");
        //代码成功了，见WinHttpGetDefaultProxyConfiguration的输出，但是有效没？
    } else {
        printf("LastError:%#lx.\n", GetLastError());
    }

    // Free memory allocated to the strings.
    delete[] proxyInfo.lpszProxy;
    delete[] proxyInfo.lpszProxyBypass;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


//#include "httprequest.h"
//#include <httprequest.idl>


EXTERN_C
DLLEXPORT
int WINAPI SetProxy()
/*


https://learn.microsoft.com/en-us/windows/win32/winhttp/iwinhttprequest-setproxy
*/
/*
// HttpRequest SetCredentials flags.
HTTPREQUEST_PROXYSETTING_DEFAULT   = 0;
HTTPREQUEST_PROXYSETTING_PRECONFIG = 0;
HTTPREQUEST_PROXYSETTING_DIRECT    = 1;
HTTPREQUEST_PROXYSETTING_PROXY     = 2;

// Instantiate a WinHttpRequest object.
var WinHttpReq = new ActiveXObject("WinHttp.WinHttpRequest.5.1");

// Use proxy_server for all requests outside of
// the microsoft.com domain.
WinHttpReq.SetProxy( HTTPREQUEST_PROXYSETTING_PROXY,
                     "proxy_server:80",
                     "*.microsoft.com");

// Initialize an HTTP request.
WinHttpReq.Open("GET", "https://www.microsoft.com", false);

// Send the HTTP request.
WinHttpReq.Send();

// Display the response text.
WScript.Echo( WinHttpReq.ResponseText);
*/
{
    // Variable for return value
    HRESULT hr = CoInitialize(nullptr); // Initialize COM

    IWinHttpRequest * pIWinHttpRequest = nullptr;

    BSTR bstrResponse = nullptr;
    VARIANT varFalse{};
    VARIANT varEmpty{};
    VARIANT varProxy{};
    VARIANT varUrl{};

    CLSID clsid{};

    VariantInit(&varFalse);
    V_VT(&varFalse) = VT_BOOL;
    V_BOOL(&varFalse) = VARIANT_FALSE;

    VariantInit(&varEmpty);
    V_VT(&varEmpty) = VT_ERROR;

    VariantInit(&varProxy);
    VariantInit(&varUrl);

    hr = CLSIDFromProgID(L"WinHttp.WinHttpRequest.5.1", &clsid);
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER, IID_IWinHttpRequest, (void **)&pIWinHttpRequest);
    }
    if (SUCCEEDED(hr)) { // Specify proxy and URL.
        varProxy.vt = VT_BSTR;
        varProxy.bstrVal = SysAllocString(L"127.0.0.1:9910"); //必须是已经存在的合理的配置，否则Send出问题，且get_ResponseText执行。
        varUrl.vt = VT_BSTR;
        varUrl.bstrVal = SysAllocString(L"*.microsoft.com");
        hr = pIWinHttpRequest->SetProxy(HTTPREQUEST_PROXYSETTING_PROXY, varProxy, varUrl);
    }
    if (SUCCEEDED(hr)) { // Open WinHttpRequest.
        BSTR bstrMethod = SysAllocString(L"GET");
        BSTR bstrUrl = SysAllocString(L"https://microsoft.com");
        hr = pIWinHttpRequest->Open(bstrMethod, bstrUrl, varFalse);
        SysFreeString(bstrMethod);
        SysFreeString(bstrUrl);
    }
    if (SUCCEEDED(hr)) { // Send Request.
        hr = pIWinHttpRequest->Send(varEmpty);
    }
    if (SUCCEEDED(hr)) { // Get Response text.
        hr = pIWinHttpRequest->get_ResponseText(&bstrResponse);
    }
    if (SUCCEEDED(hr)) { // Print the response to a console.
        wprintf(L"%.256s", bstrResponse);
    }

    // Release memory.
    if (pIWinHttpRequest)
        pIWinHttpRequest->Release();
    if (varProxy.bstrVal)
        SysFreeString(varProxy.bstrVal);
    if (varUrl.bstrVal)
        SysFreeString(varUrl.bstrVal);
    if (bstrResponse)
        SysFreeString(bstrResponse);

    CoUninitialize();
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


EXTERN_C
DLLEXPORT
int WINAPI WriteProxySettings()
/*
功能：设置代理。

经procmon监控代理设置操作和IDA(winhttp.dll)分析出：WinHttpWriteProxySettings是有效的代理设置，经测试也是有效的。
这里对应：设置->网络和Internet->代理。Windows 10环境。
*/
{
    WINHTTP_PROXY_SETTINGS WinHttpProxySettings{};
    HINTERNET hSession{};
    BOOL fForceUpdate = true;
    DWORD ret = 0;

    hSession = WinHttpOpen(
        L"WinHTTP Example/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, // WINHTTP_ACCESS_TYPE_NO_PROXY  WINHTTP_ACCESS_TYPE_DEFAULT_PROXY
        WINHTTP_NO_PROXY_NAME,             //#define WINHTTP_NO_PROXY_NAME     NULL
        WINHTTP_NO_PROXY_BYPASS,
        0);          //#define WINHTTP_NO_PROXY_BYPASS   NULL
    if (!hSession) { // Specify an HTTP server.
        printf("LastError:%#lx.\n", GetLastError());
        return 0;
    }

    // WinHttpProxySettings.pwszConnectionName = new WCHAR[MAX_PATH];
    // WinHttpProxySettings.pwszProxy = new WCHAR[MAX_PATH];
    // WinHttpProxySettings.pwszProxyBypass = new WCHAR[MAX_PATH];
    // WinHttpProxySettings.pwszAutoconfigUrl = new WCHAR[MAX_PATH];
    // WinHttpProxySettings.pwszAutoconfigSecondaryUrl = new WCHAR[MAX_PATH];
    // WinHttpProxySettings.pwszLastKnownGoodAutoConfigUrl = new WCHAR[MAX_PATH];
    // WinHttpProxySettings.pNetworkKeys = new WINHTTP_PROXY_NETWORKING_KEY;
    WinHttpProxySettings.dwStructSize = sizeof(_WINHTTP_PROXY_SETTINGS); //这个很重要哦！

    DWORD pdwSettingsVersion = 1;
    BOOL pfDefaultSettingsAreReturned = true;
    ret = WinHttpReadProxySettings(hSession,
                                   nullptr,
                                   true,
                                   true,
                                   &pdwSettingsVersion,
                                   &pfDefaultSettingsAreReturned,
                                   &WinHttpProxySettings); //这家伙会自动申请内存。
    if (0 != ret) {
        printf("LastError:%#lx.\n", GetLastError());
        WinHttpCloseHandle(hSession);
        return ret;
    }

    printf("CurrentSettingsVersion:%lu.\r\n", WinHttpProxySettings.dwCurrentSettingsVersion);
    printf("Proxy:%ls.\r\n", WinHttpProxySettings.pwszProxy);
    printf("ProxyBypass:%ls.\r\n", WinHttpProxySettings.pwszProxyBypass);

    ret = WinHttpWriteProxySettings(hSession, fForceUpdate, &WinHttpProxySettings);
    if (0 != ret) {
        printf("LastError:%#lx.\n", GetLastError());
    }

    WinHttpFreeProxySettings(&WinHttpProxySettings);

    if (hSession)
        WinHttpCloseHandle(hSession);

    return ret;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
