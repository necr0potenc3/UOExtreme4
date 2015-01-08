#include <windows.h>
#include <winsock.h>
#include "uoehelper.h"
#include "hook.h"
#include "process.h"
#include "processors.h"
#include "processmacros.h"
#include "commandqueue.h"
#include "commands.h"
#include "bank.h"

#include <stdio.h>
#include <stdlib.h>

EXTREMEHELPER_DATA ExtremeHelper_Data = { 0 };

void ExtremeHelper_HookFunctions(void);
void ExtremeHelper_UnhookFunctions(void);


void ExtremeHelper_Begin(void) {
	if (!ExtremeHelper_Data.bInitialized) {		

		UltimaOnline_Data.hwnd = FindWindow("Ultima Online", NULL);

		g_heventReady = CreateEvent(NULL, TRUE, FALSE, NULL);
		g_heventNotReady = CreateEvent(NULL, TRUE, FALSE, NULL);
		g_heventTargetCursor = OpenEvent(EVENT_ALL_ACCESS, FALSE, "g_heventTargetCursor");
		g_heventCountDone = CreateEvent(NULL, TRUE, FALSE, NULL);

		ObjectDefinitions_Initialize();
		Macros_Initialize();

		CommandQueue_Initialize();
		Commands_Initialize();

		Processors_Initialize();


		ExtremeHelper_Data.bInitialized = TRUE;
	}

	if (!ExtremeHelper_Data.bEnabled) {

		ExtremeHelper_HookFunctions();

	//	ObjectDefinitions_Load();
	//	Macros_Load();

		ExtremeHelper_Data.bEnabled = TRUE;
	}

	Bank_Load();
}



void ExtremeHelper_End(void) {
	if (ExtremeHelper_Data.bEnabled) {

		ExtremeHelper_UnhookFunctions();

		UltimaOnline_Data.FakeContainers.nPosition = 0;

		//CommandQueue_Flush();

		ExtremeHelper_Data.bEnabled = FALSE;
	}

}


//HWND g_hwnd;

BOOL g_bWatchKeys = FALSE;

SOCKET g_socketReceive;

struct _HOOK_DATA {
	PROC pfnOriginalProc;
	PROC pfnHookProc;
	char* pszFunctionName;
};

typedef struct _HOOK_DATA HOOK_DATA;

#define MAX_HOOKS 3

#define HOOK_RECV 0
#define HOOK_SEND 1
#define HOOK_SELECT 2

int PASCAL FAR hook_select(int nfds, fd_set FAR *readfds, fd_set FAR *writefds, fd_set FAR *exceptfds, const struct timeval FAR *timeout);
int PASCAL FAR hook_recv(SOCKET s, char FAR * buf, int len, int flags);
int PASCAL FAR hook_send(SOCKET s, const char FAR * buf, int len, int flags);

HOOK_DATA g_hdFunctionHooks[MAX_HOOKS] = {
	{ NULL, &hook_recv, "recv" },
	{ NULL, &hook_send, "send" },
	{ NULL, &hook_select, "select" }
};


int PASCAL FAR hook_select(int nfds, fd_set FAR *readfds, fd_set FAR *writefds, fd_set FAR *exceptfds, const struct timeval FAR *timeout) {
	if (Commands_Fake_IsAvailable()) {
		if (readfds) {
			if (writefds) FD_ZERO(writefds);
			if (exceptfds) FD_ZERO(exceptfds);

			FD_ZERO(readfds);
			FD_SET(g_socketReceive, readfds);
			return 1;

		}
	}
	return (g_hdFunctionHooks[HOOK_SELECT].pfnOriginalProc)(nfds, readfds, writefds, exceptfds, timeout);
}

int PASCAL FAR hook_recv(SOCKET s, char FAR * buf, int len, int flags) {
	int iReturn;

	if (Commands_Fake_IsAvailable()) {
		iReturn = Commands_Fake_Get(buf);
	} else {
		iReturn = (g_hdFunctionHooks[HOOK_RECV].pfnOriginalProc)(s, buf, len, flags);
		if (iReturn != SOCKET_ERROR) {
			if (s != g_socketReceive) {
				Reset();
				g_socketReceive = s;
				OutputDebugString("Socket changed\n");
			}
			iReturn = ProcessNewData(buf, iReturn);
		}
	}

	return iReturn;

}



