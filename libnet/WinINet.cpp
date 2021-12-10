#include "pch.h"
#include "WinINet.h"

#include <Wininet.h>//Winhttp.h与Wininet.h不能包含在同一个文件中编译，且建议不要在头文件里引用。
#pragma comment(lib, "Wininet.lib")


//////////////////////////////////////////////////////////////////////////////////////////////////


int _cdecl GetCookie()
/*
获取cookie的函数困扰了我几个星期，网上很少找到示例。
微软的示例让人很难理解，很繁琐。

最终还是靠自己大胆的实验来证明，用自己的方法，所以：
made by correy
made at 2012.10.20
homepage:http://correy.webs.com
以前也许知道这，认为小事一桩，不削一顾，
可真正实现的时候，发现还是有点麻烦。
脚本的就不说了。很简单。
*/
{
    //////////////////////////////////////////////////////////////////////////////////////////////
    //这个我就不再写函数了，函数的声明可以定义为：bool get_cookie(IN TCHAR wsz_url, OUT TCHAR wsz_cookie);

    TCHAR szURL[256] = L"http://www.163.com";//注意：http://不可却。
    LPTSTR lpszData = 0;//这个也可以预先定义大小，但最好是动态获取。
    DWORD dwSize = 0;

    //获取大小。
    bool b = InternetGetCookie(szURL, NULL, lpszData, &dwSize);
    if (!dwSize && b == false) {
        MessageBox(0, L"没有Cookie", szURL, 0);//并非都有，有的没有。
        return 0;
    }

    lpszData = new TCHAR[dwSize];

    b = InternetGetCookie(szURL, NULL, lpszData, &dwSize);
    if (b && dwSize) //此时的dwSize的值是上面获取的一半。
    {
        MessageBox(0, lpszData, szURL, 0);
    }

    delete[]lpszData;

    //////////////////////////////////////////////////////////////////////////////////////////////
    //一下是设置或者改变Cookie,可以有两种形式，但必须配对使用方可成功。不能混合使用。
    //这个也可以写为函数，里面多一个日期的设置。
    //函数原型建议为：bool set_cookie(IN TCHAR wsz_url, IN TCHAR wsz_cookie);

    b = InternetSetCookie(szURL, NULL, TEXT("TestData = Test"));//这个方式，最后一个参数里面必须有等号。
    b = InternetSetCookie(szURL, NULL, TEXT("TestData = Test; expires = Sat,01-Jan-2015 00:00:00 GMT"));//显示用的。

    b = InternetSetCookie(szURL, L"QQ", TEXT("112426112"));  //这个会自动加等号。  
    //b = InternetSetCookie(szURL, NULL, TEXT("QQ = 112426112"));
    b = InternetSetCookie(szURL, NULL, TEXT("QQ = 112426112; expires = Sat,01-Jan-2015 00:00:00 GMT"));

    //b = InternetSetCookie(szURL, L"email", TEXT("kouleguan@hotmail.com"));
    b = InternetSetCookie(szURL, NULL, TEXT("email = XXXXX@hotmail.com"));
    b = InternetSetCookie(szURL, NULL, TEXT("email = XXXXX@hotmail.com; expires = Sat,01-Jan-2015 00:00:00 GMT"));

    //////////////////////////////////////////////////////////////////////////////////////////////
    /*
    关于cookie的位置，

    1.SHGetSpecialFolderPath函数的CSIDL_COOKIES参数应该能获取。
      例如：在win 7 32里面是：C:\Users\Administrator\AppData\Roaming\Microsoft\Windows\Cookies
      这个Cookies文件夹默认是隐藏的。
      这里的名字带后缀.txt,但名字好像加密了。
      里面有个Low目录。

    2.internet选项->常规选项卡的浏览历史记录的设置按钮->internet临时文件的当前位置。
      例如：在win 7 32里面是：C:\Users\Administrator\AppData\Local\Microsoft\Windows\Temporary Internet Files。
      这个文件夹Temporary Internet Files，默认页式隐藏的。
      这里的名字例如：Cookie:administrator@163.com/
      其实这里的文件指向上面。也许是因为上面的不好找，加密了。
    */

    TCHAR sz_cookie[MAX_PATH] = {0};
    b = SHGetSpecialFolderPath(0, sz_cookie, CSIDL_COOKIES, 0);
    if (b) {
        MessageBox(0, sz_cookie, L"cookie的目录是：", 0);//或者直接打开目录。
    }

    //////////////////////////////////////////////////////////////////////////////////////////////

    return   0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
