#include "SSsocket.h"
int SSsocket::size_SOCKADDR = sizeof(SOCKADDR);		//only use to pass to the functions


void SSsocket::DataInitialize(void)
{
	//Sockets should use INVALID_SOCKET to initialize
	uSocket = INVALID_SOCKET;
	uSocketTCP = INVALID_SOCKET;

	//tmpParameter used to empty the class private parameters
	SOCKADDR_IN TempEmpty__SOCKADDR_IN = {};
	SockADDR = TempEmpty__SOCKADDR_IN;
	TargetADDR = TempEmpty__SOCKADDR_IN;
	PeerADDR = TempEmpty__SOCKADDR_IN;

	//set state parameter to the initial state
	SSptc = ptcREMAIN;
}
SSsocket::SSsocket(bool Debug)
{
	//if want to output the debug info of the present socket
	debug = Debug;

	//load socket lib 
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);

	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		RaiseErr(SockErr::WSAStartup, NULL);
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		RaiseErr(SockErr::NoWinsock, NULL);
		WSACleanup();
	}

	DataInitialize();
}
SSsocket::~SSsocket(void)
{
	//close sockets is neccessary
	//when it's a TCP client, only needs to close one since the uSocket is a copy of uSocketTCP.
	if (uSocket == uSocketTCP)
		closesocket(uSocket);
	else
	{
		if (uSocketTCP != INVALID_SOCKET)
			closesocket(uSocketTCP);
		if (uSocket != INVALID_SOCKET)
			closesocket(uSocket);
	}
	//data intialize to use again
	DataInitialize();
	//mustn't WSACleanup();
	//	WSACleanup();  
}


bool SSsocket::startup_UDP(void)
{
	//create the socket
	uSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == uSocket)
	{
		RaiseErr(SockErr::CreateSocket_UDP, WSAGetLastError());
		return FALSE;
	}

	//change state parameter
	SSptc = ptcUDP;
	return TRUE;
}
bool SSsocket::startup_TCP(void)
{
	//TCP create uSocketTCP instead of uSocket
	//create the socket  
	uSocketTCP = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == uSocketTCP)
	{
		RaiseErr(SockErr::CreateSocket_TCP, WSAGetLastError());
		return FALSE;
	}

	//change state parameter
	SSptc = ptcTCP;
	return TRUE;
}
int SSsocket::getSendBuffSize(void)
{
	int sendbuffsize;
	int optlen = sizeof(int);
	if (getsockopt(uSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendbuffsize, &optlen) == SOCKET_ERROR)
	{
		RaiseErr(SockErr::SetSockOption, WSAGetLastError());
		return FALSE;
	}
	return sendbuffsize;
}

int SSsocket::getRecvBuffSize(void)
{
	int recvbuffsize;
	int optlen = sizeof(int);
	if (getsockopt(uSocket, SOL_SOCKET, SO_RCVBUF, (char*)&recvbuffsize, &optlen) == SOCKET_ERROR)
	{
		RaiseErr(SockErr::SetSockOption, WSAGetLastError());
		return FALSE;
	}
	return recvbuffsize;
}

