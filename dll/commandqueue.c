#include <windows.h>
#include <stdio.h>
#include "uoehelper.h"
#include "crypt.h"
#include "processors.h"
#include "process.h"
#include "commandqueue.h"

COMMANDQUEUE_DATA CommandQueue_Data = { 0 };


void threadCommandQueue_Cleanup(LINKLISTINFO* plli, COMMANDQUEUEITEM* pcqd) {

	EnterCriticalSection(&CommandQueue_Data.cs);
	LinkList_Delete2(CommandQueue_Data.pll, plli);
	LeaveCriticalSection(&CommandQueue_Data.cs);


	if (pcqd->pbaWaitForThis) {
		EnterCriticalSection(&CommandQueue_Data.WaitForThis.cs);
		CommandQueue_Data.WaitForThis.bEnabled = FALSE;
		CommandQueue_Data.WaitForThis.pbaBuffer = NULL;
		CommandQueue_Data.WaitForThis.wSize = 0;
		LeaveCriticalSection(&CommandQueue_Data.WaitForThis.cs);

		Memory_Free(pcqd->pbaWaitForThis);
	}


	Memory_Free(pcqd->pbaCommand);
	Memory_Free(pcqd);
}

DWORD threadCommandQueue(PVOID pUnused) {
	COMMANDQUEUEITEM* pcqd;
	LINKLISTINFO* plli;

	DWORD dwTimeOut = 5000;
	static DWORD dwTicks;
	DWORD dwLag, dwTemp;
	DWORD dwAverageLag = 1000;
	char szText[1024];

	for (;;) {
		WaitForSingleObject(CommandQueue_Data.heventActivate, INFINITE);
			
		EnterCriticalSection(&CommandQueue_Data.cs);
		plli = LinkList_GetHead(CommandQueue_Data.pll);
		LeaveCriticalSection(&CommandQueue_Data.cs);
		
		while (plli != NULL) {
			EnterCriticalSection(&CommandQueue_Data.cs);
			pcqd = LinkList_GetObject(CommandQueue_Data.pll, plli) ;
			LeaveCriticalSection(&CommandQueue_Data.cs);
		
			
			if (ExtremeHelper_Data.bEnabled) {
				
				if ((!(pcqd->uFlags & COMMAND_FLAG_RIGHTEMPTY) || (!UltimaOnline_Data.Character.Items.dwRightHandId && (pcqd->uFlags & COMMAND_FLAG_RIGHTEMPTY))) &&
					(!(pcqd->uFlags & COMMAND_FLAG_LEFTEMPTY) || (!UltimaOnline_Data.Character.Items.dwLeftHandId && (pcqd->uFlags & COMMAND_FLAG_LEFTEMPTY)))) {
					
					if (!(pcqd->uFlags & COMMAND_FLAG_NOWAIT)) {
						WaitForSingleObject(g_heventReady, INFINITE);
						dwTemp = GetTickCount() - dwTicks;
						if (dwTemp < dwAverageLag) {
							Sleep(dwAverageLag - dwTemp);
							wsprintf(szText, "stalling for %d\n", dwAverageLag - dwTemp);
							OutputDebugString(szText);
						}

					}
					
					EnterCriticalSection(&CommandQueue_Data.WaitForThis.cs);
					CommandQueue_Data.WaitForThis.bEnabled = TRUE;
					CommandQueue_Data.WaitForThis.pbaBuffer = pcqd->pbaWaitForThis;
					CommandQueue_Data.WaitForThis.wSize = pcqd->wWaitForThisSize;
					ResetEvent(CommandQueue_Data.WaitForThis.hevents[CQWFTE_SUCCESS]);
					ResetEvent(CommandQueue_Data.WaitForThis.hevents[CQWFTE_FAILURE]);
					LeaveCriticalSection(&CommandQueue_Data.WaitForThis.cs);
					
					dwTicks = GetTickCount();
				
					ResetEvent(CommandQueue_Data.heventExecuted);
					PostThreadMessage(UltimaOnline_Data.dwThreadId, QCM_EXECUTE, (WPARAM)pcqd, (LPARAM)0);
					WaitForSingleObject(CommandQueue_Data.heventExecuted, INFINITE);
					
					
					if (CommandQueue_Data.bExecutionSuccessful) {
						
						if (pcqd->pbaWaitForThis) {
							switch (WaitForMultipleObjects(2, CommandQueue_Data.WaitForThis.hevents, FALSE, dwTimeOut)) {
							case WAIT_TIMEOUT:
								OutputDebugString("Timeout\n");
								pcqd->bRetry--;
								if (pcqd->bRetry == 0) {
									OutputDebugString("Max Timeouts Reached! Skipping command!\n");
									
									threadCommandQueue_Cleanup(plli, pcqd);
								}
								break;
							case WAIT_OBJECT_0 + CQWFTE_FAILURE:
								OutputDebugString("you must wait to perfrom another action!\n");
								break;
							case WAIT_OBJECT_0 + CQWFTE_SUCCESS:
								dwLag = GetTickCount() - dwTicks;
								dwAverageLag = ((dwAverageLag * 9) + dwLag) / 10;
								dwTimeOut = dwAverageLag * 5;
								
								wsprintf(szText, "lag=%d averagelag=%d timeout=%d\n", dwLag, dwAverageLag, dwTimeOut);
								OutputDebugString(szText);
								
								threadCommandQueue_Cleanup(plli, pcqd);
								break;
							}
						} else {
							threadCommandQueue_Cleanup(plli, pcqd);
						}
						
					}
					
				
				} else {
					threadCommandQueue_Cleanup(plli, pcqd);
				}
			} else {
				OutputDebugString("Flushing command\n");
				threadCommandQueue_Cleanup(plli, pcqd);
			}
			
			EnterCriticalSection(&CommandQueue_Data.cs);
			plli = LinkList_GetHead(CommandQueue_Data.pll);
			LeaveCriticalSection(&CommandQueue_Data.cs);
		}
	}





	return 0;
}

