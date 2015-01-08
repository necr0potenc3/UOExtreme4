#include <windows.h>
#include <stdio.h>
#include "uoehelper.h"
#include "crypt.h"
#include "process.h"
#include "processors.h"
#include "processmacros.h"
#include "commandqueue.h"
#include "commands.h"
#include "bank.h"

#define ALONGTIME (2 * 60 * 1000)

DWORD threadProcessMacro(MACRO* pmacro) {
	MACROCOMMAND* pmc;
	OBJECTDEFINITION* pobjdef;
	DWORD dwTempId;
	char szText[1024];

	OutputDebugString("THREAD STARTED\n");

	pmc = Macro_Commands_GetFirst(pmacro);

	while (pmc != NULL) {
		switch (pmc->iType) {
		case MCT_EQUIP_LEFT:
			pobjdef = ObjectDefinitions_Find(pmc->u.szObjectUniqueName);
			if (pobjdef) {
				OutputDebugString("MCT_EQUIP_LEFT\n");

				Commands_GetObject(pobjdef->dwId, COMMAND_FLAG_LEFTEMPTY);
				Commands_EquipObject(2, pobjdef->dwId, COMMAND_FLAG_LEFTEMPTY);

			}
			break;
		case MCT_EQUIP_RIGHT:
			pobjdef = ObjectDefinitions_Find(pmc->u.szObjectUniqueName);
			if (pobjdef) {
				OutputDebugString("MCT_EQUIP_RIGHT\n");

				Commands_GetObject(pobjdef->dwId, COMMAND_FLAG_RIGHTEMPTY);
				Commands_EquipObject(1, pobjdef->dwId, COMMAND_FLAG_RIGHTEMPTY);
			}
			break;

		case MCT_UNEQUIP_LEFT:
			OutputDebugString("MCT_UNEQUIP_LEFT\n");

			if (UltimaOnline_Data.Character.Items.dwLeftHandId) {

				dwTempId = UltimaOnline_Data.Character.Items.dwLeftHandId;

				Commands_GetObject(dwTempId, 0);
				Commands_PutObject(dwTempId, UltimaOnline_Data.Character.dwId, 0);

			}
			
			break;

		case MCT_UNEQUIP_RIGHT:
			OutputDebugString("MCT_UNEQUIP_RIGHT\n");

			if (UltimaOnline_Data.Character.Items.dwRightHandId) {

				dwTempId = UltimaOnline_Data.Character.Items.dwRightHandId;

				Commands_GetObject(dwTempId, 0);
				Commands_PutObject(dwTempId, UltimaOnline_Data.Character.dwId, 0);

			}


			break;
		case MCT_TARGETOBJECT:

			pobjdef = ObjectDefinitions_Find(pmc->u.szObjectUniqueName);
			if (pobjdef) {
				OutputDebugString("MCT_TARGETOBJECT\n");

				UltimaOnline_Data.Filters.bTargetCursor = TRUE;
				if (WaitForSingleObject(g_heventTargetCursor, 5000) == WAIT_OBJECT_0) {

					Commands_Target(pobjdef->dwId);

				}
				UltimaOnline_Data.Filters.bTargetCursor = FALSE;

			}


			break;

		case MCT_CASTSPELL:
			OutputDebugString("MCT_CASTSPELL\n");
			Commands_CastSpell(pmc->u.bSpellNumber);
			break;

		case MCT_LASTTARGET:
			OutputDebugString("MCT_LASTTARGET\n");

			UltimaOnline_Data.Filters.bTargetCursor = TRUE;
			if (WaitForSingleObject(g_heventTargetCursor, 5000) == WAIT_OBJECT_0) {

				Commands_Target(UltimaOnline_Data.History.dwLastTargetId);
			}
			UltimaOnline_Data.Filters.bTargetCursor = FALSE;

			break;
		case MCT_TARGETSELF:
			OutputDebugString("MCT_TARGETSELF\n");

			UltimaOnline_Data.Filters.bTargetCursor = TRUE;
			if (WaitForSingleObject(g_heventTargetCursor, 5000) == WAIT_OBJECT_0) {

				Commands_Target(UltimaOnline_Data.Character.dwId);
			}
			UltimaOnline_Data.Filters.bTargetCursor = FALSE;

			break;
		case MCT_ARCHERYCOUNT:
			OutputDebugString("MCT_ARCHERYCOUNT\n");

//			g_nNumberOfPacksWaiting = 1;
			EnterCriticalSection(&g_csContainers);
			LinkList_RemoveAll(g_pllContainers);
			LinkList_Add(g_pllContainers, (PVOID)UltimaOnline_Data.Character.Items.dwBackpackId);
			LeaveCriticalSection(&g_csContainers);

			g_nNumberOfArrows = 0;
			g_nNumberOfBolts = 0;
			g_bCountObjects = TRUE;
			ResetEvent(g_heventCountDone);

			Commands_SystemMessage("Beginning to count arrows and bolts...");
			Commands_OpenContainer(UltimaOnline_Data.Character.Items.dwBackpackId);

			WaitForSingleObject(g_heventCountDone, ALONGTIME);
			g_bCountObjects = FALSE;

			wsprintf(szText, "You have %d|Arrows and %d|Bolts.", g_nNumberOfArrows, g_nNumberOfBolts);

			Commands_SystemMessage(szText);

			wsprintf(szText, "Found %d Arrows\n", g_nNumberOfArrows);
			OutputDebugString(szText);
			wsprintf(szText, "Found %d Bolts\n", g_nNumberOfBolts);
			OutputDebugString(szText);

			break;
		case MCT_REAGENTCOUNT:
			OutputDebugString("MCT_REAGENTCOUNT\n");
			EnterCriticalSection(&g_csContainers);
			LinkList_RemoveAll(g_pllContainers);
			LinkList_Add(g_pllContainers, (PVOID)UltimaOnline_Data.Character.Items.dwBackpackId);
			LeaveCriticalSection(&g_csContainers);
			//g_nNumberOfPacksWaiting = 1;
			g_nNumberOfSulphurousAsh = 0;
			g_nNumberOfBloodMoss = 0;
			g_nNumberOfMandrakeRoot = 0;
			g_nNumberOfSpiderSilk = 0;
			g_nNumberOfGarlic = 0;
			g_nNumberOfGinseng = 0;
			g_nNumberOfNightshade = 0;
			g_nNumberOfBlackPearl = 0;
			g_bCountObjects = TRUE;
			ResetEvent(g_heventCountDone);

			Commands_SystemMessage("Beginning to count reagents...");
			Commands_OpenContainer(UltimaOnline_Data.Character.Items.dwBackpackId);
			WaitForSingleObject(g_heventCountDone, ALONGTIME);
			g_bCountObjects = FALSE;

			wsprintf(szText, "You have %d|Garlic, %d|Ginseng, "
			"%d|Mandrake|Root, %d|Spider's|Silk, "
			"%d|Blood|Moss, %d|Black|Pearl, "
			"%d|Nighshade and %d|Sulphurous|Ash.", 			
			g_nNumberOfGarlic, g_nNumberOfGinseng,	
			g_nNumberOfMandrakeRoot, g_nNumberOfSpiderSilk,
			g_nNumberOfBloodMoss, g_nNumberOfBlackPearl,
			g_nNumberOfNightshade, g_nNumberOfSulphurousAsh);

			Commands_SystemMessage(szText);

			wsprintf(szText, "Found %d Nightshade\n", g_nNumberOfNightshade);
			OutputDebugString(szText);
			wsprintf(szText, "Found %d Black Pearl\n", g_nNumberOfBlackPearl);
			OutputDebugString(szText);
			wsprintf(szText, "Found %d Mandrake Root\n", g_nNumberOfMandrakeRoot);
			OutputDebugString(szText);
			wsprintf(szText, "Found %d Garlic\n", g_nNumberOfGarlic);
			OutputDebugString(szText);
			wsprintf(szText, "Found %d Sulphurous Ash\n", g_nNumberOfSulphurousAsh);
			OutputDebugString(szText);
			wsprintf(szText, "Found %d Ginseng\n", g_nNumberOfGinseng);
			OutputDebugString(szText);
			wsprintf(szText, "Found %d Blood Moss\n", g_nNumberOfBloodMoss);
			OutputDebugString(szText);
			wsprintf(szText, "Found %d Spider's Silk\n", g_nNumberOfSpiderSilk);
			OutputDebugString(szText);


			break;
		case MCT_WAITFORTARGETCURSOR:
			OutputDebugString("MCT_WAITFORTARGETCURSOR\n");
			UltimaOnline_Data.Filters.bTargetCursor = TRUE;
			WaitForSingleObject(g_heventTargetCursor, ALONGTIME);
			UltimaOnline_Data.Filters.bTargetCursor = FALSE;
			break;
		case MCT_OPENBANK:
			Bank_Open();
			break;

		}

		pmc = Macro_Commands_GetNext(pmacro, pmc);
	}

	OutputDebugString("THREAD EXIT\n");

	return 0;
}