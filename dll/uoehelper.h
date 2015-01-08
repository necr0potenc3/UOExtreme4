#ifndef DLL 
#ifdef ISDLL
#define DLL _declspec(dllexport)
#else
#define DLL _declspec(dllimport)
#endif
#endif

#include "common.h"


#define UOEM_BEGIN							0x7000
#define UOEM_END							0x7001
#define UOEM_GETOBJECTNAME					0x7003
#define UOEM_MACROSCHANGED					0x7004
#define UOEM_OBJECTDEFINITIONSCHANGED		0x7005
#define UOEM_SENDWALKOKAY					0x7006
#define UOED_SEND							0x7100


typedef struct {
	BOOL bInitialized;
	BOOL bEnabled;


} EXTREMEHELPER_DATA;

extern EXTREMEHELPER_DATA ExtremeHelper_Data;


DLL LRESULT CALLBACK GetMsgProc(int iCode, WPARAM wParam, LPARAM lParam);