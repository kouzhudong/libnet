# libnet
windows network library

建立本工程的目的。
这是一个应用层的收集各种网路操作的集合，主要是API的用法，主要是枚举和信息的收集。
详细的有：socket(raw),Spi(lsp),wfp,wlan(wifi),winhttp(proxy),wininet,netblos,IpHelper,html,firewall,Netapi等。

命名管道和邮件槽算不算网络?有的说他们是属于文件。

The Security Support Provider Interface (SSPI) 算不算网络，微软把它列为Authentication (Authentication)。

还有无线和代理以及netsh的接口。


--------------------------------------------------------------------------------------------------


本库说明：  
本库主要包含进程相关的内容，  
如：Sock，WFP，NetBIOS，FireWall，SPI，WinHttp，WinINet等。  
因为本工程是应用层，所以不包含TDI，WSK，WFP，NDIS等的驱动层的内容。  

本库的设计的几个规则：  
1.尽量不调用日志函数。  
  刚开始的时候还考虑是否使用日志，以及何种日志。  
2.因为上一条，所以代码失败要返回详细的信息，也就是失败的原因。  
3.因为上两条的原因，所以使用者要检查返回值，并记录日志。  
4.代码不会主动抛异常。代码尽量屏蔽异常。但是不代表代码中没有异常。  
  代码尽量捕捉异常并返回信息。  
5.导出（对外公开的）的函数都是NTAPI调用格式。  
6.代码尽量使用SAL（source-code annotation language:源代码批注语言）。  
7.代码格式类似go和python.  
8.代码尽量不使用硬编码。  
9.代码开启静态的语法检查（启用Microsoft Code Analysis, 关闭Clang-Tidy）。  
10.警告的等级是4级，且将警告视为错误。  
11.代码的运行要过：驱动程序校验器/应用程序校验器/gflags.exe.  
12.禁止使用断言（估计现在代码中还有不少断言）。  
13.接口的参数只有基本类型和指针（没有类，模板和引用）。  
14.只依赖操作系统的库，不再依赖第三方的库(特指动态库)，当然crt,wil不可免。  
15.所有接口皆为C接口，即EXTERN_C。  
16.C语言标准选择：ISO C17 (2018)标准 (/std:c17)。
17.C++语言标准选择：ISO C++17 标准 (/std:c++17) 或者：预览 - 最新 C++ 工作草案中的功能 (/std:c++latest)。

特别说明：  
本库只提供c/c++/rust调用接口文档，  
其他的，如：ASM，C#，GO，PYTHON，JAVA，PHP等自行定义。  

注意：  
1.接口或者回调的调用方式。__stdcall or __cdecl or __fastcall。  
2.解决方案的默认调试程序是DLL，而不是EXE。  
  且这个是保存在数据库里，而不是配置文件里。  
  所以在调试时，做好设置默认的启动程序为EXE。  
