#include <windows.h>
#include <stdio.h>
#include "uoehelper.h"
#include "crypt.h"
#include "processors.h"
#include "process.h"
#include "commandqueue.h"
#include "commands.h"

typedef void (__stdcall *SENDCOMMAND)(char* pszBuffer);

BOOL g_bCountObjects;
//UINT g_nNumberOfPacksWaiting;
UINT g_nNumberOfArrows;
UINT g_nNumberOfBolts;
UINT g_nNumberOfSulphurousAsh;
UINT g_nNumberOfBloodMoss;
UINT g_nNumberOfMandrakeRoot;
UINT g_nNumberOfSpiderSilk;
UINT g_nNumberOfGarlic;
UINT g_nNumberOfGinseng;
UINT g_nNumberOfNightshade;
UINT g_nNumberOfBlackPearl;


HANDLE g_heventCountDone;
HANDLE g_heventTargetCursor;
HANDLE g_heventReady;
HANDLE g_heventNotReady;


//SENDCOMMAND g_pfnSendCommand = (SENDCOMMAND)0x480e20;
SENDCOMMAND g_pfnSendCommand = (SENDCOMMAND)0x481de0;
//WORD* g_pwCommandSize = (WORD*)0x753f70;
WORD* g_pwCommandSize = (WORD*)0x0075A4E8;

BOOL g_bWantObjectName = FALSE; 

BYTE* g_pRawBuffer;
BYTE* g_pBuffer;
UINT g_nRawBufferBytesInUse;
UINT g_nBufferBytesInUse;


BYTE* g_pRawOutBuffer;
UINT g_nRawOutBufferBytesInUse;


void Reset(void) {
	g_nRawBufferBytesInUse = 0;
	g_nBufferBytesInUse = 0;
	g_nRawOutBufferBytesInUse = 0;

}

void SendCommand(void) {

	if (g_bDisableSendCommand) {
		return;
	}

	_asm pushad

	*g_pwCommandSize = g_wOutBufferSize;
//	_asm mov ebx, [0x6aa6dc]
	_asm mov ebx, [0x6ADA3C]
	_asm mov ecx, [ebx]

	g_pfnSendCommand(g_baOutBuffer);

	_asm popad
}