void CommandQueue_OnExecute(COMMANDQUEUEITEM* pcqd) {

	CommandQueue_Data.bExecutionSuccessful = FALSE;

	if ((pcqd->uFlags & COMMAND_FLAG_NOWAIT) || (WaitForSingleObject(g_heventReady, 0) == WAIT_OBJECT_0)) {

		CommandQueue_Data.bExecutionSuccessful = TRUE;

		g_wOutBufferSize = pcqd->wCommandSize;

		CopyMemory(g_baOutBuffer, pcqd->pbaCommand, pcqd->wCommandSize);

		SendCommand();


		if (pcqd->uFlags & COMMAND_FLAG_KILLTARGETCURSOR) {
			if (UltimaOnline_Data.TargetCursor.bVisible) {
				OutputDebugString("Cancelling targetcursor!\n");
				UltimaOnline_Data.TargetCursor.bVisible = FALSE;
				PostMessage(UltimaOnline_Data.hwnd, WM_KEYDOWN, (WPARAM)VK_ESCAPE, (LPARAM)0);				
			}
		}
	

	}
	
	SetEvent(CommandQueue_Data.heventExecuted);

}

void CommandQueue_Initialize(void) {
	DWORD dwThreadId;

	if (!CommandQueue_Data.bInitialized) {
		CommandQueue_Data.bInitialized = TRUE;

		InitializeCriticalSection(&CommandQueue_Data.cs);
		InitializeCriticalSection(&CommandQueue_Data.WaitForThis.cs);
		
		CommandQueue_Data.heventActivate = CreateEvent(NULL, FALSE, FALSE, NULL);
		CommandQueue_Data.heventExecuted = CreateEvent(NULL, TRUE, FALSE, NULL);
		CommandQueue_Data.WaitForThis.hevents[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
		CommandQueue_Data.WaitForThis.hevents[1] = CreateEvent(NULL, TRUE, FALSE, NULL);

		CommandQueue_Data.pll = LinkList_Create();

		CommandQueue_Data.hthread = CreateThread(NULL, 0x1000, (PTHREAD_START_ROUTINE)threadCommandQueue, NULL, 0, &dwThreadId);
		SetThreadPriority(CommandQueue_Data.hthread, THREAD_PRIORITY_ABOVE_NORMAL);
	}
}

void CommandQueue_Add(BYTE* pbaCommand, WORD wCommandSize, BYTE* pbaWaitForThis, WORD wWaitForThisSize, BYTE bPriority, UINT uFlags) {
	COMMANDQUEUEITEM* pcqd = NULL;
	COMMANDQUEUEITEM* pcqdNew = NULL;
	LINKLISTINFO* plli = NULL;
	BOOL bAdded = FALSE;

	//OutputDebugString("CommandQueue_Add\n");

	pcqdNew = Memory_Allocate(sizeof(COMMANDQUEUEITEM));
	pcqdNew->bRetry = 3;
	pcqdNew->bPriority = bPriority;
	pcqdNew->uFlags = uFlags;


	pcqdNew->pbaCommand = Memory_Allocate(wCommandSize);
	CopyMemory(pcqdNew->pbaCommand, pbaCommand, wCommandSize);
	pcqdNew->wCommandSize = wCommandSize;


	if (pbaWaitForThis) {
		pcqdNew->pbaWaitForThis = Memory_Allocate(wWaitForThisSize);
		CopyMemory(pcqdNew->pbaWaitForThis, pbaWaitForThis, wWaitForThisSize);
		pcqdNew->wWaitForThisSize = wWaitForThisSize;
	} else {
		pcqdNew->pbaWaitForThis = NULL;
		pcqdNew->wWaitForThisSize = 0;
	}


	EnterCriticalSection(&CommandQueue_Data.cs);
	plli = LinkList_GetHead(CommandQueue_Data.pll);
	while (plli != NULL) {
		pcqd = LinkList_GetObject(CommandQueue_Data.pll, plli) ;

		if (pcqdNew->bPriority > pcqd->bPriority) {
			LinkList_Insert(CommandQueue_Data.pll, plli, pcqdNew);
			bAdded = TRUE;
			break;
		}
		plli = LinkList_GetNext(CommandQueue_Data.pll, plli);
	}

	if (!bAdded) {
		LinkList_Add(CommandQueue_Data.pll, pcqdNew);
	}

	LeaveCriticalSection(&CommandQueue_Data.cs);

	SetEvent(CommandQueue_Data.heventActivate);

}
