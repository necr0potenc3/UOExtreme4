#include <windows.h>
#include "uoehelper.h"

//int g_nAllocatedBlocks;

PVOID Memory_Allocate(DWORD dwBytes) {
//	g_nAllocatedBlocks++;
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, dwBytes);
}

void Memory_Free(PVOID pMemory) {
//	char szText[1024];
	HeapFree(GetProcessHeap(), 0, pMemory);
//	g_nAllocatedBlocks--;
//	wsprintf(szText, "g_nAllocatedBlocks=%d\n", g_nAllocatedBlocks);
//	OutputDebugString(szText);
}