#include <windows.h>
#include "uoehelper.h"


#pragma data_seg(".shared")

ULTIMAONLINE_DATA UltimaOnline_Data = { 0 };

BOOL g_bLoggedIn = FALSE;

HHOOK g_hhook = 0;
BYTE g_baOutBuffer[1024] = { 0 };
WORD g_wOutBufferSize = 0;

DWORD g_GetObject_dwObjectId = 0;
WORD g_GetObject_wObjectType = 0;
BYTE g_GetObject_szObjectName[1024] = { 0 };

BOOL g_bDisableSendCommand = FALSE;
