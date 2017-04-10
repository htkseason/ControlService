#include "SSP_OnRecv.h"

void Recv_Cmd_Device(SSprotocol& sock, char* msg, int len) {
	char* msgPos = msg;
	char response[255];

	bool state = true;
	int autoRecover = 0;
	char deviceGUID[64 + 1] = {};

	int expectedLen = sizeof(state) + sizeof(autoRecover) + sizeof(deviceGUID) - 1;
	if (LeftLen != expectedLen)
	{
		sprintf_s(response, sizeof(response), "(Device)Broken Msg. (%d/%d)\n", LeftLen, expectedLen);
		sock._feedBack(response);
		return;
	}
	memcpy(&state, msgPos, sizeof(state));
	msgPos += sizeof(state);

	memcpy(&autoRecover, msgPos, sizeof(autoRecover));
	msgPos += sizeof(autoRecover);

	memcpy(deviceGUID, msgPos, sizeof(deviceGUID));
	msgPos += sizeof(deviceGUID) - 1;

	if (state == false && autoRecover)
	{
		DeviceControl::setDevice(false, deviceGUID);
		Sleep(autoRecover);
		DeviceControl::setDevice(true, deviceGUID);
		sprintf_s(response, sizeof(response), "(Device)Success. state=auto delay=%d GUID=%s\n", autoRecover, deviceGUID);
		sock._feedBack(response);
		return;
	}

	if (DeviceControl::setDevice(state, deviceGUID))
	{
		sprintf_s(response, sizeof(response), "(Device)Success. state=%d GUID=%s\n", state, deviceGUID);
		sock._feedBack(response);
		return;
	}
	else
	{
		sprintf_s(response, sizeof(response), "(Device)Set Device Failed. GUID=%s\n", deviceGUID);
		sock._feedBack(response);
		return;
	}
}