bool SSsocket::setSendBuffSize(int SendBuff)
{
	//set SendBuf
	if (setsockopt(uSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&SendBuff, sizeof(int)) == SOCKET_ERROR)
	{
		RaiseErr(SockErr::SetSockOption, WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}
bool SSsocket::setRecvBuffSize(int RecvBuff)
{
	//set RecvBuf
	if (setsockopt(uSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&RecvBuff, sizeof(int)) == SOCKET_ERROR)
	{
		RaiseErr(SockErr::SetSockOption, WSAGetLastError());
		return FALSE;
	}

	return TRUE;
}

bool SSsocket::setSendTimeLimit(int SendTimeLim)
{
	if (setsockopt(uSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&SendTimeLim, sizeof(int)) == SOCKET_ERROR)
	{
		RaiseErr(SockErr::SetSockOption, WSAGetLastError());
		return FALSE;
	}

	return TRUE;
}

bool SSsocket::setRecvTimeLimit(int RecvTimeLim)
{
	if (setsockopt(uSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&RecvTimeLim, sizeof(int)) == SOCKET_ERROR)
	{
		RaiseErr(SockErr::SetSockOption, WSAGetLastError());
		return FALSE;
	}

	return TRUE;
}

int SSsocket::getSendTimeLimit(void)
{
	int sendtimelim = 0;
	int optlen = sizeof(int);
	if (getsockopt(uSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&sendtimelim, &optlen) == SOCKET_ERROR)
	{
		RaiseErr(SockErr::GetSockOption, WSAGetLastError());
		return FALSE;
	}
	return sendtimelim == 0 ? -1 : sendtimelim;
}
int SSsocket::getRecvTimeLimit(void)
{
	int recvtimelim = 0;
	int optlen = sizeof(int);
	if (getsockopt(uSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvtimelim, &optlen) == SOCKET_ERROR)
	{
		RaiseErr(SockErr::GetSockOption, WSAGetLastError());
		return FALSE;
	}
	return recvtimelim == 0 ? -1 : recvtimelim;
}

bool SSsocket::_bind(char* addr, USHORT port)
{
	//set local socket address
	if (addr == 0) { addr = "0.0.0.0"; }
	SockADDR.sin_addr.S_un.S_addr = inet_addr(addr);
	SockADDR.sin_family = AF_INET;
	SockADDR.sin_port = htons(port);

	//if TCP is used. bind uSocketTCP instead of uSocket
	if (uSocketTCP != INVALID_SOCKET)
	{
		if (bind(uSocketTCP, (SOCKADDR*)&SockADDR, sizeof(SOCKADDR)) == SOCKET_ERROR)
		{
			RaiseErr(SockErr::Bind, WSAGetLastError());
			return FALSE;
		}
		return TRUE;
	}

	//TCP is not used. bind uSocket.
	if (bind(uSocket, (SOCKADDR*)&SockADDR, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		RaiseErr(SockErr::Bind, WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}

bool SSsocket::_bind(void)
{
	//bind 0.0.0.0 and a random port
	return _bind("0.0.0.0", NULL);
}

bool SSsocket::_listen(void)
{
	//only TCP uses listen()
	//if don't call bind() first. #ERROR# may occurs
	/*listen() set the uSocketTCP to the listen state
	  connection uses the accept() return socket which I store it in uSocket*/

	//to find if socket is bind() or not. if it's not, bind the default addr&port
	if (!getSocketPort()) { _bind(NULL, NULL); }

	if (listen(uSocketTCP, SS_TCP_LISTEN_BACKLOG) == SOCKET_ERROR)
	{
		RaiseErr(SockErr::Listen, WSAGetLastError());
		return FALSE;
	}

	return TRUE;
}
bool SSsocket::_listen(char* addr, USHORT port)
{
	//combination of bind(addr,port) and listen().
	if ((_bind(addr, port)) && (_listen()))
		return TRUE;
	else
		return FALSE;
}

bool SSsocket::_accept(void)
{
	//accept() blocks the thread until someone connect() you.
	//if it is a bad connection. while(1) makes it to wait to accept() the next connection.
	/* accept() will give a new Socket(which will be used in the contact instead of uSocketTCP)
		I store it in uSocket as UDP.
		makes it easy for send() */
	while (1)
	{
		uSocket = accept(uSocketTCP, (SOCKADDR*)&PeerADDR, (int*)&size_SOCKADDR);
		if (uSocketTCP != INVALID_SOCKET) { break; }
		if (uSocket == SOCKET_ERROR)
		{
			RaiseErr(SockErr::Accept, WSAGetLastError());
			//continue;
			return FALSE;
		}
	}

	//set TargetADDR equals PeerADDR(which is exactly the TCP partner's ADDR)
	//it makes send() more clearly
	TargetADDR = PeerADDR;

	return TRUE;
}

bool SSsocket::_connect(void)
{
	//you should set TargetADDR before connect()
	if (connect(uSocketTCP, (SOCKADDR*)&TargetADDR, size_SOCKADDR) == SOCKET_ERROR)
	{
		RaiseErr(SockErr::Connect, WSAGetLastError());
		return FALSE;
	}

	//makes it easy to send()
	uSocket = uSocketTCP;
	//set PeerADDR
	PeerADDR = TargetADDR;

	return TRUE;
}
bool SSsocket::_connect(char* addr, USHORT port)
{
	//combination of SetTarget() and connect()
	setTarget(addr, port);
	return _connect();
}


int SSsocket::_send(const char* content, int len)
{
	int BytesSend = 0;
	//if the protocol is UDP call sendto(TargetADDR)
	if (SSptc == ptcUDP)
	{
		BytesSend = sendto(uSocket, content, len, NULL, (SOCKADDR*)&TargetADDR, sizeof(SOCKADDR));
		if (BytesSend == SOCKET_ERROR)
		{
			RaiseErr(SockErr::Send_UDP, WSAGetLastError());
			return FALSE;
		}
		else
		{
			return BytesSend;
		}
	}

	//if the protocal is TCP call send(*uSocket*)
	if (SSptc == ptcTCP)
	{
		BytesSend = send(uSocket, content, len, NULL);
		if (BytesSend == SOCKET_ERROR)
		{
			RaiseErr(SockErr::Send_TCP, WSAGetLastError());
			return FALSE;
		}
		else
		{
			return BytesSend;
		}
	}

	return FALSE;
}

int SSsocket::_send(const char* content, int len, char* addr, USHORT port)
{
	//combination of SetTarget() and send()
	setTarget(addr, port);
	return _send(content, len);
}

int SSsocket::_recv(char* recvBuf, int len)
{
	//if protocol is UDP call recvfrom()
	//recvfrom() gets the Peer's address info and stores it in PeerADDR.
	int BytesRecved;
	if (SSptc == ptcUDP)
	{
		BytesRecved = recvfrom(uSocket, recvBuf, len, NULL, (SOCKADDR*)&PeerADDR, (int*)&size_SOCKADDR);
		if (BytesRecved == SOCKET_ERROR)
		{
			int errcode = WSAGetLastError();
			RaiseErr(SockErr::Recv_UDP, errcode);
			if (errcode == WSAETIMEDOUT)
				return SS_RECV_TIMEOUT;
			else
				return FALSE;
		}
		else
		{
			return BytesRecved;
		}
	}

	//if protocol is TCP call recv()
	//the peer of TCP 's info is always stores in PeerADDR and TargetADDR
	if (SSptc == ptcTCP)
	{
		BytesRecved = recv(uSocket, recvBuf, len, NULL);
		if (BytesRecved == SOCKET_ERROR)
		{
			int errcode = WSAGetLastError();
			RaiseErr(SockErr::Recv_TCP, errcode);
			if (errcode == WSAETIMEDOUT)
				return SS_RECV_TIMEOUT;
			else
				return FALSE;
		}
		else
		{
			return BytesRecved;
		}
	}

	return FALSE;
}

int SSsocket::_recv(char* recvBuf, int len, int tmpTimeLimit)
{
	//the given tmptimelim can only state this time's recv timeout limit
	int rcvtimelim = getRecvTimeLimit();
	if (rcvtimelim == -1)
		rcvtimelim = 0;
	setRecvTimeLimit(tmpTimeLimit);
	int result = _recv(recvBuf, len);
	setRecvTimeLimit(rcvtimelim);

	return result;
}

bool SSsocket::setTarget(char* addr, USHORT port)
{
	//used to set TargetADDR
	TargetADDR.sin_addr.S_un.S_addr = inet_addr(addr);
	TargetADDR.sin_family = AF_INET;
	TargetADDR.sin_port = htons(port);
	return TRUE;
}


USHORT SSsocket::getSocketPort(void)
{
	//function used to get the present socket's present port
	if (uSocket != INVALID_SOCKET)
	{
		if (getsockname(uSocket, (SOCKADDR*)&SockADDR, (int*)&size_SOCKADDR) == SOCKET_ERROR)
		{
			RaiseErr(SockErr::GetSockName, WSAGetLastError());
			return FALSE;
		}
		else
		{
			return ntohs(SockADDR.sin_port);
		}
	}

	if (uSocketTCP != INVALID_SOCKET)
	{
		if (getsockname(uSocketTCP, (SOCKADDR*)&SockADDR, (int*)&size_SOCKADDR) == SOCKET_ERROR)
		{
			RaiseErr(SockErr::GetSockName, WSAGetLastError());
			return FALSE;
		}
		else
		{
			return ntohs(SockADDR.sin_port);
		}
	}

	return FALSE;
}
char* SSsocket::getSocketAddr(void)
{
	//function used to get the present socket's present address
	if (uSocket != INVALID_SOCKET)
	{
		if (getsockname(uSocket, (SOCKADDR*)&SockADDR, (int*)&size_SOCKADDR) == SOCKET_ERROR)
		{
			RaiseErr(SockErr::GetSockName, WSAGetLastError());
			return FALSE;
		}
		else
		{
			return inet_ntoa(SockADDR.sin_addr);
		}
	}

	if (uSocketTCP != INVALID_SOCKET)
	{
		if (getsockname(uSocketTCP, (SOCKADDR*)&SockADDR, (int*)&size_SOCKADDR) == SOCKET_ERROR)
		{
			RaiseErr(SockErr::GetSockName, WSAGetLastError());
			return FALSE;
		}
		else
		{
			return inet_ntoa(SockADDR.sin_addr);
		}
	}

	return FALSE;
}

USHORT SSsocket::getPeerPort(void)
{
	/*
	if (getpeername(uSocket,(SOCKADDR*)&PeerADDR,(int*)&size_SOCKADDR) == SOCKET_ERROR)
	{
	return FALSE;
	}
	else
	{
	return ntohs(PeerADDR.sin_port);
	}
	*/
	return ntohs(PeerADDR.sin_port);
}
char* SSsocket::getPeerAddr(void)
{
	/*
	if (getpeername(uSocket,(SOCKADDR*)&PeerADDR,(int*)&size_SOCKADDR) == SOCKET_ERROR)
	{
	return FALSE;
	}
	else
	{
	return inet_ntoa(PeerADDR.sin_addr);
	}
	*/
	return inet_ntoa(PeerADDR.sin_addr);
}


int SSsocket::RaiseErr(int sockerr, int errcode)
{
	if (debug)
	{
		switch (sockerr)
		{
		case SockErr::WSAStartup:
			printf_s("WSAStartup failed!\n"); break;
		case SockErr::NoWinsock:
			printf_s("Can't find useable Winsock Dll!\n"); break;
		case SockErr::CreateSocket_UDP:
			printf_s("Can't create a UDP socket!\n\t err : %d\n", errcode); break;
		case SockErr::CreateSocket_TCP:
			printf_s("Can't create a TCP socket!\n\t err : %d\n", errcode); break;

		case SockErr::Bind:
			printf_s("Bind() called err!\n\t err : %d\n", errcode); break;
		case SockErr::Listen:
			printf_s("Listen() called err!\n\t err : %d\n", errcode); break;
		case SockErr::Connect:
			printf_s("Connect() called err!\n\t err : %d\n", errcode); break;
		case SockErr::Accept:
			printf_s("Accept() called err!\n\t err : %d\n", errcode); break;


		case SockErr::Recv_UDP:
			if (errcode == WSAETIMEDOUT)
				printf_s("UDP Recv() time out!\n");
			else
				printf_s("UDP Recv() called err!\n\t err : %d\n", errcode);
			break;
		case SockErr::Recv_TCP:
			if (errcode == WSAETIMEDOUT)
				printf_s("TCP Recv() time out!\n");
			else
				printf_s("TCP Recv() called err!\n\t err : %d\n", errcode);
			break;
		case SockErr::Send_UDP:
			if (errcode == WSAETIMEDOUT)
				printf_s("UDP Send() time out!\n");
			else
				printf_s("UDP Send() called err!\n\t err : %d\n", errcode);
			break;
		case SockErr::Send_TCP:
			if (errcode == WSAETIMEDOUT)
				printf_s("TCP Send() time out!\n");
			else
				printf_s("TCP Send() called err!\n\t err : %d\n", errcode);
			break;

		case SockErr::SetSockOption:
		case SockErr::GetSockOption:
		case SockErr::GetSockName:
			printf_s("Get/Set Sock err!\n\t err : %d\n", errcode); break;

		default:
			printf_s("UNKNOWN SOCKET ERR!\n");
		}
	}
	return NULL;
}