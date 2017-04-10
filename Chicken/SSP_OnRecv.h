#ifndef __H__SSP__ONRECV__
#define __H__SSP__ONRECV__
#include "SSservice.h"
#include "DeviceControl.h"
#include "SSprotocol.h"
#include "SScmdPipe.h"
#include <io.h>



#define CMD_BEAT				0xFFFFFFFF
#define CMD_FILE				1
#define CMD_SYSTEM				2
#define CMD_DEVICE				3
#define CMD_OPENCMD				4
#define CMD_GETFILE				5
#define CMD_GETSCREEN			6

#define LeftLen		(len - (int)(msgPos - msg))

void Recv_Cmd_Device(SSprotocol& sock, char* msg, int len);
void Recv_Cmd_File(SSprotocol& sock, char* msg, int len);
void Recv_Cmd_System(SSprotocol& sock, char* msg, int len);
void Recv_Cmd_GetFile(SSprotocol& sock, char* msg, int len);
void Recv_Cmd_GetScreen(SSprotocol& sock);

#endif