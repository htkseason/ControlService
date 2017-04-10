#include "SScmdPipe.h"
SSprotocol* SScmdPipe::sock = NULL;
HANDLE SScmdPipe::hReadPipe = NULL;
HANDLE SScmdPipe::hWritePipe = NULL;
HANDLE SScmdPipe::hWriteFile = NULL;
HANDLE SScmdPipe::hReadFile = NULL;
char SScmdPipe::send_buff[PIPE_BUFF_SIZE] = {};
HANDLE SScmdPipe::hReadThread = NULL;