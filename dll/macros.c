#include <windows.h>
#include <stdio.h>
#include "uoehelper.h"

#define MACRO_FILENAME "macros2.txt"

char* g_pszaMacroCommandTypeNames[MAX_MACRO_COMMAND_TYPES] = {
	"Equip Left Hand",
	"Equip Right Hand",
	"Unequip Left Hand",
	"Unequip Right Hand",
	"Target Object",
	"Last Target",
	"Target Self",
	"Archery Count",
	"Reagent Count",
	"Cast Spell",
	"Wait for Target Cursor",
	"Open Bank Box"
};

char* g_pszaSpellNames[MAX_SPELLS] = {
	"Clumsy",
	"Create Food",
	"Feeblemind",
	"Heal",
	"Magic Arrow",
	"Night Sight",
	"Reactive Armor",
	"Weaken",
	"Agility",
	"Cunning",
	"Cure",
	"Harm",
	"Magic Trap",
	"Protection",
	"Remove Trap",
	"Strength",
	"Bless",
	"Fireball",
	"Magic Lock",
	"Poison",
	"Telekinesis",
	"Teleport",
	"Unlock",
	"Wall of Stone",
	"Archcure",
	"Archprotection",
	"Curse",
	"Fire Field",
	"Greater Heal",
	"Lightning",
	"Mana Drain",
	"Recall",
	"Blade Spirits",
	"Dispel Field",
	"Incognito",
	"Magic Reflection",
	"Mind Blast",
	"Paralyze",
	"Poison Field",
	"Summon Creature",
	"Dispel",
	"Energy Bolt",
	"Explosion",
	"Invisibility",
	"Mark",
	"Mass Curse",
	"Paralyze Field",
	"Reveal",
	"Chain Lightning",
	"Energy Field",
	"Flamestrike",
	"Gate Travel",
	"Mana Vampire",
	"Mass Dispel",
	"Meteor Swarm",
	"Polymorph",
	"Earthquake",
	"Energy Vortex",
	"Resurrection",
	"Summon Air Elem.",
	"Summon Daemon",
	"Summon Earth Elem.",
	"Summon Fire Elem.",
	"Summon Water Elem."
};

LINKLIST* g_pllMacros;

MACRO* Macro_Create(void) {
	MACRO* pmacro;

	pmacro = Memory_Allocate(sizeof(MACRO));
	pmacro->pll = LinkList_Create();

	return pmacro;
}

void Macro_Destroy(MACRO* pmacro) {
	Macro_Commands_RemoveAll(pmacro);
	Memory_Free(pmacro);
}


MACROCOMMAND* Macro_Commands_Create(void) {
	MACROCOMMAND* pmc;

	pmc = Memory_Allocate(sizeof(MACROCOMMAND));
	pmc->iType = -1;

	return pmc;
}

void Macro_Commands_Destroy(MACROCOMMAND* pmc) {

	Memory_Free(pmc);

}

void Macro_Commands_Delete(MACRO* pmacro, MACROCOMMAND* pmc) {
	LinkList_Delete(pmacro->pll, pmc);
}

MACROCOMMAND* Macro_Commands_GetFirst(MACRO* pmacro) {
	return LinkList_GetObject(pmacro->pll, LinkList_GetHead(pmacro->pll));
}

MACROCOMMAND* Macro_Commands_GetNext(MACRO* pmacro, MACROCOMMAND* pmc) {
	return LinkList_GetObject(pmacro->pll, LinkList_GetNext(pmacro->pll, LinkList_Find(pmacro->pll, pmc)));
}

void Macro_Commands_RemoveAll(MACRO* pmacro) {
	MACROCOMMAND* pmc;


	pmc = Macro_Commands_GetFirst(pmacro);
	while (pmc != NULL) {
		Macro_Commands_Delete(pmacro, pmc);
		Memory_Free(pmc);

		pmc = Macro_Commands_GetFirst(pmacro);
	}

}

void Macro_Commands_Add(MACRO* pmacro, MACROCOMMAND* pmc) {
	LinkList_Add(pmacro->pll, pmc);
}

