/*
此文主要演示：
WSCEnumProtocols
WSCEnumProtocols32
WSCEnumNameSpaceProviders
WSCEnumNameSpaceProviders32
WSCEnumNameSpaceProvidersEx
WSCEnumNameSpaceProvidersEx32

WSAEnumNetworkEvents
WSAEnumProtocols
WSAEnumNameSpaceProviders
WSAEnumNameSpaceProvidersEx
等函数的用法。

一些名字解释：
SPI：Service Provider Interface
WOSA：Windows Open System Architecture

WSA：Windows Sockets API, 简短记为Winsock
WSP：Winsock Service Provider
WPU：Winsock Provider Upcall
WSC：WinSock Configuration
NSP：NameSpace Provider
LSP：Layered Service Provider
*/


#pragma once


int WSAEnumNameSpaceProviders();
int WSAEnumNetworkEvents();
int WSAEnumProtocols();

int WSCEnumProtocols();
int WSCEnumProtocols32();
