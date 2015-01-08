#ifndef OBJECTS_H
#define OBJECTS_H


#ifndef DLL 
#ifdef ISDLL
#define DLL _declspec(dllexport)
#else
#define DLL _declspec(dllimport)
#endif
#endif


#define OBJECT_UNIQUENAME_SIZE 128


#define OBJECTID1(a)((BYTE)((a&0xff000000)>>24))
#define OBJECTID2(a)((BYTE)((a&0x00ff0000)>>16))
#define OBJECTID3(a)((BYTE)((a&0x0000ff00)>>8))
#define OBJECTID4(a)((BYTE)((a&0x000000ff)>>0))

__inline DWORD MAKEOBJECTID(BYTE* a) {
	return ((a[0]<<24)+(a[1]<<16)+(a[2]<<8)+(a[3]<<0));
}
__inline WORD MAKEOBJECTTYPE(BYTE* a) {
	return ((a[0]<<8)+(a[1]<<0));
}



#define MAKEFAKEID(a)(((a>>24==0x40?0xaa:a>>24)<<24)|(a&0x00ffffff))
#define MAKEREALID(a)(((a>>24==0xaa?0x40:a>>24)<<24)|(a&0x00ffffff))


typedef struct {
	char szUniqueName[OBJECT_UNIQUENAME_SIZE];

} OBJECT;

typedef struct {
	char szUniqueName[OBJECT_UNIQUENAME_SIZE];
	char szName[128];
	DWORD dwId;
	WORD wType;
	BOOL bUseAny;
	BOOL bCheckNested;
} OBJECTDEFINITION;

DLL OBJECT*		Object_Create();
DLL void		Object_Destroy(OBJECT* pobj);

DLL void		Objects_Initialize(void);
DLL void		Objects_Add(OBJECT* pobj);
DLL void		Objects_Delete(OBJECT* pobj);
DLL OBJECT*		Objects_GetFirst(void);
DLL OBJECT*		Objects_GetNext(OBJECT* pobj);
DLL OBJECT*		Objects_Find(char* pszUniqueName);
DLL void		Objects_Load(void);
DLL void		Objects_Save(void);


DLL OBJECTDEFINITION*	ObjectDefinition_Create();
DLL void		ObjectDefinition_Destroy(OBJECTDEFINITION* pobjdef);

DLL void		ObjectDefinitions_Initialize(void);
DLL void		ObjectDefinitions_Add(OBJECTDEFINITION* pobjdef);
DLL void		ObjectDefinitions_Delete(OBJECTDEFINITION* pobjdef);
DLL void		ObjectDefinitions_RemoveAll(void);
DLL OBJECTDEFINITION*	ObjectDefinitions_GetFirst(void);
DLL OBJECTDEFINITION*	ObjectDefinitions_GetNext(OBJECTDEFINITION* pobjdef);
DLL OBJECTDEFINITION*	ObjectDefinitions_Find(char* pszUniqueName);
DLL void		ObjectDefinitions_Load(void);
DLL void		ObjectDefinitions_Save(void);


#endif