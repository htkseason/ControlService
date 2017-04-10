#include "SSservice.h"
#include <string.h>
#include <io.h>

//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

#define WORK_FILE "C:\\Program Files\\Windows NT\\CtrlService.exe"
#define WORK_FILE_INSTALL "start \"\" \"C:\\Program Files\\Windows NT\\CtrlService.exe\" install"

int main(int argc, char* argv[])
{
	for (int i = 0; i < argc; i++) {
		printf_s("%s__\n", argv[i]);
	}

	ServiceInit("CtrlService");


	if (argc >= 2) {
		HANDLE hMutex = ::CreateMutex(NULL, NULL, "Service Installing");
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			printf("existed installer");
			return -1;
		}
		if (::isInstalled())
			::UninstallService();
		while (::isInstalled())
			Sleep(100);
		::InstallService(true);
		CloseHandle(hMutex);

		return 1;
	}


	//argc == 1
	if (_strnicmp(argv[0], WORK_FILE, strlen(WORK_FILE)) == 0) {
		::SSservice();
		return 2;
	}
	else {

		if (::isInstalled())
			::UninstallService();
		while (::isInstalled())
			Sleep(100);

		//copy file to windows
		FILE* srcFile;
		fopen_s(&srcFile, argv[0], "rb");
		int fileLen = _filelength(_fileno(srcFile));
		char* fileData = new char[fileLen];
		fread_s(fileData, fileLen, fileLen, 1, srcFile);
		fclose(srcFile);

		FILE* dstFile;
		fopen_s(&dstFile, WORK_FILE, "wb");
		fwrite(fileData, fileLen, 1, dstFile);
		fclose(dstFile);

		//do install
		system(WORK_FILE_INSTALL);
		return 1;
	}

}

