#include "pch.h"
#include "WebBrowserApp.h"


/*
����ȡ��Ϊ��IWebBrowserApp.Cpp

���������кܶ��ְ취���磺
1.URLDownloadToFile����URLDownloadToCacheFile
2.HttpOpenRequest
3.WinHttpReadData
4.IWinHttpRequest
5.�ű���"WinHttp.WinHttpRequest.5.1"
6.SOCKET���Լ�дhttpͷ��
7.�����ӿ�(XMLHttpRequest)���籾�ĵģ����п��ܡ�
8.����
9.������������������ؽӿڡ�

���Ƶ��ϴ�����Ҳ������Ĵ�ͬС��

���ȴ�����������ҳɹ���

��Σ�����һ��asp�ļ������ڴ�����յ������ݣ�����Ϊdata.asp,�������£�

<%
Dim vntPostedData, lngCount

Response.AddHeader "Request-Method", Request.ServerVariables("REQUEST_METHOD")

lngCount = Request.TotalBytes
if (lngCount <> 0) then
vntPostedData = Request.BinaryRead(lngCount)
Response.BinaryWrite vntPostedData
end if
%>

���ϴ���ժ�ԣ�Microsoft SDKs\Windows\v7.1\Samples\web\winhttp\winhttppostsample\data.asp��

���Ѵ��ļ��ŵ�iis�������У����磺C:\inetpub\wwwroot\data.asp.
���ļ����ط���������������Ĵ򿪣���Ϊ�������Ĵ��ǿ�����ɶ�ġ�

��������Ĵ��룬�ȿ��Կ����㷢�͵����ݡ�

����Ĵ����޸��ԣ�
����Զ����ͱ����ݵ� Internet Explorer
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
    pWBApp->put_Visible(VARIANT_TRUE);//��ʾ��

    Error:
    if (bstrURL) SysFreeString(bstrURL);
    if (bstrHeaders) SysFreeString(bstrHeaders);
    VariantClear(&vPostData);
    if (pWBApp) pWBApp->Release();
    CoUninitialize();
}
