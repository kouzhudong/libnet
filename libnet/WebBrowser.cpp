#include "pch.h"
#include "WebBrowser.h"


/*
本文取名为：IWebBrowserApp.Cpp

下载数据有很多种办法，如：
1.URLDownloadToFile或者URLDownloadToCacheFile
2.HttpOpenRequest
3.WinHttpReadData
4.IWinHttpRequest
5.脚本的"WinHttp.WinHttpRequest.5.1"
6.SOCKET，自己写http头。
7.其他接口(XMLHttpRequest)，如本文的，就有可能。
8.驱动
9.其他。如浏览器的下载接口。

类似的上传数据也和上面的大同小异

首先搭建服务器，并且成功。

其次，建立一个asp文件，用于处理接收到的数据，命名为data.asp,内容如下：

<%
Dim vntPostedData, lngCount

Response.AddHeader "Request-Method", Request.ServerVariables("REQUEST_METHOD")

lngCount = Request.TotalBytes
if (lngCount <> 0) then
vntPostedData = Request.BinaryRead(lngCount)
Response.BinaryWrite vntPostedData
end if
%>

以上代码摘自：Microsoft SDKs\Windows\v7.1\Samples\web\winhttp\winhttppostsample\data.asp。

并把此文件放到iis服务器中，例如：C:\inetpub\wwwroot\data.asp.
此文件不必放在浏览器中主动的打开，因为你主动的打开是看不到啥的。

运行下面的代码，既可以看到你发送的数据。

下面的代码修改自：
如何自动发送表单数据到 Internet Explorer
http://support.microsoft.com/kb/q167658
*/


HRESULT GetPostData(LPVARIANT pvPostData)
// Pack some data into a SAFEARRAY of BYTEs. Return in a VARIANT
{
    HRESULT hr;
    LPSAFEARRAY psa;
    LPCTSTR cszPostData = L"made by correy";//FName=Matt&Flavor=Mocha+Chip
    UINT cElems = lstrlen(cszPostData) * sizeof(wchar_t);
    LPSTR pPostData;

    if (!pvPostData) {
        return E_POINTER;
    }

    VariantInit(pvPostData);

    psa = SafeArrayCreateVector(VT_UI1, 0, cElems);
    if (!psa) {
        return E_OUTOFMEMORY;
    }

    hr = SafeArrayAccessData(psa, (LPVOID *)&pPostData);
    memcpy(pPostData, cszPostData, cElems);
    hr = SafeArrayUnaccessData(psa);

    V_VT(pvPostData) = VT_ARRAY | VT_UI1;
    V_ARRAY(pvPostData) = psa;
    return NOERROR;
}


void WebBrowserApp()
{
    HRESULT hr;
    IWebBrowserApp * pWBApp = NULL; // Derived from IWebBrowser
    BSTR bstrURL = NULL, bstrHeaders = NULL;
    VARIANT vFlags = {0},
        vTargetFrameName = {0},
        vPostData = {0},
        vHeaders = {0};

    if (FAILED(hr = CoInitialize(NULL))) {
        return;
    }

    if (FAILED(hr = CoCreateInstance(CLSID_InternetExplorer,
                                     NULL, CLSCTX_SERVER,
                                     IID_IWebBrowserApp,
                                     (LPVOID *)&pWBApp))) {
        goto Error;
    }

    bstrURL = SysAllocString(L"http://127.0.0.1/data.asp");//http://<server>/scripts/navpost.asp
    if (!bstrURL) {
        goto Error;
    }

    bstrHeaders = SysAllocString(L"Content-Type: application/x-www-form-urlencoded\r\n");
    if (!bstrHeaders) {
        goto Error;
    }

    V_VT(&vHeaders) = VT_BSTR;
    V_BSTR(&vHeaders) = bstrHeaders;

    hr = GetPostData(&vPostData);
    hr = pWBApp->Navigate(bstrURL, &vFlags, &vTargetFrameName, &vPostData, &vHeaders);
    pWBApp->put_Visible(VARIANT_TRUE);//显示。

Error:
    if (bstrURL) SysFreeString(bstrURL);
    if (bstrHeaders) SysFreeString(bstrHeaders);
    VariantClear(&vPostData);
    if (pWBApp) pWBApp->Release();
    CoUninitialize();
}
