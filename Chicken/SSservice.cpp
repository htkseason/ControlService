#include "SSservice.h"
char* szServiceName;
SERVICE_STATUS_HANDLE hServiceStatus;
SERVICE_STATUS status;
bool serviceEndFlag = false;
void ServiceInit(char* ServiceName) {
	szServiceName = ServiceName;
	hServiceStatus = NULL;
	status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	status.dwCurrentState = SERVICE_STOPPED;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	status.dwWin32ExitCode = 0;
	status.dwServiceSpecificExitCode = 0;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;
}

bool isInstalled() {
	//打开服务控制管理器
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		//MessageBox(NULL, "Couldn't open service manager", szServiceName, MB_OK);
		return FALSE;
	}
	SC_HANDLE hService = OpenService(hSCM, szServiceName, SERVICE_QUERY_STATUS);
	if (hService) {
		::CloseServiceHandle(hService);
		::CloseServiceHandle(hSCM);
		return TRUE;
	} else {
		::CloseServiceHandle(hService);
		::CloseServiceHandle(hSCM);
		return FALSE;
	}

}

bool InstallService(bool AutoStart) {
	//打开服务控制管理器
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		//MessageBox(NULL, "Couldn't open service manager", szServiceName, MB_OK);
		return FALSE;
	}

	// Get the executable file path
	TCHAR szFilePath[MAX_PATH];
	::GetModuleFileName(NULL, szFilePath, MAX_PATH);

	//创建服务
	SC_HANDLE hService = ::CreateService(
		hSCM, szServiceName, szServiceName,
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START, SERVICE_ERROR_IGNORE,
		szFilePath, NULL, NULL, NULL, NULL, NULL);

	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		//MessageBox(NULL, "Couldn't create service", szServiceName, MB_OK);
		return FALSE;
	}

	if (AutoStart) 
	{
		::StartService(hService, NULL, NULL);
	}



	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);
	return TRUE;
}


bool UninstallService() {
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM == NULL)
	{
		//MessageBox(NULL, "Couldn't open service manager", szServiceName, MB_OK);
		return FALSE;
	}

	SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_STOP | DELETE);

	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		//MessageBox(NULL, "Couldn't open service", szServiceName, MB_OK);
		return FALSE;
	}
	SERVICE_STATUS status;
	::ControlService(hService, SERVICE_CONTROL_STOP, &status);

	//删除服务
	BOOL bDelete = ::DeleteService(hService);
	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);

	if (bDelete)
		return TRUE;

	return FALSE;
}

bool SSservice() {
	SERVICE_TABLE_ENTRY st[] =
	{
		{ szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};
	return !!StartServiceCtrlDispatcher(st);
}

void WINAPI ServiceStrl(DWORD dwOpcode)
{
	switch (dwOpcode)
	{
	case SERVICE_CONTROL_STOP:
		status.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus(hServiceStatus, &status);
		serviceEndFlag = true;
		break;
	case SERVICE_CONTROL_PAUSE:
		break;
	case SERVICE_CONTROL_CONTINUE:
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		break;
	default:;
		//LogEvent("Bad service request");
		break;
	}
}

void WINAPI ServiceMain()
{
	// Register the control request handler
	status.dwCurrentState = SERVICE_START_PENDING;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	//注册服务控制
	hServiceStatus = RegisterServiceCtrlHandler(szServiceName, ServiceStrl);
	if (hServiceStatus == NULL)
	{
		// LogEvent("Handler not installed");
		return;
	}
	SetServiceStatus(hServiceStatus, &status);

	status.dwWin32ExitCode = S_OK;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;
	status.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hServiceStatus, &status);

	//服务运行
	ServiceDo();

	
	serviceEndFlag = false;

	status.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(hServiceStatus, &status);
	// LogEvent("Service stopped");
}