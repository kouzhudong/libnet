#pragma once

class NetApi
{
};


void UserEnum();
int UserEnum(int argc, wchar_t * argv[]);

void EnumLocalGroup();

void EnumShare();
void EnumShare(int argc, TCHAR * lpszArgv[]);

void EnumWkstaUser();
int EnumWkstaUser(int argc, wchar_t * argv[]);

int EnumSession(int argc, wchar_t * argv[]);

void EnumConnection(int argc, wchar_t * argv[]);

int EnumServer(int argc, wchar_t * argv[]);

int EnumServerDisk(int argc, wchar_t * argv[]);

BOOL WINAPI EnumResource(LPNETRESOURCE lpnr);
