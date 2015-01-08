#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "uoehelper.h"
#include "crypt.h"
#include "process.h"
#include "processors.h"
#include "commandqueue.h"
#include "commands.h"
#include "bank.h"

#define MAKEOBJECTX (int)MAKEOBJECTTYPE
#define MAKEOBJECTY (int)MAKEOBJECTTYPE

#define OBJECT_HUMANMALE		0x0190
#define OBJECT_HUMANFEMALE		0x0191
#define OBJECT_CORPSE			0x2006


#define MAKESOUNDID MAKEOBJECTTYPE

#define SOUND_DRUM1			0x0038
#define SOUND_DRUM2			0x0039
#define SOUND_HARP1			0x0043
#define SOUND_HARP2			0x0044
#define SOUND_HARP3			0x0045
#define SOUND_HARP4			0x0046
#define SOUND_LUTE1			0x004c
#define SOUND_LUTE2			0x004d
#define SOUND_TAMBORINE1	0x0052
#define SOUND_TAMBORINE2	0x0053
#define SOUND_SPIRITSPEAK	0x024a

#define OBJECT_ARROW				0x0f3f
#define OBJECT_BOLT					0x1bfb
#define OBJECT_BLACKPEARL			0x0f7a
#define OBJECT_BLOODMOSS			0x0f7b
#define OBJECT_GARLIC				0x0f84
#define OBJECT_GINSENG				0x0f85
#define OBJECT_MANDRAKEROOT			0x0f86
#define OBJECT_NIGHTSHADE			0x0f88
#define OBJECT_SULPHUROUSASH		0x0f8c
#define OBJECT_SPIDERSILK			0x0f8d

#define OBJECT_PACK_TYPE1			0x0e75
#define OBJECT_PACK_TYPE2			0x0e76
#define OBJECT_PACK_TYPE3			0x0e77
#define OBJECT_PACK_TYPE4			0x0e78
#define OBJECT_PACK_TYPE5			0x0e79
#define OBJECT_PACK_TYPE6			0x0e7a
#define OBJECT_PACK_TYPE7			0x0e7c
#define OBJECT_PACK_TYPE8			0x0e7d
#define OBJECT_PACK_TYPE9			0x0e7e
#define OBJECT_PACK_TYPE10			0x0e7f

#define COLOR_HIDINGTEXT 0x013f


typedef struct {
	DWORD dwCharacterId;
	DWORD dwTicks;
} NAMETIME;

LINKLIST* g_pllNameTimes;

typedef struct {
	DWORD dwId;
	DWORD dwTicks;
} HIDDENOBJECT;

LINKLIST* g_pllHiddenObjects;

LINKLIST* g_pllContainers;
CRITICAL_SECTION g_csContainers;
DWORD g_dwNextContainerId;


void Processors_Initialize(void) {
	g_pllHiddenObjects = LinkList_Create();
	g_pllNameTimes = LinkList_Create();
	g_pllContainers = LinkList_Create();
	InitializeCriticalSection(&g_csContainers);

}

BOOL HiddenObjects_Find(DWORD dwId) {
	LINKLISTINFO* plli;
	LINKLISTINFO* plli2;
	HIDDENOBJECT* pho;
	static DWORD dwTicks;

	if (GetTickCount() - dwTicks > 10 * 1000) {
		plli = LinkList_GetHead(g_pllHiddenObjects);
		while (plli != NULL) {
			pho = LinkList_GetObject(g_pllHiddenObjects, plli);
			plli2 = plli;
			plli = LinkList_GetNext(g_pllHiddenObjects, plli);

			if (GetTickCount() - pho->dwTicks > 1 * 60 * 1000) {
				OutputDebugString("removing old hidden object (timeout)\n");

				LinkList_Delete2(g_pllHiddenObjects, plli2);
				Memory_Free(pho);
			} else {
				break;
			}
		}
	}

	dwTicks = GetTickCount();

	plli = LinkList_GetHead(g_pllHiddenObjects);
	while (plli != NULL) {
		pho = LinkList_GetObject(g_pllHiddenObjects, plli);
		plli = LinkList_GetNext(g_pllHiddenObjects, plli);

		if (dwId == pho->dwId) {
			return TRUE;
		}		
	}


	return FALSE;
}

