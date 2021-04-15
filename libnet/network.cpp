#include "pch.h"
#include "log.h"
#include "NetApi.h"
#include "Sock.h"
#include "IpHelper.h"
#include "wfp.h"
#include "NetBIOS.h"
#include "spi.h"
#include "Firewall.h"
#include "IpAddr.h"
#include "WinhttpExamples.h"
#include "WinINetExamples.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


int _cdecl main(_In_ int argc, _In_reads_(argc) CHAR * argv[])
{
    //__debugbreak();

    setlocale(LC_CTYPE, ".936");

    int Args;
    LPWSTR * Arglist = CommandLineToArgvW(GetCommandLineW(), &Args);
    if (NULL == Arglist) {
        LOGA(ERROR_LEVEL, "LastError：%d", GetLastError());
        return 0;
    } 

    //EnumWfpInfo(Args, Arglist);
    
    GetAdaptersAddressesInfo(argc, argv);

    //Sessions();
    //GetExtendedUdpTable6();

    LocalFree(Arglist);
}
