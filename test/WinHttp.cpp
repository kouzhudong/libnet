#include "WinHttp.h"


void TestWinHttp()
{
    //RegisterProxyChangeNotification();
    GetDefaultProxyConfiguration();
    GetIEProxyConfigForCurrentUser();
    DetectAutoProxyConfigUrl();
    //GetProxyForUrl();
    SetDefaultProxyConfiguration();
    //SetProxy();
    WriteProxySettings();
}