void Macros_Initialize(void) {
	if (!g_pllMacros) {
		g_pllMacros = LinkList_Create();
	}
}

void Macros_Add(MACRO* pmacro) {
	LinkList_Add(g_pllMacros, pmacro);
}

MACRO* Macros_GetFirst(void) {
	return LinkList_GetObject(g_pllMacros, LinkList_GetHead(g_pllMacros));
}

MACRO* Macros_GetNext(MACRO* pmacro) {
	return LinkList_GetObject(g_pllMacros, LinkList_GetNext(g_pllMacros, LinkList_Find(g_pllMacros, pmacro)));
}


void Macros_Delete(MACRO* pmacro) {
	Macro_Commands_RemoveAll(pmacro);
	LinkList_Delete(g_pllMacros, pmacro);
}

void Macros_RemoveAll(void) {
	MACRO* pmacro;

	pmacro = Macros_GetFirst();
	while (pmacro != NULL) {
		Macros_Delete(pmacro);
		Memory_Free(pmacro);

		pmacro = Macros_GetFirst();
	}
}

void Macros_Save(void) {
	MACRO* pmacro;
	MACROCOMMAND* pmc;
	FILE* fp;

	fp = fopen(MACRO_FILENAME, "wt");

	pmacro = Macros_GetFirst();

	while (pmacro != NULL) {
		fprintf(fp, "%s\n", pmacro->szDescription);
		fprintf(fp, "%s\n", pmacro->szKeyName);
		fprintf(fp, "%d,%d,%d\n", pmacro->bCtrl, pmacro->bAlt, pmacro->bShift);

		pmc = Macro_Commands_GetFirst(pmacro);
		while (pmc != NULL) {
			fprintf(fp,"%d\n", pmc->iType);
			switch (pmc->iType) {
			case MCT_EQUIP_LEFT:
			case MCT_EQUIP_RIGHT:
			case MCT_TARGETOBJECT:
				fprintf(fp, "%s\n", pmc->u.szObjectUniqueName);
				break;

			case MCT_CASTSPELL:
				fprintf(fp, "%d\n", pmc->u.bSpellNumber);
				break;
			default:
				break;
			}

			pmc = Macro_Commands_GetNext(pmacro, pmc);
		}

		fprintf(fp, "\n");

		pmacro = Macros_GetNext(pmacro);
	}

	fclose(fp);

}

void Macros_Load(void) {

	MACRO* pmacro;
	MACROCOMMAND* pmc;
	FILE* fp;
	char szText[1024];

	Macros_RemoveAll();

	fp = fopen(MACRO_FILENAME, "rt");
	if (!fp) return;

	for (;;) {
		if (!fgets(szText, sizeof(szText), fp)) break;

		pmacro = Macro_Create();

		strcpy(pmacro->szDescription, szText);
		pmacro->szDescription[strlen(pmacro->szDescription) - 1] = '\x00';

		fgets(pmacro->szKeyName, sizeof(pmacro->szKeyName), fp);
		pmacro->szKeyName[strlen(pmacro->szKeyName) - 1] = '\x00';

		fscanf(fp, "%d,%d,%d\n", &pmacro->bCtrl, &pmacro->bAlt, &pmacro->bShift);

		for (;;) {

			if (!fgets(szText, sizeof(szText), fp)) break;
			if (strlen(szText) == 1) break;

			pmc = Macro_Commands_Create();

			pmc->iType = atoi(szText);

			switch (pmc->iType) {
			case MCT_EQUIP_LEFT:
			case MCT_EQUIP_RIGHT:
			case MCT_TARGETOBJECT:
				fgets(pmc->u.szObjectUniqueName, sizeof(pmc->u.szObjectUniqueName), fp);
				pmc->u.szObjectUniqueName[strlen(pmc->u.szObjectUniqueName) - 1] = '\x00';
				break;

			case MCT_CASTSPELL:
				fscanf(fp, "%d\n", &pmc->u.bSpellNumber);

				break;
			default:
				break;
			}

			Macro_Commands_Add(pmacro, pmc);
		}
		Macros_Add(pmacro);

	}

	fclose(fp);


}
