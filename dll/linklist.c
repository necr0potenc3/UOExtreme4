#include <windows.h>
#include "uoehelper.h"


LINKLIST* LinkList_Create() {
	LINKLIST* pll;

	pll = Memory_Allocate(sizeof(LINKLIST));
	pll->plliHead = NULL;
	pll->plliTail = NULL;

	return pll;
}

void LinkList_Destroy(LINKLIST* pll) {
	while (pll->plliHead) LinkList_Delete2(pll, pll->plliHead);

	Memory_Free(pll);
}

void LinkList_RemoveAll(LINKLIST* pll) {
	while (pll->plliHead) LinkList_Delete2(pll, pll->plliHead);
}

LINKLISTINFO* LinkList_Find(LINKLIST* pll, PVOID pObject) {
	LINKLISTINFO* plli;

	plli = LinkList_GetHead(pll);

	while (plli != NULL) {
		if (plli->pObject == pObject) {
			return plli;
		}
		plli = LinkList_GetNext(pll, plli);
	}
	return NULL;
}

void LinkList_Delete2(LINKLIST* pll, LINKLISTINFO* plli) {
	LINKLISTINFO* plliNext;
	LINKLISTINFO* plliPrev;

	if (!plli)
		return;

	plliNext = plli->plliNext;
	plliPrev = plli->plliPrev;

	if (plliNext && plliPrev) {
		plliPrev->plliNext = plliNext;
		plliNext->plliPrev = plliPrev;
	} else if (plliNext) {
		plliNext->plliPrev = NULL;
		pll->plliHead = plliNext;
	} else if (plliPrev) {
		plliPrev->plliNext = NULL;
		pll->plliTail = plliPrev;
	} else {
		pll->plliHead = NULL;
		pll->plliTail = NULL;
	}

	Memory_Free(plli);
}

void LinkList_Delete(LINKLIST* pll, PVOID pObject) {
	LinkList_Delete2(pll, LinkList_Find(pll, pObject));
}

LINKLISTINFO* LinkList_GetHead(LINKLIST* pll) {
	return pll->plliHead;
}

LINKLISTINFO* LinkList_GetTail(LINKLIST* pll) {
	return pll->plliTail;
}

LINKLISTINFO* LinkList_GetNext(LINKLIST* pll, LINKLISTINFO* plli) {
	if (plli) 
		return plli->plliNext;
	return NULL;
}

LINKLISTINFO* LinkList_GetPrev(LINKLIST* pll, LINKLISTINFO* plli) {
	if (plli) 
		return plli->plliPrev;
	return NULL;
}

PVOID LinkList_GetObject(LINKLIST* pll, LINKLISTINFO* plli) {
	if (plli) 
		return plli->pObject;
	return NULL;
}

void LinkList_Add(LINKLIST* pll, PVOID pObject) {
	LINKLISTINFO* plliNew;

	plliNew = Memory_Allocate(sizeof(LINKLISTINFO));

	plliNew->pObject = pObject;

	if (!pll->plliHead) {
		pll->plliHead = plliNew;
		pll->plliTail = plliNew;
	} else {
		plliNew->plliPrev = pll->plliTail;
		pll->plliTail->plliNext = plliNew;
		pll->plliTail = plliNew;
	}
}

void LinkList_Insert(LINKLIST* pll, LINKLISTINFO* plliNext, PVOID pObject) {
	LINKLISTINFO* plliPrev;
	LINKLISTINFO* plliNew;

	if (!plliNext) {
		LinkList_Add(pll, pObject);
	} else {

		plliPrev = plliNext->plliPrev;

		plliNew = Memory_Allocate(sizeof(LINKLISTINFO));
		plliNew->pObject = pObject;
		plliNew->plliNext = plliNext;
		plliNew->plliPrev = plliPrev;

		if (plliPrev) {
			plliPrev->plliNext = plliNew;
		} else {
			pll->plliHead = plliNew;
		}

		plliNext->plliPrev = plliNew;
	}

}