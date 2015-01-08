
#ifndef DLL 
#ifdef ISDLL
#define DLL _declspec(dllexport)
#else
#define DLL _declspec(dllimport)
#endif
#endif


typedef struct {
	int iType;
	union {
		char szObjectUniqueName[OBJECT_UNIQUENAME_SIZE];
		BYTE bSpellNumber;

	} u;
} MACROCOMMAND;

typedef struct {
	char szDescription[128];
	char szKeyName[64];
	BOOL bAlt;
	BOOL bShift;
	BOOL bCtrl;
	LINKLIST* pll;
} MACRO;

#define MAX_SPELLS					64

#define MAX_MACRO_COMMAND_TYPES		12

#define MCT_EQUIP_LEFT				0
#define MCT_EQUIP_RIGHT				1
#define MCT_UNEQUIP_LEFT			2
#define MCT_UNEQUIP_RIGHT			3
#define MCT_TARGETOBJECT			4
#define MCT_LASTTARGET				5
#define MCT_TARGETSELF				6
#define MCT_ARCHERYCOUNT			7
#define MCT_REAGENTCOUNT			8
#define MCT_CASTSPELL				9
#define MCT_WAITFORTARGETCURSOR		10
#define MCT_OPENBANK				11

extern DLL char* g_pszaMacroCommandTypeNames[MAX_MACRO_COMMAND_TYPES];
extern DLL char* g_pszaSpellNames[MAX_SPELLS];

DLL MACRO* Macro_Create(void);
DLL void Macro_Destroy(MACRO* pmacro);
DLL MACROCOMMAND* Macro_Commands_Create(void);
DLL void Macro_Commands_Destroy(MACROCOMMAND* pmc);
DLL void Macro_Commands_Add(MACRO* pmacro, MACROCOMMAND* pmc);
DLL void Macro_Commands_Delete(MACRO* pmacro, MACROCOMMAND* pmc);
DLL void Macro_Commands_RemoveAll(MACRO* pmacro);
DLL MACROCOMMAND* Macro_Commands_GetFirst(MACRO* pmacro);
DLL MACROCOMMAND* Macro_Commands_GetNext(MACRO* pmacro, MACROCOMMAND* pmc);


DLL void Macros_Add(MACRO* pmacro);
DLL void Macros_Delete(MACRO* pmacro);
DLL MACRO* Macros_GetNext(MACRO* pmacro);
DLL MACRO* Macros_GetFirst(void);
DLL void Macros_Load(void);
DLL void Macros_Save(void);

DLL void Macros_Initialize(void);

