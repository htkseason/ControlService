#include "SSP_OnRecv.h"

void Recv_Cmd_File(SSprotocol& sock, char* msg, int len) {
	char* msgPos = msg;
	char response[255];

	char filePath[255 + 1] = {};
	int fileOffset = 0;
	int dataSize = 0;
	int headLen = sizeof(filePath) - 1
		+ sizeof(fileOffset) + sizeof(dataSize);

	if (LeftLen < headLen)
	{
		sprintf_s(response, sizeof(response), "(File)Broken Head. (%d/%d)\n", LeftLen, headLen);
		sock._feedBack(response);
		return;
	}

	memcpy(filePath, msgPos, sizeof(filePath) - 1);
	msgPos += sizeof(filePath) - 1;

	memcpy(&fileOffset, msgPos, sizeof(int));
	msgPos += sizeof(int);

	memcpy(&dataSize, msgPos, sizeof(int));
	msgPos += sizeof(dataSize);


	if (LeftLen != dataSize)
	{
		sprintf_s(response, sizeof(response), "(File)Broken Data. (%d/%d)\n", LeftLen, dataSize);
		sock._feedBack(response);
		return;
	}

	FILE* file = NULL;
	if (fileOffset == 0)
		fopen_s(&file, filePath, "wb");
	else
		fopen_s(&file, filePath, "ab");

	if (file == NULL) {
		sprintf_s(response, sizeof(response), "(File)Cannot Open File. (%s)\n", filePath);
		sock._feedBack(response);
		return;
	}

	int fileSize = _filelength(_fileno(file));
	if (fileSize != fileOffset)
	{
		sprintf_s(response, sizeof(response), "(File)Unmatched File Pack. (%d <-> %d)\n", fileSize, fileOffset);
		sock._feedBack(response);
		return;
	}



	fwrite(msgPos, dataSize, 1, file);
	fclose(file);

	sprintf_s(response, sizeof(response), "(File)Success. (%d -> %d)\n", fileOffset, fileOffset + dataSize);
	sock._feedBack(response);
	return;
}