BOOL HiddenObjects_Add(DWORD dwId) {
	HIDDENOBJECT* pho;

	if (!HiddenObjects_Find(dwId)) {
		pho = Memory_Allocate(sizeof(HIDDENOBJECT));
		pho->dwId = dwId;
		pho->dwTicks = GetTickCount();
		LinkList_Add(g_pllHiddenObjects, pho);

		return TRUE;
	}
	
	return FALSE;
}

void HiddenObjects_Remove(DWORD dwId) {
	LINKLISTINFO* plli;
	LINKLISTINFO* plli2;
	HIDDENOBJECT* pho;

	plli = LinkList_GetHead(g_pllHiddenObjects);
	while (plli != NULL) {
		pho = LinkList_GetObject(g_pllHiddenObjects, plli);
		plli2 = plli;
		plli = LinkList_GetNext(g_pllHiddenObjects, plli);

		if (dwId == pho->dwId) {
			OutputDebugString("removing old hidden object\n");

			LinkList_Delete2(g_pllHiddenObjects, plli2);
			Memory_Free(pho);

			break;
		}		
	}

}
//0x03db
BOOL Processors_Incoming_Walk(BYTE* pbaStream) {
	DWORD dwId;

	dwId = MAKEOBJECTID(&pbaStream[1]);

	if (UltimaOnline_Data.Character.dwId != dwId) {
		if (UltimaOnline_Data.Filters.bHiddenPlayers) {

			if (pbaStream[15] & 0x80) {
				if (!UltimaOnline_Data.Filters.bHiddenPlayerMessages) {
					if (HiddenObjects_Add(dwId)) {
						Commands_Say(dwId, "(now hidden)", COLOR_HIDINGTEXT);
					}
				}
/*				pbaStream[5] = 0x03;
				pbaStream[6] = 0xdb;*/
				pbaStream[15] &= 0x7f;
			} else {
				if (!UltimaOnline_Data.Filters.bHiddenPlayerMessages) {
					if (HiddenObjects_Find(dwId)) {
						Commands_Say(dwId, "(revealed)", COLOR_HIDINGTEXT);
						HiddenObjects_Remove(dwId);
					}
				}

			}
		}
	}
	return FALSE;
}

BOOL NameTimes_Find(DWORD dwId) {
	NAMETIME* pnt;
	LINKLISTINFO* plli;
	LINKLISTINFO* plli2;
	static DWORD dwTicks;

	if (GetTickCount() - dwTicks > 10 * 1000) {
		plli = LinkList_GetHead(g_pllNameTimes);
		while (plli != NULL) {
			pnt = LinkList_GetObject(g_pllNameTimes, plli);
			plli2 = plli;
			plli = LinkList_GetNext(g_pllNameTimes, plli);

			if (GetTickCount() - pnt->dwTicks > 30 * 1000) {
				LinkList_Delete2(g_pllNameTimes, plli2);
				Memory_Free(pnt);
			} else {
				break;
			}

		}
	}
	dwTicks = GetTickCount();

	plli = LinkList_GetHead(g_pllNameTimes);
	while (plli != NULL) {
		pnt = LinkList_GetObject(g_pllNameTimes, plli);
		plli = LinkList_GetNext(g_pllNameTimes, plli);

		if (dwId == pnt->dwCharacterId) {
			return TRUE;
		}

	}

	return FALSE;
}

BOOL NameTimes_Add(DWORD dwId) {
	NAMETIME* pnt;

	if (!NameTimes_Find(dwId)) {
		pnt = Memory_Allocate(sizeof(NAMETIME));
		pnt->dwCharacterId = dwId;
		pnt->dwTicks = GetTickCount();
		LinkList_Add(g_pllNameTimes, pnt);

		return TRUE;
	}

	return FALSE;
}

BOOL Processors_Incoming_PutItem(BYTE* pbaStream) {
	DWORD dwId;
	DWORD dwType;
	DWORD dwCorpseType;

	dwId = MAKEOBJECTID(&pbaStream[3]) & 0x7fffffff;
	dwType = MAKEOBJECTTYPE(&pbaStream[7]);

	if (UltimaOnline_Data.Options.bShowNames) {
		if (dwType == OBJECT_CORPSE) {
			dwCorpseType = MAKEOBJECTTYPE(&pbaStream[9]);

			if (!(UltimaOnline_Data.Filters.bNonHumans) || (dwCorpseType == OBJECT_HUMANMALE) || (dwCorpseType == OBJECT_HUMANFEMALE)) {
				if (NameTimes_Add(dwId)) {
					Commands_ShowName(dwId);
				}
			}
		}
	}


	return FALSE;
}


