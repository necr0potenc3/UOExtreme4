
#ifndef MEMORY_H
#define MEMORY_H

#ifndef DLL 
#ifdef ISDLL
#define DLL _declspec(dllexport)
#else
#define DLL _declspec(dllimport)
#endif
#endif


PVOID Memory_Allocate(DWORD dwBytes);
void Memory_Free(PVOID pMemory);


#endif