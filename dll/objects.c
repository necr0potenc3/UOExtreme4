#include <windows.h>
#include <stdio.h>
#include "uoehelper.h"

#define OBJECT_FILENAME "objects.txt"

LINKLIST* g_pllObjects;
LINKLIST* g_pllObjectDefinitions;





OBJECT* Object_Create() {
	OBJECT* pobj;

	pobj = Memory_Allocate(sizeof(OBJECT));

	return pobj;
}

void Object_Destroy(OBJECT* pobj) {
	Memory_Free(pobj);
}


void Objects_Initialize(void) {
	if (!g_pllObjects) {
		g_pllObjects = LinkList_Create();
	}
	if (!g_pllObjectDefinitions) {
		g_pllObjectDefinitions = LinkList_Create();
	}
}

void Objects_Add(OBJECT* pobj) {
	LinkList_Add(g_pllObjects, pobj);
}

void Objects_Delete(OBJECT* pobj) {
	LinkList_Delete(g_pllObjects, pobj);
}

OBJECT* Objects_GetFirst(void) {
	return LinkList_GetObject(g_pllObjects, LinkList_GetHead(g_pllObjects));
}

OBJECT* Objects_GetNext(OBJECT* pobj) {
	return LinkList_GetObject(g_pllObjects, LinkList_GetNext(g_pllObjects, LinkList_Find(g_pllObjects, pobj)));
}


OBJECT* Objects_Find(char* pszUniqueName) {
	OBJECT* pobj;


	pobj = Objects_GetFirst();
	while (pobj != NULL) {
		if (strcmp(pszUniqueName, pobj->szUniqueName) == 0) {
			return pobj;
		}
		pobj = Objects_GetNext(pobj);
	}

	return NULL;
}

void Objects_RemoveAll(void) {
	OBJECT* pobj;


	pobj = Objects_GetFirst();
	while (pobj != NULL) {
		Objects_Delete(pobj);
		Object_Destroy(pobj);

		pobj = Objects_GetFirst();
	}

}


void Objects_Load(void) {
	OBJECT* pobj;
	FILE* fp;
	char szTemp[1024];

	Objects_RemoveAll();

	fp = fopen(OBJECT_FILENAME, "rt");
	if (!fp) return;

	for (;;) {
		
		if (!fgets(szTemp, sizeof(szTemp), fp)) break;

		szTemp[strlen(szTemp) - 1] = 0;

		pobj = Object_Create();

		strcpy(pobj->szUniqueName, szTemp);

		Objects_Add(pobj);

	}
	fclose(fp);

}

void Objects_Save(void) {
	OBJECT* pobj;
	FILE* fp;


	fp = fopen(OBJECT_FILENAME, "wt");

	pobj = Objects_GetFirst();

	while (pobj != NULL) {
		fprintf(fp, "%s\n", pobj->szUniqueName);

		pobj = Objects_GetNext(pobj);
	}

	fclose(fp);

}

OBJECTDEFINITION* ObjectDefinition_Create() {
	OBJECTDEFINITION* pobjdef;

	pobjdef = Memory_Allocate(sizeof(OBJECTDEFINITION));

	return pobjdef;
}

void ObjectDefinitions_Initialize(void) {
	if (!g_pllObjectDefinitions) {
		g_pllObjectDefinitions = LinkList_Create();
	}
}

void ObjectDefinition_Destroy(OBJECTDEFINITION* pobjdef) {
	Memory_Free(pobjdef);
}

void ObjectDefinitions_Add(OBJECTDEFINITION* pobjdef) {
	LinkList_Add(g_pllObjectDefinitions, pobjdef);
}

void ObjectDefinitions_Delete(OBJECTDEFINITION* pobjdef) {
	LinkList_Delete(g_pllObjectDefinitions, pobjdef);
}

OBJECTDEFINITION* ObjectDefinitions_GetFirst(void) {
	return LinkList_GetObject(g_pllObjectDefinitions, LinkList_GetHead(g_pllObjectDefinitions));
}

OBJECTDEFINITION* ObjectDefinitions_GetNext(OBJECTDEFINITION* pobjdef) {
	return LinkList_GetObject(g_pllObjectDefinitions, LinkList_GetNext(g_pllObjectDefinitions, LinkList_Find(g_pllObjectDefinitions, pobjdef)));
}


OBJECTDEFINITION* ObjectDefinitions_Find(char* pszUniqueName) {
	OBJECTDEFINITION* pobjdef;


	pobjdef = ObjectDefinitions_GetFirst();
	while (pobjdef != NULL) {
		if (strcmp(pszUniqueName, pobjdef->szUniqueName) == 0) {
			return pobjdef;
		}
		pobjdef = ObjectDefinitions_GetNext(pobjdef);
	}

	return NULL;
}

void ObjectDefinitions_RemoveAll(void) {
	OBJECTDEFINITION* pobjdef;

	pobjdef = ObjectDefinitions_GetFirst();
	while (pobjdef != NULL) {
		ObjectDefinitions_Delete(pobjdef);
		ObjectDefinition_Destroy(pobjdef);

		pobjdef = ObjectDefinitions_GetFirst();
	}

}


void ObjectDefinitions_Load(void) {
	OBJECTDEFINITION* pobjdef;
	FILE* fp;
	char szTemp[1024];
	char szFileName[256];

	wsprintf(szFileName, "%s.obj", UltimaOnline_Data.Character.szName);

	ObjectDefinitions_RemoveAll();

	fp = fopen(szFileName, "rt");
	if (!fp) return;

	for (;;) {
		
		if (!fgets(szTemp, sizeof(szTemp), fp)) break;

		pobjdef = ObjectDefinition_Create();

		strcpy(pobjdef->szUniqueName, szTemp);

		pobjdef->szUniqueName[strlen(pobjdef->szUniqueName) - 1] = '\x00';
		fgets(pobjdef->szName, sizeof(pobjdef->szName), fp);
		pobjdef->szName[strlen(pobjdef->szName) - 1] = '\x00';
		fscanf(fp, "%08x\n", &pobjdef->dwId);
		fscanf(fp, "%04x\n", &pobjdef->wType);
		fscanf(fp, "%d\n", &pobjdef->bUseAny);
		fscanf(fp, "%d\n", &pobjdef->bCheckNested);

		ObjectDefinitions_Add(pobjdef);

	}
	fclose(fp);

}

void ObjectDefinitions_Save(void) {
	OBJECTDEFINITION* pobjdef;
	FILE* fp;
	char szFileName[256];

	wsprintf(szFileName, "%s.obj", UltimaOnline_Data.Character.szName);

	fp = fopen(szFileName, "wt");

	pobjdef = ObjectDefinitions_GetFirst();

	while (pobjdef != NULL) {
		fprintf(fp, "%s\n", pobjdef->szUniqueName);
		fprintf(fp, "%s\n", pobjdef->szName);
		fprintf(fp, "%08x\n", pobjdef->dwId);
		fprintf(fp, "%04x\n", pobjdef->wType);
		fprintf(fp, "%d\n", pobjdef->bUseAny);
		fprintf(fp, "%d\n", pobjdef->bCheckNested);

		pobjdef = ObjectDefinitions_GetNext(pobjdef);
	}

	fclose(fp);

}