void Processors_Incoming_LoadPlayer(BYTE* pbaStream) {
	DWORD dwId;
	DWORD dwType;

	dwId = MAKEOBJECTID(&pbaStream[3]);
	dwType = MAKEOBJECTTYPE(&pbaStream[7]);

	if (UltimaOnline_Data.Character.dwId == dwId) {

//		OutputDebugString("Checking objects\n");

		UltimaOnline_Data.Character.Items.dwRightHandId = 0;
		UltimaOnline_Data.Character.Items.dwLeftHandId = 0;

		pbaStream += 19;
	

		while (MAKEOBJECTID(pbaStream)) {
			switch (pbaStream[6]) {
			case 0x01:
				OutputDebugString("Got Right Hand\n");
				UltimaOnline_Data.Character.Items.dwRightHandId = MAKEOBJECTID(pbaStream);
				break;
			case 0x02:
				OutputDebugString("Got Left Hand\n");
				UltimaOnline_Data.Character.Items.dwLeftHandId = MAKEOBJECTID(pbaStream);
				break;
			case 0x15:
				OutputDebugString("Got backpack!\n");
				UltimaOnline_Data.Character.Items.dwBackpackId = MAKEOBJECTID(pbaStream);
				break;

			}
			if (pbaStream[4] & 0x80) pbaStream+= 2;
			pbaStream += 7;
		}

	} else {

		if (UltimaOnline_Data.Filters.bHiddenPlayers) {

			if (pbaStream[17] & 0x80) {
				if (!UltimaOnline_Data.Filters.bHiddenPlayerMessages) {
					if (HiddenObjects_Add(dwId)) {
						Commands_Say(dwId, "(hidden)", COLOR_HIDINGTEXT);
					}
				}
/*				pbaStream[7] = 0x03;
				pbaStream[8] = 0xdb;*/

				pbaStream[17] &= 0x7f;
			} else {
				if (!UltimaOnline_Data.Filters.bHiddenPlayerMessages) {
					if (HiddenObjects_Find(dwId)) {
						Commands_Say(dwId, "(revealed)", COLOR_HIDINGTEXT);
						HiddenObjects_Remove(dwId);
					}
				}
			}
		}



		if (UltimaOnline_Data.Options.bShowNames) {
			if (!(UltimaOnline_Data.Filters.bNonHumans) || (dwType == OBJECT_HUMANMALE) || (dwType == OBJECT_HUMANFEMALE)) {
				if (NameTimes_Add(dwId)) {
					Commands_ShowName(dwId);				
				}
			}
		}
	}


}

BOOL Processors_Incoming_Speech(BYTE* pbaStream) {
	BOOL bFilter = FALSE;
	HANDLE hevent;
	char* pszText;
	char* p;
	int nSize;
	char szText[1024];

	pszText = &pbaStream[44];

	if (g_bWantObjectName) {
		if (g_GetObject_dwObjectId == MAKEOBJECTID(&pbaStream[3])) {
			g_bWantObjectName = FALSE;
			g_GetObject_wObjectType = MAKEOBJECTTYPE(&pbaStream[7]);
			strcpy(g_GetObject_szObjectName, pszText);
			hevent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "g_heventGotObjectName");
			SetEvent(hevent);
			CloseHandle(hevent);
		}
		bFilter = TRUE;
	} else {

		EnterCriticalSection(&CommandQueue_Data.WaitForThis.cs);

		if (CommandQueue_Data.WaitForThis.bEnabled) {
			if (strcmp(pszText, "You must wait to perform another action.") == 0) {
				CommandQueue_Data.WaitForThis.bEnabled = FALSE;
				CommandQueue_Data.WaitForThis.pbaBuffer = NULL;
				CommandQueue_Data.WaitForThis.wSize = 0;

				SetEvent(CommandQueue_Data.WaitForThis.hevents[CQWFTE_FAILURE]);

				bFilter = TRUE;
			}
		}

		LeaveCriticalSection(&CommandQueue_Data.WaitForThis.cs);




		if (UltimaOnline_Data.Filters.bGuildNames) {
			nSize = strlen(pszText);
			if (nSize > 3) {
				if ((pszText[0] == '[') && (pszText[nSize - 1] == ' ') && (pszText[nSize - 2] == ']')) {
					if (!UltimaOnline_Data.Filters.bGuildTitlesOnly) {		
						bFilter = TRUE;
					} else {
						p = strrchr(pszText, ',');
						if (p && *(p + 1) == ' ') {
							wsprintf(szText, "[%s", p + 2);
							strcpy(pszText, szText);
						}
					}
				}
			}
		}
	}


	return bFilter;
}


