/*
������Ҫ��ʾ��
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
�Ⱥ������÷���

һЩ���ֽ��ͣ�
SPI��Service Provider Interface
WOSA��Windows Open System Architecture

WSA��Windows Sockets API, ��̼�ΪWinsock
WSP��Winsock Service Provider
WPU��Winsock Provider Upcall
WSC��WinSock Configuration
NSP��NameSpace Provider
LSP��Layered Service Provider
*/


#pragma once


int WSAEnumNameSpaceProviders();
int WSAEnumNetworkEvents();
int WSAEnumProtocols();

int WSCEnumProtocols();
int WSCEnumProtocols32();
