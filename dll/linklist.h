
#ifndef LINKLIST_H
#define LINKLIST_H

#ifndef DLL 
#ifdef ISDLL
#define DLL _declspec(dllexport)
#else
#define DLL _declspec(dllimport)
#endif
#endif


struct _LINKLISTINFO {
	struct _LINKLISTINFO*	plliPrev;
	struct _LINKLISTINFO*	plliNext;
	PVOID			pObject;
};

typedef struct _LINKLISTINFO LINKLISTINFO;

typedef struct {
	LINKLISTINFO* plliHead;
	LINKLISTINFO* plliTail;
} LINKLIST;

DLL LINKLISTINFO* LinkList_Find(LINKLIST* pll, PVOID pObject);

DLL LINKLIST* LinkList_Create(void);
DLL void LinkList_Destroy(LINKLIST* pll);
DLL void LinkList_Add(LINKLIST* pll, PVOID pObject);
DLL void LinkList_Insert(LINKLIST* pll, LINKLISTINFO* plli, PVOID pObject);
DLL void LinkList_Delete(LINKLIST* pll, PVOID pObject);
DLL void LinkList_Delete2(LINKLIST* pll, LINKLISTINFO* plli);
DLL void LinkList_RemoveAll(LINKLIST* pll);
DLL PVOID LinkList_GetObject(LINKLIST* pll, LINKLISTINFO* plli);
DLL LINKLISTINFO* LinkList_GetHead(LINKLIST* pll);
DLL LINKLISTINFO* LinkList_GetTail(LINKLIST* pll);
DLL LINKLISTINFO* LinkList_GetPrev(LINKLIST* pll, LINKLISTINFO* plli);
DLL LINKLISTINFO* LinkList_GetNext(LINKLIST* pll, LINKLISTINFO* plli);


#endif