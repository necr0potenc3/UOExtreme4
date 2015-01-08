#include <windows.h>
#include <stdio.h>
#include "uoehelper.h"
#include "crypt.h"
#include "processors.h"
#include "process.h"
#include "commandqueue.h"
#include "commands.h"

struct {
	BOOL bInitialized;

	CRITICAL_SECTION cs;
	BYTE* pbaBuffer;
	DWORD dwBufferBytesInUse;



} Commands_Data = { 0 };


void Commands_Initialize(void) {

	//OutputDebugString("Commands_Initialize\n");

	if (!Commands_Data.bInitialized) {
		InitializeCriticalSection(&Commands_Data.cs);
		Commands_Data.pbaBuffer = VirtualAlloc(NULL, 0x10000,  MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		Commands_Data.dwBufferBytesInUse = 0;
	}
}

BOOL Commands_Fake_IsAvailable(void) {
	BOOL bReturn;

	//OutputDebugString("Commands_Fake_IsAvailable\n");

	EnterCriticalSection(&Commands_Data.cs);
	bReturn = (Commands_Data.dwBufferBytesInUse > 0);
	LeaveCriticalSection(&Commands_Data.cs);

	return bReturn;

}

int Commands_Fake_Get(BYTE* pbaBuffer) {
	int iReturn;

	//OutputDebugString("Commands_Fake_Get\n");

	EnterCriticalSection(&Commands_Data.cs);
	iReturn = Commands_Data.dwBufferBytesInUse;
	CopyMemory(pbaBuffer, Commands_Data.pbaBuffer, Commands_Data.dwBufferBytesInUse);
	Commands_Data.dwBufferBytesInUse = 0;
	LeaveCriticalSection(&Commands_Data.cs);

	return iReturn;

}

void Commands_Fake_Add(BYTE* pbaCommand, DWORD nSize) {
#ifdef _DEBUG
	FILE* fp = NULL;
	char szText[1024] = { 0 };
	UINT i = 0;
#endif

	//OutputDebugString("Commands_Fake_Add\n");

#ifdef _DEBUG
	fp = fopen("c:\\temp\\d.dmp", "at");
	if (fp) {
		wsprintf(szText, "FAKERECV[%d]", nSize);
		fputs(szText, fp);
		for (i = 0; i < nSize; i++) {
			if (isprint((BYTE)pbaCommand[i])) {
//				wsprintf(szText, " %c", (BYTE)pData[i]);
				wsprintf(szText, " %02x", (BYTE)pbaCommand[i]);			
			} else {
				wsprintf(szText, " %02x", (BYTE)pbaCommand[i]);			

			}
			fwrite(szText, strlen(szText), 1, fp);
		}	
		fputs("\n",fp);



		fclose(fp);
	}
#endif


	EnterCriticalSection(&Commands_Data.cs);
	Commands_Data.dwBufferBytesInUse += Encrypt((Commands_Data.pbaBuffer + Commands_Data.dwBufferBytesInUse), pbaCommand, nSize);
	LeaveCriticalSection(&Commands_Data.cs);
}


void Commands_EquipObject(BYTE bSlot, DWORD dwObjectId, UINT uFlags) {
	BYTE baCommand[10] = { 0 };
	BYTE baWaitForThis[5] = { 0 };

	OutputDebugString("Commands_EquipObject\n");

	baCommand[0] = 0x13;
	baCommand[1] = OBJECTID1(dwObjectId);
	baCommand[2] = OBJECTID2(dwObjectId);
	baCommand[3] = OBJECTID3(dwObjectId);
	baCommand[4] = OBJECTID4(dwObjectId);
	baCommand[5] = bSlot;
	baCommand[6] = OBJECTID1(UltimaOnline_Data.Character.dwId);
	baCommand[7] = OBJECTID2(UltimaOnline_Data.Character.dwId);
	baCommand[8] = OBJECTID3(UltimaOnline_Data.Character.dwId);
	baCommand[9] = OBJECTID4(UltimaOnline_Data.Character.dwId);

	baWaitForThis[0] = 0x1d;
	baWaitForThis[1] = OBJECTID1(dwObjectId);
	baWaitForThis[2] = OBJECTID2(dwObjectId);
	baWaitForThis[3] = OBJECTID3(dwObjectId);
	baWaitForThis[4] = OBJECTID4(dwObjectId);

	CommandQueue_Add(baCommand, 10, baWaitForThis, 5, COMMAND_PRIORITY_NORMAL, uFlags);
}

void Commands_GetObject(DWORD dwObjectId, UINT uFlags) {
	BYTE baCommand[7] = { 0 };
	BYTE baWaitForThis[5] = { 0 };

	OutputDebugString("Commands_GetObject\n");

	baCommand[0] = 0x07;
	baCommand[1] = OBJECTID1(dwObjectId);
	baCommand[2] = OBJECTID2(dwObjectId);
	baCommand[3] = OBJECTID3(dwObjectId);
	baCommand[4] = OBJECTID4(dwObjectId);
	baCommand[5] = 0;
	baCommand[6] = 0;

	baWaitForThis[0] = 0x1d;
	baWaitForThis[1] = OBJECTID1(dwObjectId);
	baWaitForThis[2] = OBJECTID2(dwObjectId);
	baWaitForThis[3] = OBJECTID3(dwObjectId);
	baWaitForThis[4] = OBJECTID4(dwObjectId);

	CommandQueue_Add(baCommand, 7, baWaitForThis, 5, COMMAND_PRIORITY_NORMAL, uFlags);
}

void Commands_PutObject(DWORD dwObjectId, DWORD dwWhereId, UINT uFlags) {
	BYTE baCommand[14] = { 0 };
	BYTE baWaitForThis[5] = { 0 };

	OutputDebugString("Commands_PutObject\n");

	baCommand[0] = 0x08;
	baCommand[1] = OBJECTID1(dwObjectId);
	baCommand[2] = OBJECTID2(dwObjectId);
	baCommand[3] = OBJECTID3(dwObjectId);
	baCommand[4] = OBJECTID4(dwObjectId);
	baCommand[5] = 0xff;
	baCommand[6] = 0xff;
	baCommand[7] = 0xff;
	baCommand[8] = 0xff;
	baCommand[9] = 0;
	baCommand[10] = OBJECTID1(dwWhereId);
	baCommand[11] = OBJECTID2(dwWhereId);
	baCommand[12] = OBJECTID3(dwWhereId);
	baCommand[13] = OBJECTID4(dwWhereId);

	baWaitForThis[0] = 0x1d;
	baWaitForThis[1] = OBJECTID1(dwObjectId);
	baWaitForThis[2] = OBJECTID2(dwObjectId);
	baWaitForThis[3] = OBJECTID3(dwObjectId);
	baWaitForThis[4] = OBJECTID4(dwObjectId);

	CommandQueue_Add(baCommand, 14, baWaitForThis, 5, COMMAND_PRIORITY_NORMAL, uFlags);
}

void Commands_CastSpell(BYTE bSpellNumber) {
	BYTE baCommand[64] = { 0 };
	BYTE baWaitForThis[5] = { 0 };
	WORD nSize = 0;

	OutputDebugString("Commands_CastSpell\n");


	itoa(bSpellNumber, baCommand + 4, 10);

	nSize = strlen(baCommand + 4) + 5;

	baCommand[0] = 0x12;
	baCommand[1] = LOBYTE(nSize);
	baCommand[2] = HIBYTE(nSize);
	baCommand[3] = 0x56;

	baWaitForThis[0] = 0x6e;
	baWaitForThis[1] = OBJECTID1(UltimaOnline_Data.Character.dwId);
	baWaitForThis[2] = OBJECTID2(UltimaOnline_Data.Character.dwId);
	baWaitForThis[3] = OBJECTID3(UltimaOnline_Data.Character.dwId);
	baWaitForThis[4] = OBJECTID4(UltimaOnline_Data.Character.dwId);

	CommandQueue_Add(baCommand, nSize, baWaitForThis, 5, COMMAND_PRIORITY_NORMAL, 0);
}

void Commands_Target(DWORD dwObjectId) {
	BYTE baCommand[19] = { 0 };

	OutputDebugString("Commands_Target\n");

	baCommand[0] = 0x6c;
	baCommand[2] = OBJECTID1(UltimaOnline_Data.TargetCursor.dwId);
	baCommand[3] = OBJECTID2(UltimaOnline_Data.TargetCursor.dwId);
	baCommand[4] = OBJECTID3(UltimaOnline_Data.TargetCursor.dwId);
	baCommand[5] = OBJECTID4(UltimaOnline_Data.TargetCursor.dwId);
	baCommand[7] = OBJECTID1(dwObjectId);
	baCommand[8] = OBJECTID2(dwObjectId);
	baCommand[9] = OBJECTID3(dwObjectId);
	baCommand[10] = OBJECTID4(dwObjectId);


	CommandQueue_Add(baCommand, 19, NULL, 0, COMMAND_PRIORITY_NORMAL, COMMAND_FLAG_KILLTARGETCURSOR);
}

void Commands_OpenContainer(DWORD dwObjectId) {
	BYTE baCommand[5] = { 0 };
	BYTE baWaitForThis[5] = { 0 };

	OutputDebugString("Commands_OpenContainer\n");

	baCommand[0] = 0x06;
	baCommand[1] = OBJECTID1(dwObjectId);
	baCommand[2] = OBJECTID2(dwObjectId);
	baCommand[3] = OBJECTID3(dwObjectId);
	baCommand[4] = OBJECTID4(dwObjectId);

	baWaitForThis[0] = 0x24;
	baWaitForThis[1] = OBJECTID1(dwObjectId);
	baWaitForThis[2] = OBJECTID2(dwObjectId);
	baWaitForThis[3] = OBJECTID3(dwObjectId);
	baWaitForThis[4] = OBJECTID4(dwObjectId);

	CommandQueue_Add(baCommand, 5, baWaitForThis, 5, COMMAND_PRIORITY_BELOW_NORMAL, 0);
}

void Commands_OpenBank(DWORD dwObjectId) {
/*	BYTE baCommand[5] = { 0 };
	BYTE baWaitForThis[5] = { 0 };
	BYTE baFakeCommand[15] = { 0  };*/

	OutputDebugString("Commands_OpenBank\n");

/*	baFakeCommand[0] = 0x2e;
	baFakeCommand[1] = OBJECTID1(dwObjectId);
	baFakeCommand[2] = OBJECTID2(dwObjectId);
	baFakeCommand[3] = OBJECTID3(dwObjectId);
	baFakeCommand[4] = OBJECTID4(dwObjectId);
	baFakeCommand[5] = 0x0e;
	baFakeCommand[6] = 0x7c;
	baFakeCommand[7] = 0x00;
	baFakeCommand[8] = 0x1d;
	baFakeCommand[9] = OBJECTID1(UltimaOnline_Data.Character.dwId);
	baFakeCommand[10] = OBJECTID2(UltimaOnline_Data.Character.dwId);
	baFakeCommand[11] = OBJECTID3(UltimaOnline_Data.Character.dwId);
	baFakeCommand[12] = OBJECTID4(UltimaOnline_Data.Character.dwId);
	baFakeCommand[13] = 0x00;
	baFakeCommand[14] = 0x00;

	Commands_Fake_Add(baFakeCommand, 15);
*/
	Commands_ItemEquipped(0x1d, dwObjectId, 0x0e7c);
	Commands_OpenContainer(dwObjectId);
/*	baCommand[0] = 0x06;
	baCommand[1] = OBJECTID1(dwObjectId);
	baCommand[2] = OBJECTID2(dwObjectId);
	baCommand[3] = OBJECTID3(dwObjectId);
	baCommand[4] = OBJECTID4(dwObjectId);

	baWaitForThis[0] = 0x24;
	baWaitForThis[1] = OBJECTID1(dwObjectId);
	baWaitForThis[2] = OBJECTID2(dwObjectId);
	baWaitForThis[3] = OBJECTID3(dwObjectId);
	baWaitForThis[4] = OBJECTID4(dwObjectId);

	CommandQueue_Add(baCommand, 5, baWaitForThis, 5, COMMAND_PRIORITY_NORMAL, 0);*/
}



void Commands_ShowName(DWORD dwObjectId) {
	BYTE baCommand[5] = { 0 };

//	OutputDebugString("Commands_ShowName\n");

	baCommand[0] = 0x09;
	baCommand[1] = OBJECTID1(dwObjectId);
	baCommand[2] = OBJECTID2(dwObjectId);
	baCommand[3] = OBJECTID3(dwObjectId);
	baCommand[4] = OBJECTID4(dwObjectId);

	CommandQueue_Add(baCommand, 5, NULL, 0, COMMAND_PRIORITY_LOWEST, COMMAND_FLAG_NOWAIT);

}

void Commands_SystemMessage(char* pszSystemMessage) {
	BYTE* pbaBuffer;
	WORD nSize;

	OutputDebugString("Commands_SystemMessage\n");

	nSize = 44 + strlen(pszSystemMessage) + 1;

	pbaBuffer = Memory_Allocate(nSize);

	pbaBuffer[0] = 0x1c;
	pbaBuffer[1] = HIBYTE(nSize);
	pbaBuffer[2] = LOBYTE(nSize);
	pbaBuffer[3] = 0xff;
	pbaBuffer[4] = 0xff;
	pbaBuffer[5] = 0xff;
	pbaBuffer[6] = 0xff;
	pbaBuffer[7] = 0xff;
	pbaBuffer[8] = 0xff;
	pbaBuffer[9] = 0x00;
	pbaBuffer[10] = 0x03;
	pbaBuffer[11] = 0xb2;
	pbaBuffer[12] = 0x00;
	pbaBuffer[13] = 0x03;

	strcpy(pbaBuffer + 44, pszSystemMessage);

	Commands_Fake_Add(pbaBuffer, nSize);

	Memory_Free(pbaBuffer);
}

void Commands_Say(DWORD dwWhoId, char* pszText, WORD wColor) {
	BYTE* pbaBuffer;
	WORD nSize;

	//OutputDebugString("Commands_Say\n");

	nSize = 44 + strlen(pszText) + 1;

	pbaBuffer = Memory_Allocate(nSize);

	pbaBuffer[0] = 0x1c;
	pbaBuffer[1] = HIBYTE(nSize);
	pbaBuffer[2] = LOBYTE(nSize);
	pbaBuffer[3] = OBJECTID1(dwWhoId);
	pbaBuffer[4] = OBJECTID2(dwWhoId);
	pbaBuffer[5] = OBJECTID3(dwWhoId);
	pbaBuffer[6] = OBJECTID4(dwWhoId);
	pbaBuffer[7] = 0x00;
	pbaBuffer[8] = 0x00;
	pbaBuffer[9] = 0x00;
	pbaBuffer[10] = HIBYTE(wColor);
	pbaBuffer[11] = LOBYTE(wColor);
	pbaBuffer[12] = 0x00;
	pbaBuffer[13] = 0x03;

	strcpy(pbaBuffer + 44, pszText);

	Commands_Fake_Add(pbaBuffer, nSize);

	Memory_Free(pbaBuffer);
}
void Commands_SetLight(BYTE bLevel) {
	BYTE baBuffer[6];

	baBuffer[0] = 0x4f;
	baBuffer[1] = bLevel;

	Commands_Fake_Add(baBuffer, 2);

	/*baBuffer[0] = 0x4e;
	baBuffer[1] = OBJECTID1(UltimaOnline_Data.Character.dwId);
	baBuffer[2] = OBJECTID2(UltimaOnline_Data.Character.dwId);
	baBuffer[3] = OBJECTID3(UltimaOnline_Data.Character.dwId);
	baBuffer[4] = OBJECTID4(UltimaOnline_Data.Character.dwId);
	baBuffer[5] = 0;

	Commands_Fake_Add(baBuffer, 6);*/
}

void Commands_SnowOn(BYTE bLevel) {
	BYTE baBuffer[4];

	baBuffer[0] = 0x65;
	baBuffer[1] = 0x02;
	baBuffer[2] = bLevel;
	baBuffer[3] = 0xec;


	Commands_Fake_Add(baBuffer, 4);
}


void Commands_WalkOkay(BYTE bNumber) {
	BYTE baBuffer[3];

//	OutputDebugString("Commands_WalkOkay\n");

	baBuffer[0] = 0x22;
	baBuffer[1] = bNumber;
	baBuffer[2] = 0x02;

	Commands_Fake_Add(baBuffer, 3);

}

void Commands_ItemEquipped(BYTE bSlot, DWORD dwObjectId, WORD dwObjectType) {
	BYTE baCommand[15] = { 0 };

	OutputDebugString("Commands_ItemEquipped\n");

	baCommand[0] = 0x2e;
	baCommand[1] = OBJECTID1(dwObjectId);
	baCommand[2] = OBJECTID2(dwObjectId);
	baCommand[3] = OBJECTID3(dwObjectId);
	baCommand[4] = OBJECTID4(dwObjectId);
	baCommand[5] = HIBYTE(dwObjectType);
	baCommand[6] = LOBYTE(dwObjectType);
	baCommand[7] = 0x00;
	baCommand[8] = bSlot;
	baCommand[9] = OBJECTID1(UltimaOnline_Data.Character.dwId);
	baCommand[10] = OBJECTID2(UltimaOnline_Data.Character.dwId);
	baCommand[11] = OBJECTID3(UltimaOnline_Data.Character.dwId);
	baCommand[12] = OBJECTID4(UltimaOnline_Data.Character.dwId);
	baCommand[13] = 0x00;
	baCommand[14] = 0x00;

	Commands_Fake_Add(baCommand, 15);

}