#pragma once
/*
=========================================================
class SSsocket;
Written by Season; 2014.10;
it is an integration of WinSock;
it makes code more easy to read
=========================================================
*/
#ifndef __H__SS__SOCKET__
#define __H__SS__SOCKET__

#include <stdio.h>  
#include <Windows.h>
#pragma comment(lib, "WS2_32.lib")
#define SS_TCP_LISTEN_BACKLOG	5
#define SS_RECV_TIMEOUT			-1
#define SS_NO_TIME_LIMIT		-1


class SSsocket
{
public:
	SSsocket(bool Debug=false);
	~SSsocket(void);

	bool startup_UDP(void);
	bool startup_TCP(void);
	
	bool setSendBuffSize(int SendBuff);
	bool setRecvBuffSize(int SendBuff);
	bool setSendTimeLimit(int SendTimeLim);
	bool setRecvTimeLimit(int RecvTimeLim);
	bool setTarget(char* addr, USHORT port);

	int  getSendBuffSize(void);
	int  getRecvBuffSize(void);
	int  getSendTimeLimit(void);	//-1 no limit. 0 is call failure.
	int  getRecvTimeLimit(void);
	char* getSocketAddr(void);	//get locol socket info. usually called after bind a random addr
	USHORT getSocketPort(void);
	char* getPeerAddr(void);
	USHORT getPeerPort(void);
	
	bool _listen(void);			//reload version. bind a random port and change to listen state
	bool _listen(char* addr, USHORT port);
	bool _accept(void);
	bool _connect(void);		//reload version. connect to the target which should be set previously
	bool _connect(char* addr, USHORT port);

	bool _bind(void);		//reload version. bind at a random port & "0.0.0.0"
	bool _bind(char* addr, USHORT port);

	/* =========Send & Recv================
	   the (int len) parameter of the following three processes
	   should be the length of the content/recvBuf
	   which can prevent memory overflow  
	//============================
		if there is a timelimit for send or recv
		when send time out, return 0. states an unexpected err.
		but when recv time out, return -1. states an expected err.
	=========Send & Recv====================*/
	int _send(const char* content, int len);	//reload version. send to the target which should be set previously
	int _send(const char* content, int len, char* addr, USHORT port);
	int _recv(char* recvBuf, int len);
	int _recv(char* recvBuf, int len, int tmptimelim);	//tmptimelim only takes effect this time


	

	
	class SockErr {
	public:
		static const int WSAStartup			= 1;
		static const int NoWinsock			= 2;
		static const int CreateSocket_UDP	= 3;
		static const int CreateSocket_TCP	= 4;
		static const int Bind				= 5;
		static const int Listen				= 6;
		static const int Accept				= 7;
		static const int Connect			= 8;

		static const int Send_UDP			= 9;
		static const int Send_TCP			= 10;
		static const int Recv_UDP			= 11;
		static const int Recv_TCP			= 12;

		static const int SetSockOption		= 13;
		static const int GetSockOption		= 14;
		static const int GetSockName		= 15;
	};

private:
	static int size_SOCKADDR;		//only use to pass to the functions

	//present protocol. used to decide send/sendto and recv/recvfrom
	enum {ptcREMAIN, ptcUDP, ptcTCP} SSptc;

	bool debug;

	SOCKET uSocket;			//main socket
	SOCKET uSocketTCP;		//extra socket(TCP use)

	SOCKADDR_IN SockADDR;	//local socket info
	SOCKADDR_IN TargetADDR;	//send target socket info
	SOCKADDR_IN PeerADDR;	//remote socket info

	void DataInitialize(void);

	int RaiseErr(int sockerr, int errcode);

};

#endif