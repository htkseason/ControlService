#include "DeviceControl.h"
char GUID::NetCardClassGUID[] = "{4d36e972-e325-11ce-bfc1-08002be10318}";
char GUID::KeyBoardClassGUID[] = "{4d36e96b-e325-11ce-bfc1-08002be10318}";
char GUID::MouseClassGUID[] = "{4d36e96f-e325-11ce-bfc1-08002be10318}";
char GUID::GraphicsClassGUID[] = "{4d36e968-e325-11ce-bfc1-08002be10318}";
char GUID::AudioClassGUID[] = "{c166523c-fe0c-4a94-a586-f1a80cfbbf3e}";
char GUID::InputClassGUID[] = "{745a17a0-74d3-11d0-b6fe-00a0c90f57da}";



bool DeviceControl::printDevice(char* TargetClassGUID)
{

	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		printf_s("Get Hard Handle Err\n");
		return FALSE;
	}


	SP_DEVINFO_DATA spDevInfoData;
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData); i++)
	{
		char buffer[CLASS_GUID_BUFFER_SIZE] = {};

		if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_CLASSGUID, NULL,
			(PBYTE)buffer, CLASS_GUID_BUFFER_SIZE, NULL))
		{
			if (GetLastError() == ERROR_INVALID_DATA)
			{
				continue;
			}
			else
			{
				printf_s("Get Hard Err\n");
				SetupDiDestroyDeviceInfoList(hDevInfo);
				return FALSE;
			}
		}

		if (_stricmp(buffer, TargetClassGUID) == 0)
		{
			char buffer[CLASS_DESC_BUFFER_SIZE] = {};
			SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_DEVICEDESC, NULL,
				(PBYTE)buffer, CLASS_DESC_BUFFER_SIZE, NULL);
			printf_s("%3d %s\n", i, buffer);
			continue;
		}

	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return TRUE;

}
bool DeviceControl::setDevice(bool Enable, int id)
{
	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		printf_s("Get Hard Handle Err\n");
		return FALSE;
	}


	SP_DEVINFO_DATA spDevInfoData;
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	if (!SetupDiEnumDeviceInfo(hDevInfo, id, &spDevInfoData))
	{
		printf_s("Wrong Device Id\n");
		return FALSE;
	}

	if (Enable)
		printf_s("Enable  %d\n", id);
	else
		printf_s("Disable %d\n", id);

	SP_PROPCHANGE_PARAMS spPropChangeParams;
	spPropChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	spPropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	spPropChangeParams.Scope = DICS_FLAG_GLOBAL;
	spPropChangeParams.StateChange = Enable ? DICS_ENABLE : DICS_DISABLE;

	if (!SetupDiSetClassInstallParams(hDevInfo, &spDevInfoData, (SP_CLASSINSTALL_HEADER*)&spPropChangeParams, sizeof(spPropChangeParams)))
	{
		printf_s("Change Hard State Err_1 0x%x\n", GetLastError());
		return FALSE;
	}

	if (!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &spDevInfoData))
	{
		printf_s("Change Hard State Err_2 0x%x\n", GetLastError());
		return FALSE;
	}


	SetupDiDestroyDeviceInfoList(hDevInfo);

	return TRUE;

}


bool DeviceControl::setDevice(bool Enable, char* TargetClassGUID)
{
	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		printf_s("Get Hard Handle Err\n");
		return FALSE;
	}


	SP_DEVINFO_DATA spDevInfoData;
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData); i++)
	{
		char buffer[CLASS_GUID_BUFFER_SIZE];

		if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_CLASSGUID, NULL,
			(PBYTE)buffer, CLASS_GUID_BUFFER_SIZE, NULL))
		{
			if (GetLastError() == ERROR_INVALID_DATA)
			{
				continue;
			}
			else
			{
				printf_s("Get Hard Err\n");
				SetupDiDestroyDeviceInfoList(hDevInfo);
				return FALSE;
			}
		}

		if (_stricmp(buffer, TargetClassGUID) == 0)
		{
			if (Enable)
				printf_s("Enable  %d\n", i);
			else
				printf_s("Disable %d\n", i);

			SP_PROPCHANGE_PARAMS spPropChangeParams;
			spPropChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
			spPropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
			spPropChangeParams.Scope = DICS_FLAG_GLOBAL;
			spPropChangeParams.StateChange = Enable ? DICS_ENABLE : DICS_DISABLE;

			if (!SetupDiSetClassInstallParams(hDevInfo, &spDevInfoData, (SP_CLASSINSTALL_HEADER*)&spPropChangeParams, sizeof(spPropChangeParams)))
			{
				printf_s("Change Hard State Err_1 0x%x\n", GetLastError());
			}

			if (!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &spDevInfoData))
			{
				printf_s("Change Hard State Err_2 0x%x\n", GetLastError());
			}

			continue;
		}

	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return TRUE;

}