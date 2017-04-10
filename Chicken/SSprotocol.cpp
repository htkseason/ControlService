#include "SSprotocol.h"

char SSprotocol::SSP_HEAD[3] = { (byte)0xCC, (byte)0xBB, (byte)0xAA };
char SSprotocol::SSP_TAIL[3] = { (byte)0xAA, (byte)0xBB, (byte)0xCC };

char* SSprotocol::packMsg(const char* msg, int len, int& outlen)
{
	char* result = new char[sizeof(SSP_HEAD) + sizeof(int) + len + sizeof(SSP_TAIL)];
	int p = 0;
	memcpy(&result[p], SSP_HEAD, sizeof(SSP_HEAD));
	p += sizeof(SSP_HEAD);
	memcpy(&result[p], &len, sizeof(int));
	p += sizeof(int);
	memcpy(&result[p], msg, len);
	p += len;
	memcpy(&result[p], SSP_TAIL, sizeof(SSP_TAIL));
	p += sizeof(SSP_TAIL);

	outlen = p;
	return result;
}
int SSprotocol::findValidMsg(char* result)
{

	//check head
	if (recvBuffOffset < sizeof(SSP_HEAD))
		return SSP_RECV_BROKEN_MSG;
	for (int i = 0; i < sizeof(SSP_HEAD); i++)
		if (recvBuff[i] != SSP_HEAD[i])
		{
			recvBuffOffset = 0;
			return SSP_RECV_WRONG_MSG;
		}

	//check size
	if (recvBuffOffset < sizeof(SSP_HEAD) + sizeof(int))
		return SSP_RECV_BROKEN_MSG;
	int msgSize = 0;
	memcpy(&msgSize, &recvBuff[sizeof(SSP_HEAD)], sizeof(int));
	if (msgSize < 0)
	{
		recvBuffOffset = 0;
		return SSP_RECV_WRONG_MSG;
	}


	//check tail
	int expectedLen = sizeof(SSP_HEAD) + sizeof(int) + msgSize + sizeof(SSP_TAIL);
	if (expectedLen > recvBuffOffset)
		return SSP_RECV_BROKEN_MSG;
	for (int i = 0; i < sizeof(SSP_TAIL); i++)
		if (recvBuff[expectedLen - sizeof(SSP_TAIL) + i] != SSP_TAIL[i])
		{
			recvBuffOffset = 0;
			return SSP_RECV_WRONG_MSG;
		}

	//copy checked msg
	if (msgSize > 0)
	{
		result[msgSize] = '\0';  //prevent overflow
		memcpy(result, &recvBuff[sizeof(SSP_HEAD) + sizeof(int)], msgSize);
	}
	else
		result = NULL;


	//format buffer if neccssary
	if (recvBuffOffset > expectedLen)
	{
		memcpy(recvBuff, &recvBuff[expectedLen], recvBuffOffset - expectedLen);
		recvBuffOffset -= expectedLen;
	}
	else
		recvBuffOffset = 0;


	return msgSize;

}