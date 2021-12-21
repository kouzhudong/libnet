/*
显示一个网页的思路：
1.API：MSHTML!ShowHTMLDialog。IE内核（Trident）。
2.COM:IHostDialogHelper->ShowHTMLDialog。IE内核（Trident）。
3.WebView，包括Cpp和c#的实现。
4.Webkit，Chromium Embedded Framework (CEF)。
5.Gecko内核。
6.微软的那个夭折的且开源的适用于edge的内核引擎（ChakraCore）。
7.MFC/QT等其他的封装。
*/


#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////


extern SHOWHTMLDIALOGFN * pfnShowHTMLDialog;


//////////////////////////////////////////////////////////////////////////////////////////////////
