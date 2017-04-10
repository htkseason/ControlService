#include "SSP_OnRecv.h"

void Recv_Cmd_System(SSprotocol& sock, char* msg, int len) {
	char* msgPos = msg;
	char response[255];

	char cmd[255 + 1] = {};

	int expectedLen = sizeof(cmd) - 1;
	if (LeftLen != expectedLen)
	{
		sprintf_s(response, sizeof(response), "(System)Broken Msg. (%d/%d)\n", LeftLen, expectedLen);
		sock._feedBack(response);
		return;
	}

	memcpy(cmd, msgPos, sizeof(cmd) - 1);
	msgPos += sizeof(cmd) - 1;

	int result = SScmdPipe::sendMsg(cmd);

	sprintf_s(response, sizeof(response), "(System)Success. result=%d cmd=\"%s\"\n", result, cmd);
	sock._feedBack(response);
	return;
}
