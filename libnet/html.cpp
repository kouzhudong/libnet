#include "pch.h"
#include "html.h"


SHOWHTMLDIALOGFN * pfnShowHTMLDialog;


//////////////////////////////////////////////////////////////////////////////////////////////////


void ShowHTMLDialogByApi(void)
/*
��ʾ��ҳ����򵥵İ취.
�ٸ���һ����ǵ��ýӿڡ�

������и��������ȥ�������磺�鿴Դ���롣
��ʵֻ��һ��SYSLINK�����Ǹ��ؼ���
���๦�ܺ���ȱ���д����롣

�����޸���MSDN��
������ϸ���ǣ�ShowHTMLDialog Sample Source Page��
��ַ�ǣ�http://www.microsoft.com/en-us/download/details.aspx?id=944
�����ǣ�htmldlg.exe���ɽ�ѹ�ͷš�
���������ʾ���Ǳ��ص�(PE�ļ���һ����Դ)�Լ�����ҳ�������Ի�ȡѡ��/�����Ľ����

��������Ƶĺ������У�
ShowHTMLDialogEx
ShowModelessHTMLDialog

made by correy
made at 2014.07.22
*/
{
    HINSTANCE hinstMSHTML = LoadLibrary(TEXT("MSHTML.DLL"));
    if (hinstMSHTML == NULL) {
        return;// Error loading module -- fail as securely as possible
    }
    
    pfnShowHTMLDialog = (SHOWHTMLDIALOGFN *)GetProcAddress(hinstMSHTML, "ShowHTMLDialog");
    if (pfnShowHTMLDialog) {
        IMoniker * pURLMoniker;
        BSTR bstrURL = SysAllocString(L"http://correy.webs.com");
        CreateURLMoniker(NULL, bstrURL, &pURLMoniker);
        if (pURLMoniker) {
            (*pfnShowHTMLDialog)(NULL, pURLMoniker, NULL, NULL, NULL);
            pURLMoniker->Release();
        }

        SysFreeString(bstrURL);
    }

    FreeLibrary(hinstMSHTML);

    //����Ҳ��ժ��MSDN��
    //IHostDialogHelper* pHDH;
    //IMoniker* pUrlMoniker;
    //BSTR bstrOptions = SysAllocString(L"dialogHeight:30;dialogWidth:40");
    //BSTR bstrPath = SysAllocString(L"c:\\dummy.htm");//c:\\dummy.htm http://correy.webs.com

    //CreateURLMoniker(NULL, bstrPath, &pUrlMoniker);
    //CoCreateInstance(CLSID_HostDialogHelper, NULL, CLSCTX_INPROC, IID_IHostDialogHelper, (void**)&pHDH);
    //pHDH->ShowHTMLDialog(NULL, pUrlMoniker, NULL, bstrOptions, NULL, NULL);//pHDH == 0

    //SysFreeString(bstrPath);
    //SysFreeString(bstrOptions);
    //pUrlMoniker->Release();
    //pHDH->Release();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void show_my_website(HWND hWnd)
{
    HINSTANCE hinstMSHTML = LoadLibrary(TEXT("MSHTML.DLL"));
    if (hinstMSHTML == NULL) {
        return;// Error loading module -- fail as securely as possible
    }

    SHOWHTMLDIALOGFN * pfnShowHTMLDialog;
    pfnShowHTMLDialog = (SHOWHTMLDIALOGFN *)GetProcAddress(hinstMSHTML, "ShowHTMLDialog");
    if (pfnShowHTMLDialog) {
        IMoniker * pURLMoniker;
        BSTR bstrURL = SysAllocString(L"http://correy.webs.com");
        CreateURLMoniker(NULL, bstrURL, &pURLMoniker);
        if (pURLMoniker) {
            (*pfnShowHTMLDialog)(hWnd, pURLMoniker, NULL, NULL, NULL);
            pURLMoniker->Release();
        }

        SysFreeString(bstrURL);
    }

    FreeLibrary(hinstMSHTML);
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_CREATE://���Կ���ˢ�µ���Ϣ��
        show_my_website(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return(DefWindowProc(hWnd, uMsg, wParam, lParam));
    }
    return(0);
}


void Entry()
/*
�Ľ�������£�
���Լ��Ĵ�������ʾһ����ҳ����ʵ���ǵģ������ýӿڣ���

made by correy
made at 2014.07.23
*/
{
    WNDCLASSEX sWndClassEx = {
        48,
        3,
        WindowProc,
        0,
        0,
        GetModuleHandle(0),
        0,
        LoadCursor(0,IDC_ARROW),
        (HBRUSH)6,
        0,
        L"correy",
        0};

    ATOM a = RegisterClassEx(&sWndClassEx);
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
    while (GetMessage(&sMsg, NULL, 0, 0)) {
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
    IHostDialogHelper * pHDH;
    IMoniker * pUrlMoniker;
    BSTR bstrOptions = SysAllocString(L"dialogHeight:30;dialogWidth:40");
    BSTR bstrPath = SysAllocString(L"https://correy.webs.com");

    HRESULT hr = CreateURLMoniker(NULL, bstrPath, &pUrlMoniker);

    hr = CoInitialize(NULL);

    hr = CoCreateInstance(CLSID_HostDialogHelper,
                          NULL,
                          CLSCTX_INPROC,
                          IID_IHostDialogHelper,
                          (void **)&pHDH);

    pHDH->ShowHTMLDialog(NULL, pUrlMoniker, NULL, bstrOptions, NULL, NULL);

    SysFreeString(bstrPath);
    SysFreeString(bstrOptions);
    pUrlMoniker->Release();
    pHDH->Release();
}


//////////////////////////////////////////////////////////////////////////////////////////////////
