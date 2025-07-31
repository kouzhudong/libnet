#include "pch.h"
#include "html.h"


SHOWHTMLDIALOGFN * pfnShowHTMLDialog;


//////////////////////////////////////////////////////////////////////////////////////////////////


void ShowHTMLDialogByApi(void)
/*
显示网页的最简单的办法.
再复杂一点的是调用接口。

这个还有更多的属性去开发，如：查看源代码。
其实只有一个SYSLINK可用那个控件。
更多功能和优缺点有待深入。

本文修改自MSDN。
更加详细的是：ShowHTMLDialog Sample Source Page，
地址是：http://www.microsoft.com/en-us/download/details.aspx?id=944
名字是：htmldlg.exe，可解压释放。
这个好像显示的是本地的(PE文件的一个资源)自己的网页。还可以获取选择/操作的结果。

更多的类似的函数还有：
ShowHTMLDialogEx
ShowModelessHTMLDialog

made by correy
made at 2014.07.22
*/
{
    HINSTANCE hinstMSHTML = LoadLibrary(TEXT("MSHTML.DLL"));
    if (hinstMSHTML == nullptr) {
        return; // Error loading module -- fail as securely as possible
    }

    pfnShowHTMLDialog = reinterpret_cast<SHOWHTMLDIALOGFN *>(GetProcAddress(hinstMSHTML, "ShowHTMLDialog"));
    if (pfnShowHTMLDialog) {
        IMoniker * pURLMoniker{};
        BSTR bstrURL = SysAllocString(L"http://correy.webs.com");
        CreateURLMoniker(nullptr, bstrURL, &pURLMoniker);
        if (pURLMoniker) {
            (*pfnShowHTMLDialog)(nullptr, pURLMoniker, nullptr, nullptr, nullptr);
            pURLMoniker->Release();
        }

        SysFreeString(bstrURL);
    }

    FreeLibrary(hinstMSHTML);

    //以下也是摘自MSDN。
    // IHostDialogHelper* pHDH;
    // IMoniker* pUrlMoniker;
    // BSTR bstrOptions = SysAllocString(L"dialogHeight:30;dialogWidth:40");
    // BSTR bstrPath = SysAllocString(L"c:\\dummy.htm");//c:\\dummy.htm http://correy.webs.com

    // CreateURLMoniker(nullptr, bstrPath, &pUrlMoniker);
    // CoCreateInstance(CLSID_HostDialogHelper, nullptr, CLSCTX_INPROC, IID_IHostDialogHelper, (void**)&pHDH);
    // pHDH->ShowHTMLDialog(nullptr, pUrlMoniker, nullptr, bstrOptions, nullptr, nullptr);//pHDH == 0

    // SysFreeString(bstrPath);
    // SysFreeString(bstrOptions);
    // pUrlMoniker->Release();
    // pHDH->Release();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void show_my_website(HWND hWnd)
{
    HINSTANCE hinstMSHTML = LoadLibrary(TEXT("MSHTML.DLL"));
    if (hinstMSHTML == nullptr) {
        return; // Error loading module -- fail as securely as possible
    }

    pfnShowHTMLDialog = reinterpret_cast<SHOWHTMLDIALOGFN *>(GetProcAddress(hinstMSHTML, "ShowHTMLDialog"));
    if (pfnShowHTMLDialog) {
        IMoniker * pURLMoniker{};
        BSTR bstrURL = SysAllocString(L"http://correy.webs.com");
        CreateURLMoniker(nullptr, bstrURL, &pURLMoniker);
        if (pURLMoniker) {
            (*pfnShowHTMLDialog)(hWnd, pURLMoniker, nullptr, nullptr, nullptr);
            pURLMoniker->Release();
        }

        SysFreeString(bstrURL);
    }

    FreeLibrary(hinstMSHTML);
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_CREATE: //可以考虑刷新的消息。
        show_my_website(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return (DefWindowProc(hWnd, uMsg, wParam, lParam));
    }

    return (0);
}


void Entry()
/*
改进版的如下：
在自己的窗口中显示一个网页（其实不是的，还得用接口）。

made by correy
made at 2014.07.23
*/
{
    WNDCLASSEX sWndClassEx = {sizeof(WNDCLASSEX),
                              3,
                              WindowProc,
                              0,
                              0,
                              GetModuleHandle(0),
                              0,
                              LoadCursor(0, IDC_ARROW),
                              reinterpret_cast<HBRUSH>(6),
                              0,
                              L"correy",
                              0};

    ATOM a = RegisterClassEx(&sWndClassEx);
    _ASSERTE(a);
    HWND hWindow = CreateWindowEx(0,
                                  L"correy",
                                  L"correy",
                                  WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  0,
                                  0,
                                  GetModuleHandle(0),
                                  0);
    ShowWindow(hWindow, 1);

    MSG sMsg;
    while (GetMessage(&sMsg, nullptr, 0, 0)) {
        DispatchMessage(&sMsg);
    }

    ExitProcess(0);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void ShowHTMLDialogByCom()
/*

https://docs.microsoft.com/en-us/previous-versions/windows/internet-explorer/ie-developer/platform-apis/aa753252(v=vs.85)
*/
{
    IHostDialogHelper * pHDH{};
    IMoniker * pUrlMoniker{};
    BSTR bstrOptions = SysAllocString(L"dialogHeight:30;dialogWidth:40");
    BSTR bstrPath = SysAllocString(L"https://correy.webs.com");

    HRESULT hr = CreateURLMoniker(nullptr, bstrPath, &pUrlMoniker);

    hr = CoInitialize(nullptr);

    hr = CoCreateInstance(CLSID_HostDialogHelper, nullptr, CLSCTX_INPROC, IID_IHostDialogHelper, reinterpret_cast<void **>(&pHDH));

    pHDH->ShowHTMLDialog(nullptr, pUrlMoniker, nullptr, bstrOptions, nullptr, nullptr);

    SysFreeString(bstrPath);
    SysFreeString(bstrOptions);
    pUrlMoniker->Release();
    pHDH->Release();
}


//////////////////////////////////////////////////////////////////////////////////////////////////
