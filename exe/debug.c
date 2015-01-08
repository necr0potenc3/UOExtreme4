#include <windows.h>
#include <stdio.h>
#include "..\dll\uoehelper.h"
#include "uoextreme.h"
#include "mainpage3.h"
#include "objectdefinitioneditor.h"


#define FLAG_SINGLE_STEP 0x0100;

#define MAX_BREAKPOINTS 1
BYTE OPCODE_INT3 = 0xcc;

//#define BREAKPOINT_ADDRESS_SENDCOMMAND ((PVOID)0x00480E63)

#define BREAKPOINT_ADDRESS_SENDCOMMAND ((PVOID)0x00481E23)

#define BREAKPOINT_SENDCOMMAND 0


typedef struct {
	BYTE bOriginalOpCode;
	PVOID pAddress;
} BREAKPOINT;


DWORD threadMonitorDebugEvents(char* pszCmdLine) {
    STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	DEBUG_EVENT de = { 0 };
	CREATE_PROCESS_DEBUG_INFO cpdi = { 0 };
	BOOL bExitLoop = 0;
	CONTEXT context = { 0 };
	BREAKPOINT bps[MAX_BREAKPOINTS] = { 0 };
	DWORD dwBytes = 0;
	UINT i = 0, j = 0;
	char szText[1024] = { 0 };
	BYTE baBuffer[1024] = { 0 };
	BOOL bException = FALSE;
	DWORD dwObjectId = 0;
#ifdef _DEBUG
	FILE* fp = NULL;
#endif

	EnableMenuItem(GetSystemMenu(g_hwndMain, FALSE), SC_CLOSE, MF_DISABLED | MF_GRAYED);
	EnableWindow(GetDlgItem(g_hwndMain, IDC_LAUNCH), FALSE);
    
    si.cb = sizeof(STARTUPINFO);      
    if (CreateProcess(NULL, pszCmdLine, NULL, NULL, TRUE, DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &si, &pi)) {
		bps[0].pAddress = BREAKPOINT_ADDRESS_SENDCOMMAND;
		ReadProcessMemory(pi.hProcess, bps[0].pAddress, &bps[0].bOriginalOpCode, 1, &dwBytes);
		WriteProcessMemory(pi.hProcess, bps[0].pAddress, &OPCODE_INT3, 1, &dwBytes);

		UltimaOnline_Data.dwThreadId = pi.dwThreadId;

	
		
		do {
		
			WaitForDebugEvent(&de, INFINITE);
			switch (de.dwDebugEventCode) {
			case CREATE_PROCESS_DEBUG_EVENT:
				cpdi = de.u.CreateProcessInfo;

				break;
			case EXIT_PROCESS_DEBUG_EVENT:
				bExitLoop = TRUE;
				break;
			case OUTPUT_DEBUG_STRING_EVENT:
				ReadProcessMemory(pi.hProcess, de.u.DebugString.lpDebugStringData, szText, de.u.DebugString.nDebugStringLength + 1, &dwBytes);
				OutputDebugString(szText);		
				break;
			case EXCEPTION_DEBUG_EVENT:
				switch (de.u.Exception.ExceptionRecord.ExceptionCode) {

				case EXCEPTION_BREAKPOINT:
					for (i = 0; i < MAX_BREAKPOINTS; i++) {
						if (bps[i].pAddress == de.u.Exception.ExceptionRecord.ExceptionAddress) {
							WriteProcessMemory(pi.hProcess, bps[i].pAddress, &bps[i].bOriginalOpCode, 1, &dwBytes);

							context.ContextFlags = CONTEXT_FULL;
							GetThreadContext(pi.hThread, &context);

							context.Eip--;
							context.EFlags |= FLAG_SINGLE_STEP;
							SetThreadContext(pi.hThread, &context);


							switch (i) {
							case BREAKPOINT_SENDCOMMAND:

								ReadProcessMemory(pi.hProcess, (PVOID)context.Esi, baBuffer, context.Edi, &dwBytes);
								
								switch (baBuffer[0]) {
								case 0x02:
									if (UltimaOnline_Data.Options.bFastWalk) {
										PostThreadMessage(UltimaOnline_Data.dwThreadId, UOEM_SENDWALKOKAY, (WPARAM)baBuffer[2], (LPARAM)0);
									}
									break;
								case 0x08:
									if (UltimaOnline_Data.Options.bPreventAutoClose) {
										dwObjectId = MAKEOBJECTID(&baBuffer[10]);
										for (j = 0; j < MAX_FAKECONTAINERS; j++) {
											if (UltimaOnline_Data.FakeContainers.dwIds[j] == dwObjectId) {

												dwObjectId = MAKEREALID(dwObjectId);
												baBuffer[10] = OBJECTID1(dwObjectId);
												baBuffer[11] = OBJECTID2(dwObjectId);
												baBuffer[12] = OBJECTID3(dwObjectId);
												baBuffer[13] = OBJECTID4(dwObjectId);
												WriteProcessMemory(pi.hProcess, (PVOID)context.Esi, baBuffer, context.Edi, &dwBytes);
												break;
											}
										}
									}

									break;
								case 0x09:
									if (WaitForSingleObject(g_heventGetObjectId, 0) == WAIT_OBJECT_0) {
										g_GetObject_dwObjectId = MAKEOBJECTID(&baBuffer[1]);
										SetEvent(g_heventGotObjectId);
									}
									break;
								case 0x80:
								case 0xa0:
								case 0x91:
								case 0x83:
									Extreme_End();
									break;
								case 0x01:
									if (memcmp(baBuffer, "\x01\xff\xff\xff\xff", 5) == 0) {
										Extreme_End();
									}
									break;
								case 0x00:
									if (!g_hhook) {
										g_hhook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, GetModuleHandle("uoehelper.dll"), pi.dwThreadId);
									}
									Extreme_Begin(&baBuffer[10]);
									break;
								case 0x5d:
									if (!g_hhook) {
										g_hhook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, GetModuleHandle("uoehelper.dll"), pi.dwThreadId);
									}
									Extreme_Begin(&baBuffer[5]);
									break;
								case 0x6c:
									UltimaOnline_Data.TargetCursor.bEnabled = FALSE;
									UltimaOnline_Data.TargetCursor.dwId = 0;

									//g_bIsTargetCursor = FALSE;
									ResetEvent(g_heventTargetCursor);
									if (MAKEOBJECTID(&baBuffer[7]) != 0 && MAKEOBJECTID(&baBuffer[7]) != UltimaOnline_Data.Character.dwId) {
										UltimaOnline_Data.History.dwLastTargetId = MAKEOBJECTID(&baBuffer[7]);
									}
									break;
								}

#ifdef _DEBUG
								fp = fopen("c:\\temp\\d.dmp", "at");
								if (fp) {
									wsprintf(szText, "SEND[%d]", dwBytes);
									fputs(szText,fp);
									for (j = 0; j < dwBytes; j++) {
										if (isprint((BYTE)baBuffer[j])) {
											//wsprintf(szText, " %c", (BYTE)baBuffer[j]);
											wsprintf(szText, " %02x", (BYTE)baBuffer[j]);			
										} else {
											wsprintf(szText, " %02x", (BYTE)baBuffer[j]);			

										}
										fwrite(szText, strlen(szText), 1, fp);
									}	
									fputs("\n",fp);
									fclose(fp);
								}
#endif

								break;

							}
						}
					}
					break;
				case EXCEPTION_SINGLE_STEP:
					for (i = 0; i < MAX_BREAKPOINTS; i++) {
						WriteProcessMemory(pi.hProcess, bps[i].pAddress, &OPCODE_INT3, 1, &dwBytes);
					}
					break;
				default:
					wsprintf(szText, "Exception %x at address %x. Please copy down this\n information and report it to the programs author.", de.u.Exception.ExceptionRecord.ExceptionCode, de.u.Exception.ExceptionRecord.ExceptionAddress);
					bException = TRUE;
					break;
				}
				break;

			}
			ContinueDebugEvent(pi.dwProcessId, pi.dwThreadId, DBG_CONTINUE);


			if (bException) {
				TerminateProcess(pi.hProcess, 0);

			}

		} while (!bExitLoop);

		if (bException) {
			MessageBox(g_hwndMain, szText, "Fater error...", MB_OK);

		}


	} else {
		MessageBox(g_hwndMain, "Cannot launch Ultima Online.  Check file name and retry.", "Error...", MB_OK);
	}


	Extreme_End();

	EnableMenuItem(GetSystemMenu(g_hwndMain, FALSE), SC_CLOSE, MF_ENABLED);
	EnableWindow(GetDlgItem(g_hwndMain, IDC_LAUNCH), TRUE);

	g_hhook = NULL;


	return TRUE;

}

