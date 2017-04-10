#ifndef __H__SS__SERVICE__
#define __H__SS__SERVICE__

#include <Windows.h>
#include <stdio.h>

void ServiceInit(char* ServiceName);
bool InstallService(bool AutoStart = false);
bool UninstallService();
bool SSservice();
bool isInstalled();
void WINAPI ServiceMain();
void WINAPI ServiceStrl(DWORD dwOpcode);

void CALLBACK ServiceDo();


#endif