int PASCAL FAR hook_send(SOCKET s, const char FAR * buf, int len, int flags) {
/*	FILE* fp;
	char szText[1024];

	fp = fopen("c:\\temp\\d.dmp", "at");
	if (fp) {
		wsprintf(szText, "CSEND[%d@%d]\n", len, GetTickCount());
		fputs(szText, fp);
		fclose(fp);
	}

*/
	return (g_hdFunctionHooks[HOOK_SEND].pfnOriginalProc)(s, buf, len, flags);
}



void ExtremeHelper_HookFunctions(void) {
	int i;

	for (i = 0; i < MAX_HOOKS; i++) {
		g_hdFunctionHooks[i].pfnOriginalProc = HookImportedFunction(GetModuleHandle(NULL), 
			"wsock32.dll", g_hdFunctionHooks[i].pszFunctionName, g_hdFunctionHooks[i].pfnHookProc);
	}

}

void ExtremeHelper_UnhookFunctions(void) {
	int i;

	for (i = 0; i < MAX_HOOKS; i++) {
		if (!UnhookImportedFunction(GetModuleHandle(NULL), 
			"wsock32.dll", g_hdFunctionHooks[i].pfnHookProc, g_hdFunctionHooks[i].pfnOriginalProc)) {
			OutputDebugString("Function restore failed\n");
		}
	}

}


DLL LRESULT CALLBACK GetMsgProc(int iCode, WPARAM wParam, LPARAM lParam) {
	char szKeyName[64];
	MACRO* pmacro;
	DWORD dwThreadId;
	HANDLE hthread;

	if (iCode < 0) 
		return CallNextHookEx(g_hhook, iCode, wParam, lParam);

	switch (((MSG*)lParam)->message) {
	case WM_SYSKEYUP:
	case WM_KEYUP:
		if (((MSG*)lParam)->wParam == VK_MENU) {
			((MSG*)lParam)->wParam = ~0;
		}
		break;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		//OutputDebugString("WM_KEYDOWN\n");
		if (ExtremeHelper_Data.bEnabled) {
			GetKeyNameText(((MSG*)lParam)->lParam, szKeyName, sizeof(szKeyName));

			pmacro = Macros_GetFirst();

			while (pmacro != NULL) {
				if (strcmpi(pmacro->szKeyName, szKeyName) == 0) {

					if ((((GetKeyState(VK_CONTROL) >> 15) & 1) == pmacro->bCtrl) &&
						(((GetKeyState(VK_MENU) >> 15) & 1) == pmacro->bAlt) &&
						(((GetKeyState(VK_SHIFT) >> 15) & 1) == pmacro->bShift)) {

						hthread = CreateThread(NULL, 0x1000, (PTHREAD_START_ROUTINE)threadProcessMacro, pmacro, 0, &dwThreadId);
						(((MSG*)lParam)->wParam) = (~0);
					}
					break;
				} 
				pmacro = Macros_GetNext(pmacro);
			}
		}
		break;
	case UOEM_OBJECTDEFINITIONSCHANGED:
		OutputDebugString("UOEM_OBJECTDEFINITIONSCHANGED\n");
		ObjectDefinitions_Load();
		break;
	case UOEM_MACROSCHANGED:
		OutputDebugString("UOEM_MACROSCHANGED\n");
		Macros_Load();
		break;
	case UOEM_BEGIN:
		OutputDebugString("UOEM_BEGIN\n");
		ExtremeHelper_Begin();
		break;
	case WM_QUIT:
		UnhookWindowsHookEx(g_hhook);
		g_hhook = 0;
	case UOEM_END:
		OutputDebugString("UOEM_END\n");
		ExtremeHelper_End();
		break;
	case UOEM_GETOBJECTNAME:
		OutputDebugString("UOEM_GETOBJECTNAME\n");
		g_bWantObjectName = TRUE;
		break;
	case QCM_EXECUTE:
		//OutputDebugString("QCM_EXECUTE\n");
		CommandQueue_OnExecute((COMMANDQUEUEITEM*)((MSG*)lParam)->wParam);
		break;
	case UOEM_SENDWALKOKAY:
		//Processors_Outgoing_Walk();
		Commands_WalkOkay((BYTE)((MSG*)lParam)->wParam);
		break;
	case UOED_SEND:
		g_hdFunctionHooks[HOOK_SEND].pfnOriginalProc = HookImportedFunction(GetModuleHandle(NULL), 
			"wsock32.dll", g_hdFunctionHooks[HOOK_SEND].pszFunctionName, g_hdFunctionHooks[HOOK_SEND].pfnHookProc);
		OutputDebugString("UOEM_SEND\n");
		break;
	}


	return CallNextHookEx(g_hhook, iCode, wParam, lParam);

}