BOOL Processors_Incoming_PlaySound(BYTE* pbaStream) {
	BOOL bFilter = FALSE;

	switch (MAKESOUNDID(&pbaStream[2])) {
	case SOUND_DRUM1:
	case SOUND_DRUM2:
	case SOUND_HARP1:
	case SOUND_HARP2:
	case SOUND_HARP3:
	case SOUND_HARP4:
	case SOUND_LUTE1:
	case SOUND_LUTE2:
	case SOUND_TAMBORINE1:
	case SOUND_TAMBORINE2:
		bFilter = UltimaOnline_Data.Filters.bBardMusic;
		break;
	case SOUND_SPIRITSPEAK:
		bFilter = UltimaOnline_Data.Filters.bSpiritSpeakSound;
		break;
	}

	return bFilter;

}

void Processors_Incoming_WearItem(BYTE* pbaStream) {
	if (MAKEOBJECTID(&pbaStream[9]) == UltimaOnline_Data.Character.dwId) {
		switch (pbaStream[8]) {
		case 0x01:
			OutputDebugString("Got Right Hand\n");
			UltimaOnline_Data.Character.Items.dwRightHandId = MAKEOBJECTID(&pbaStream[1]);
			break;
		case 0x02:
			OutputDebugString("Got Left Hand\n");
			UltimaOnline_Data.Character.Items.dwLeftHandId = MAKEOBJECTID(&pbaStream[1]);
			break;
		case 0x1d:
			OutputDebugString("Got Bank Box\n");
			Bank_SetId(MAKEOBJECTID(&pbaStream[1]));
			break;
		}
	}

}

BYTE g_bContainerDump[7];

BOOL Processors_Incoming_ContainerInfo(BYTE* pbaStream) {

	g_dwNextContainerId = MAKEOBJECTID(&pbaStream[1]);

	memcpy(g_bContainerDump, pbaStream, 7);	

	return g_bCountObjects || UltimaOnline_Data.Options.bPreventAutoClose;
}



