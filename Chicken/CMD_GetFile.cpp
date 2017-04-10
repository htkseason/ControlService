#include "SSP_OnRecv.h"

#define FILE_PACK_SIZE (64*1024)

char* makeFileMessage(FILE* file, char* fileName, int offset, int counts, int& outlen);
void Recv_Cmd_GetFile(SSprotocol& sock, char* msg, int len) {
	char* msgPos = msg;
	char response[255];


	char filePath[255 + 1] = {};

	int expectedLen = sizeof(filePath) - 1;
	if (LeftLen != expectedLen)
	{
		sprintf_s(response, sizeof(response), "(GetFile)Broken Msg. (%d/%d)\n", LeftLen, expectedLen);
		sock._feedBack(response);
		return;
	}

	memcpy(filePath, msgPos, sizeof(filePath) - 1);
	msgPos += sizeof(filePath) - 1;


	FILE* file = NULL;
	fopen_s(&file, filePath, "rb");
	if (file == NULL)
	{
		sprintf_s(response, sizeof(response), "(GetFile)Cannnot Open File. (%s)\n", filePath);
		sock._feedBack(response);
		return;
	}


	int fileSize = _filelength(_fileno(file));
	int filePos = 0;
	char* fileName = strrchr(filePath, '\\');
	if (fileName == NULL)
		fileName = filePath;
	else
		fileName++;

	sprintf_s(response, sizeof(response), "(GetFile)Trans File Start. (%s)\n", filePath);
	sock._feedBack(response);
	while (filePos < fileSize) {
		char* msgtosend;
		int msglen = 0;
		if ((fileSize - filePos) > FILE_PACK_SIZE)
		{
			msgtosend = makeFileMessage(file, fileName, filePos, FILE_PACK_SIZE, msglen);
			filePos += FILE_PACK_SIZE;
		}
		else
		{
			msgtosend = makeFileMessage(file, fileName, filePos, fileSize - filePos, msglen);
			filePos = fileSize;
		}
		sock._send(msgtosend, msglen);
		delete[] msgtosend;
	}
	sprintf_s(response, sizeof(response), "(GetFile)Trans File End. (%s)\n", filePath);
	sock._feedBack(response);

	fclose(file);
}

char* makeFileMessage(FILE* file, char* fileName, int offset, int counts, int& outlen)
{
	//		cmd	filepath	offset		counts		data
	outlen = 20 + 255 + sizeof(int) + sizeof(int) + counts;
	char* result = new char[outlen];
	char* tp = result;

	memset(result, 0, outlen);

	char* cmd = "File";
	memcpy(tp, cmd, strlen(cmd));
	tp += 20;

	memcpy(tp, fileName, (int)strlen(fileName));
	tp += 255;

	memcpy(tp, &offset, sizeof(int));
	tp += sizeof(int);

	memcpy(tp, &counts, sizeof(int));
	tp += sizeof(int);

	fseek(file, offset, SEEK_SET);
	fread_s(tp, counts, counts, 1, file);
	tp += counts;

	return result;

}