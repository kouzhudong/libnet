#include "pch.h"
#include "WinINet.h"

#include <Wininet.h>//Winhttp.h��Wininet.h���ܰ�����ͬһ���ļ��б��룬�ҽ��鲻Ҫ��ͷ�ļ������á�
#pragma comment(lib, "Wininet.lib")


//////////////////////////////////////////////////////////////////////////////////////////////////


int _cdecl GetCookie()
/*
��ȡcookie�ĺ����������Ҽ������ڣ����Ϻ����ҵ�ʾ����
΢���ʾ�����˺�����⣬�ܷ�����

���ջ��ǿ��Լ��󵨵�ʵ����֤�������Լ��ķ��������ԣ�
made by correy
made at 2012.10.20
homepage:http://correy.webs.com
��ǰҲ��֪���⣬��ΪС��һ׮������һ�ˣ�
������ʵ�ֵ�ʱ�򣬷��ֻ����е��鷳��
�ű��ľͲ�˵�ˡ��ܼ򵥡�
*/
{
    //////////////////////////////////////////////////////////////////////////////////////////////
    //����ҾͲ���д�����ˣ��������������Զ���Ϊ��bool get_cookie(IN TCHAR wsz_url, OUT TCHAR wsz_cookie);

    TCHAR szURL[256] = L"http://www.163.com";//ע�⣺http://����ȴ��
    LPTSTR lpszData = 0;//���Ҳ����Ԥ�ȶ����С��������Ƕ�̬��ȡ��
    DWORD dwSize = 0;

    //��ȡ��С��
    bool b = InternetGetCookie(szURL, NULL, lpszData, &dwSize);
    if (!dwSize && b == false) {
        MessageBox(0, L"û��Cookie", szURL, 0);//���Ƕ��У��е�û�С�
        return 0;
    }

    lpszData = new TCHAR[dwSize];

    b = InternetGetCookie(szURL, NULL, lpszData, &dwSize);
    if (b && dwSize) //��ʱ��dwSize��ֵ�������ȡ��һ�롣
    {
        MessageBox(0, lpszData, szURL, 0);
    }

    delete[]lpszData;

    //////////////////////////////////////////////////////////////////////////////////////////////
    //һ�������û��߸ı�Cookie,������������ʽ�����������ʹ�÷��ɳɹ������ܻ��ʹ�á�
    //���Ҳ����дΪ�����������һ�����ڵ����á�
    //����ԭ�ͽ���Ϊ��bool set_cookie(IN TCHAR wsz_url, IN TCHAR wsz_cookie);

    b = InternetSetCookie(szURL, NULL, TEXT("TestData = Test"));//�����ʽ�����һ��������������еȺš�
    b = InternetSetCookie(szURL, NULL, TEXT("TestData = Test; expires = Sat,01-Jan-2015 00:00:00 GMT"));//��ʾ�õġ�

    b = InternetSetCookie(szURL, L"QQ", TEXT("112426112"));  //������Զ��ӵȺš�  
    //b = InternetSetCookie(szURL, NULL, TEXT("QQ = 112426112"));
    b = InternetSetCookie(szURL, NULL, TEXT("QQ = 112426112; expires = Sat,01-Jan-2015 00:00:00 GMT"));

    //b = InternetSetCookie(szURL, L"email", TEXT("kouleguan@hotmail.com"));
    b = InternetSetCookie(szURL, NULL, TEXT("email = XXXXX@hotmail.com"));
    b = InternetSetCookie(szURL, NULL, TEXT("email = XXXXX@hotmail.com; expires = Sat,01-Jan-2015 00:00:00 GMT"));

    //////////////////////////////////////////////////////////////////////////////////////////////
    /*
    ����cookie��λ�ã�

    1.SHGetSpecialFolderPath������CSIDL_COOKIES����Ӧ���ܻ�ȡ��
      ���磺��win 7 32�����ǣ�C:\Users\Administrator\AppData\Roaming\Microsoft\Windows\Cookies
      ���Cookies�ļ���Ĭ�������صġ�
      ��������ִ���׺.txt,�����ֺ�������ˡ�
      �����и�LowĿ¼��

    2.internetѡ��->����ѡ��������ʷ��¼�����ð�ť->internet��ʱ�ļ��ĵ�ǰλ�á�
      ���磺��win 7 32�����ǣ�C:\Users\Administrator\AppData\Local\Microsoft\Windows\Temporary Internet Files��
      ����ļ���Temporary Internet Files��Ĭ��ҳʽ���صġ�
      ������������磺Cookie:administrator@163.com/
      ��ʵ������ļ�ָ�����档Ҳ������Ϊ����Ĳ����ң������ˡ�
    */

    TCHAR sz_cookie[MAX_PATH] = {0};
    b = SHGetSpecialFolderPath(0, sz_cookie, CSIDL_COOKIES, 0);
    if (b) {
        MessageBox(0, sz_cookie, L"cookie��Ŀ¼�ǣ�", 0);//����ֱ�Ӵ�Ŀ¼��
    }

    //////////////////////////////////////////////////////////////////////////////////////////////

    return   0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