BOOL Processors_Incoming_ItemsInContainer(BYTE* pbaStream) {
	LINKLISTINFO* plli;
	UINT nNumberOfItems, nPileSize;
	UINT i;
	BOOL bFilter = FALSE;
	DWORD dwContainerId;
	BYTE* p;

	if (g_bCountObjects) {

		bFilter = TRUE;


		EnterCriticalSection(&g_csContainers);
		plli = LinkList_Find(g_pllContainers, (PVOID)g_dwNextContainerId);
		LeaveCriticalSection(&g_csContainers);

		if (plli) {

			EnterCriticalSection(&g_csContainers);
			LinkList_Delete2(g_pllContainers, (PVOID)plli);
			LeaveCriticalSection(&g_csContainers);
			

			nNumberOfItems = MAKEWORD(pbaStream[4],  pbaStream[3]);
			pbaStream += 5;

			for (i = 0; i < nNumberOfItems; i++) {

				nPileSize = MAKEWORD(pbaStream[8], pbaStream[7]);

				switch (MAKEOBJECTTYPE(&pbaStream[4])) {
				case OBJECT_PACK_TYPE1:
				case OBJECT_PACK_TYPE2:
				case OBJECT_PACK_TYPE3:
				case OBJECT_PACK_TYPE4:
				case OBJECT_PACK_TYPE5:
				case OBJECT_PACK_TYPE6:
				case OBJECT_PACK_TYPE7:
				case OBJECT_PACK_TYPE8:
				case OBJECT_PACK_TYPE9:
				case OBJECT_PACK_TYPE10:

					dwContainerId = MAKEOBJECTID(&pbaStream[0]);

					Commands_OpenContainer(dwContainerId);

					EnterCriticalSection(&g_csContainers);
					LinkList_Add(g_pllContainers, (PVOID)dwContainerId);
					LeaveCriticalSection(&g_csContainers);

					break;


				case OBJECT_ARROW:
					g_nNumberOfArrows += nPileSize;
					break;
				case OBJECT_BOLT:
					g_nNumberOfBolts += nPileSize;
					break;
				case OBJECT_NIGHTSHADE:
					g_nNumberOfNightshade += nPileSize;
					break;
				case OBJECT_BLACKPEARL:
					g_nNumberOfBlackPearl += nPileSize;
					break;
				case OBJECT_MANDRAKEROOT:
					g_nNumberOfMandrakeRoot += nPileSize;
					break;
				case OBJECT_GARLIC:
					g_nNumberOfGarlic += nPileSize;
					break;
				case OBJECT_SULPHUROUSASH:
					g_nNumberOfSulphurousAsh += nPileSize;
					break;
				case OBJECT_GINSENG:
					g_nNumberOfGinseng += nPileSize;
					break;
				case OBJECT_BLOODMOSS:
					g_nNumberOfBloodMoss += nPileSize;
					break;
				case OBJECT_SPIDERSILK:
					g_nNumberOfSpiderSilk += nPileSize;
					break;
				}

				pbaStream += 19;
			}

//			g_nNumberOfPacksWaiting--;
			EnterCriticalSection(&g_csContainers);
			plli = LinkList_GetHead(g_pllContainers);
			LeaveCriticalSection(&g_csContainers);

			if (!plli) {
				OutputDebugString("Count done!\n");
				SetEvent(g_heventCountDone);
			}
		}

	} else {

		if (UltimaOnline_Data.Options.bPreventAutoClose && g_dwNextContainerId) {

			bFilter = TRUE;

			dwContainerId = MAKEFAKEID(MAKEOBJECTID(&g_bContainerDump[1]));
			UltimaOnline_Data.FakeContainers.dwIds[UltimaOnline_Data.FakeContainers.nPosition++] = dwContainerId;

			if (UltimaOnline_Data.FakeContainers.nPosition == MAX_FAKECONTAINERS) UltimaOnline_Data.FakeContainers.nPosition = MAX_FAKECONTAINERS / 4;
			
			g_bContainerDump[1] = OBJECTID1(dwContainerId); 
			g_bContainerDump[2] = OBJECTID2(dwContainerId); 
			g_bContainerDump[3] = OBJECTID3(dwContainerId); 
			g_bContainerDump[4] = OBJECTID4(dwContainerId); 


			nNumberOfItems = MAKEWORD(pbaStream[4],  pbaStream[3]);


			p = pbaStream + 5;

			for (i = 0; i < nNumberOfItems; i++) {
				p[13] = OBJECTID1(dwContainerId); 
				p[14] = OBJECTID2(dwContainerId); 
				p[15] = OBJECTID3(dwContainerId); 
				p[16] = OBJECTID4(dwContainerId); 


				p += 19;
			}



			Commands_ItemEquipped(0x15, MAKEOBJECTID(&g_bContainerDump[1]), OBJECT_PACK_TYPE1);
			Commands_Fake_Add(g_bContainerDump, 7);
			Commands_Fake_Add(pbaStream, MAKEWORD(pbaStream[2], pbaStream[1]));
			Commands_ItemEquipped(0x15, UltimaOnline_Data.Character.Items.dwBackpackId, OBJECT_PACK_TYPE1);

		}
	}

	g_dwNextContainerId = 0;

	return bFilter;
}

BOOL Processors_Incoming_MoveItem(BYTE* pbaStream) {
	DWORD dwObjectId;
	int i;

	if (UltimaOnline_Data.Options.bPreventAutoClose) {
		dwObjectId = MAKEFAKEID(MAKEOBJECTID(&pbaStream[14]));

		for (i = 0; i < MAX_FAKECONTAINERS; i++) {
			if (UltimaOnline_Data.FakeContainers.dwIds[i] == dwObjectId) {

				pbaStream[14] = OBJECTID1(dwObjectId);
				pbaStream[15] = OBJECTID2(dwObjectId);
				pbaStream[16] = OBJECTID3(dwObjectId);
				pbaStream[17] = OBJECTID4(dwObjectId);
				break;
			}
		}
	}

	return FALSE;
}