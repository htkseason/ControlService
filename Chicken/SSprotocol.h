#pragma once
#ifndef __H__SS__PROTOCOL__
#define __H__SS__PROTOCOL__
#include "SSsocket.h"


#define RECV_BUFF_SIZE (128*1024)
#define SSP_RECV_ERROR 0
#define SSP_RECV_TIMEO -1
#define SSP_RECV_BROKEN_MSG -2
#define SSP_RECV_WRONG_MSG -3

class SSprotocol : public SSsocket{

private:
	static char SSP_HEAD[3];
	static char SSP_TAIL[3];

	char* packMsg(const char* msg, int len, int& outlen);

	int findValidMsg(char* result);

	char* recvBuff = NULL;
	int recvBuffOffset = 0;

public:
	SSprotocol()
	{
		recvBuff = new char[RECV_BUFF_SIZE];
		recvBuffOffset = 0;
	}
	~SSprotocol()
	{
		delete[] recvBuff;
	}


	int _send(const char* content, int len)
	{
		return _send(content, len, NULL, NULL);
	}

	int _send(const char* content, int len, char* addr, USHORT port)
	{
		int outlen;
		char* packed = packMsg(content, len, outlen);
		int result;
		if (addr == NULL || port == NULL)
			result = SSsocket::_send(packed, outlen);
		else
			result = SSsocket::_send(packed, outlen, addr, port);
		delete[] packed;
		return result;
	}

	int _feedBack(char* content) {
		int len = (int)strlen(content);
		char* str = new char[len + 20];
		sprintf_s(str, len + 20, "Result %s", content);
		int result = _send(str, (int)strlen(str));
		delete[] str;
		return result;
	}

	int _recv(char* recvbuff)
	{
		return _recv(recvbuff, 0);
	}

	int _recv(char* recvbuff, int timeLim)
	{
		//find left msg
		int result = findValidMsg(recvbuff);
		if (result > 0)
			return result;

		//no msg left, recv
		int bytesRecved = SSsocket::_recv(&recvBuff[recvBuffOffset], RECV_BUFF_SIZE - recvBuffOffset, timeLim);
		if (bytesRecved == SS_RECV_TIMEOUT)
			return SSP_RECV_TIMEO;
		if (bytesRecved == 0)
			return SSP_RECV_ERROR;
		recvBuffOffset += bytesRecved;

		//and find msg
		return findValidMsg(recvbuff);
	}

};


#endif