void ProcessData(BYTE* pData, UINT nSize) {
	BOOL bFilter = FALSE;
#ifdef _DEBUG
	FILE* fp = NULL;
	char szText[1024] = { 0 };
	UINT i = 0;
#endif

	EnterCriticalSection(&CommandQueue_Data.WaitForThis.cs);

	if (CommandQueue_Data.WaitForThis.bEnabled) {
		if (memcmp(pData, CommandQueue_Data.WaitForThis.pbaBuffer, CommandQueue_Data.WaitForThis.wSize) == 0) {
			CommandQueue_Data.WaitForThis.bEnabled = FALSE;
			CommandQueue_Data.WaitForThis.pbaBuffer = NULL;
			CommandQueue_Data.WaitForThis.wSize = 0;

			SetEvent(CommandQueue_Data.WaitForThis.hevents[CQWFTE_SUCCESS]);
		}
	}

	LeaveCriticalSection(&CommandQueue_Data.WaitForThis.cs);




	switch (pData[0]) {
	case 0x25:
		bFilter = Processors_Incoming_MoveItem(pData);
		break;
	case 0x1a:
		bFilter = Processors_Incoming_PutItem(pData);
		break;
	case 0x65:
		bFilter = UltimaOnline_Data.Filters.bWeather;
		break;
	case 0x4f:
		if (UltimaOnline_Data.Options.bAlwaysLight) {
			pData[1] = 0;
		}
		break;
	case 0x77:
		bFilter = Processors_Incoming_Walk(pData);
		break;
	case 0x54:
		bFilter = Processors_Incoming_PlaySound(pData);
		break;
	case 0x22:
		bFilter = UltimaOnline_Data.Options.bFastWalk;
		//Commands_OpenBank(0x40e79737);
		break;
	case 0x24:
		bFilter = Processors_Incoming_ContainerInfo(pData);
		break;
	case 0x3c:
		bFilter = Processors_Incoming_ItemsInContainer(pData);
		break;
	case 0x1d:
		if (MAKEOBJECTID(&pData[1]) == UltimaOnline_Data.Character.Items.dwRightHandId) {
			OutputDebugString("Clearing right hand.\n");
			UltimaOnline_Data.Character.Items.dwRightHandId = 0;
		} else if (MAKEOBJECTID(&pData[1]) == UltimaOnline_Data.Character.Items.dwLeftHandId) {
			OutputDebugString("Clearing left hand.\n");
			UltimaOnline_Data.Character.Items.dwLeftHandId = 0;
		}

		break;
	case 0x33:
		switch (pData[1]) {
		case 0:
			//OutputDebugString("Ready\n");
			SetEvent(g_heventReady);
			ResetEvent(g_heventNotReady);
			break;
		case 1:
			//OutputDebugString("Not Ready\n");
			SetEvent(g_heventNotReady);
			ResetEvent(g_heventReady);
			break;
		}
		break;
	case 0x1c:
		bFilter = Processors_Incoming_Speech(pData);
		break;
	case 0x1b:
		UltimaOnline_Data.Character.dwId = MAKEOBJECTID(&pData[1]);
		break;
	case 0x78:
		Processors_Incoming_LoadPlayer(pData);
		break;
	case 0x2e:
		Processors_Incoming_WearItem(pData);
		break;
	case 0x6c:
		UltimaOnline_Data.TargetCursor.bEnabled = TRUE;
		UltimaOnline_Data.TargetCursor.dwId = MAKEOBJECTID(&pData[2]);

		UltimaOnline_Data.TargetCursor.bVisible = !UltimaOnline_Data.Filters.bTargetCursor;
		bFilter = UltimaOnline_Data.Filters.bTargetCursor;

		SetEvent(g_heventTargetCursor);
		break;

	}

#ifdef _DEBUG
	fp = fopen("c:\\temp\\d.dmp", "at");
	if (fp) {
		if (bFilter) {
			wsprintf(szText, "FILTERED[%d]", nSize);
		} else {
			wsprintf(szText, "RECV[%d]", nSize);
		}
		fputs(szText, fp);
		for (i = 0; i < nSize; i++) {
			if (isprint((BYTE)pData[i])) {
//				wsprintf(szText, " %c", (BYTE)pData[i]);
				wsprintf(szText, " %02x", (BYTE)pData[i]);			
			} else {
				wsprintf(szText, " %02x", (BYTE)pData[i]);			

			}
			fwrite(szText, strlen(szText), 1, fp);
		}	
		fputs("\n",fp);



		fclose(fp);
	}
#endif
	if (!bFilter) {
		g_nRawOutBufferBytesInUse += Encrypt((g_pRawOutBuffer + g_nRawOutBufferBytesInUse), pData, nSize);
	}
}



UINT ProcessNewData(BYTE* pData, UINT nSize) {

	if (!g_pBuffer) {
		g_pRawBuffer = VirtualAlloc(NULL, 0x10000,  MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		g_pBuffer = VirtualAlloc(NULL, 0x10000,  MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		g_pRawOutBuffer = VirtualAlloc(NULL, 0x10000,  MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	}

	g_nRawOutBufferBytesInUse = 0;

	memmove(&g_pRawBuffer[g_nRawBufferBytesInUse], pData, nSize);

	g_nRawBufferBytesInUse = Decrypt(g_pBuffer, g_pRawBuffer, g_nRawBufferBytesInUse + nSize, (PROCESSDATA)ProcessData);

	memmove(pData, g_pRawOutBuffer, g_nRawOutBufferBytesInUse);
	
	return g_nRawOutBufferBytesInUse;	
}
