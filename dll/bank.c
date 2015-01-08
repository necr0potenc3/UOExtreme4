#include <windows.h>
#include <stdio.h>
#include "uoehelper.h"
#include "commands.h"
#include "bank.h"

struct {
	DWORD dwId;

} Bank_Data;


void Bank_Open(void) {
	if (!Bank_Data.dwId) {
		Commands_SystemMessage("You must use the bank at least once to use this feature.");
		return;
	}

	Commands_OpenBank(Bank_Data.dwId);
}

DWORD Bank_GetId(void) {
	return Bank_Data.dwId;
}


void Bank_SetId(DWORD dwId) {
	Bank_Data.dwId = dwId;
	Bank_Save();
}

void Bank_Save(void) {
	FILE* fp;
	char szFileName[256];

	wsprintf(szFileName, "%s.bnk", UltimaOnline_Data.Character.szName);

	fp = fopen(szFileName, "wt");
	fprintf(fp, "%08x\n", Bank_Data.dwId);
	fclose(fp);

}

void Bank_Load(void) {
	FILE* fp;
	char szFileName[256];

	Bank_Data.dwId = 0;

	wsprintf(szFileName, "%s.bnk", UltimaOnline_Data.Character.szName);

	fp = fopen(szFileName, "rt");
	if (!fp) return;

	fscanf(fp, "%08x\n", &Bank_Data.dwId);

	fclose(fp);

}
