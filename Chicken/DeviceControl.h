#pragma once
#ifndef __H__HARD__CONTROL__
#define __H__HARD__CONTROL__
#include <stdio.h>
#include <windows.h>
#include <setupapi.h>
#pragma comment( lib,"ws2_32.lib" )
#pragma comment( lib,"setupapi.lib" )
#define CLASS_GUID_BUFFER_SIZE 255
#define CLASS_DESC_BUFFER_SIZE 255

#define GUID DeviceControl::DeviceInfo
class DeviceControl
{
public:
	class DeviceInfo
	{
	public :
		static char NetCardClassGUID[];
		static char KeyBoardClassGUID[];
		static char MouseClassGUID[];
		static char GraphicsClassGUID[];
		static char AudioClassGUID[];
		static char InputClassGUID[];
	};
	static bool printDevice(char* TargetClassGUID);
	static bool setDevice(bool Enable, char* TargetClassGUID);
	static bool setDevice(bool Enable, int id);
};


#endif