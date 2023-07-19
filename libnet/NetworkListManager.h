/*
你能感知到你电脑上的网络变化吗？
如：插上网线，链接wifi，使用手机网络等。
能监控到吗？能枚举到吗？
完美的网络程序怎能少了这些功能。

参考：
1.Windows-classic-samples\Samples\NetworkListManager
2.https://learn.microsoft.com/en-us/windows/win32/api/netlistmgr/nn-netlistmgr-inetworklistmanager
3.

核心实现文件应该在：netprofm.dll里。
*/


/*
监控网络变动的三个类：
iNetworkConnectionEvents 
iNetworkEvents 
iNetworkListManagerEvents

https://review.jami.net/plugins/gitiles/jami-client-qt/+/a4787e2a45bd4c6abbfe08849f7bcd5ad50a9b8c/src/connectivitymonitor.cpp
https://review.jami.net/plugins/gitiles/jami-client-qt/+/8cb4e442d95df6d65a26e592844769002c0add1b/src/connectivitymonitor.cpp
*/

#pragma once


#include "pch.h"


class NetworkListManager
{

};
