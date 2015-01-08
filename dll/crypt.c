#include <windows.h>
#include "crypt.h"


#include "skey.c"

int Encrypt(BYTE* pOut, BYTE* pData, UINT nSize) {
	UINT i = 0, j = 0;
	int nBitIndex = 0; // Offset in output byte
	int nBufferBytesInUse = 0;
	
	for (i = 0; i < nSize;i++){
		for (j = 0; j < (keyid[pData[i]]&0xF); j++) {
			pOut[nBufferBytesInUse]=(pOut[nBufferBytesInUse] << 1) + ((keyid[pData[i]] >> (4+(keyid[pData[i]]&0xF))-(j+1)) & 1);
			nBitIndex++;
			if (nBitIndex == 8) {
				nBufferBytesInUse++;
				nBitIndex = 0;
			}
		}
	}
	for (j = 0; j < (keyid[256]&0xF); j++) {
		pOut[nBufferBytesInUse] = (pOut[nBufferBytesInUse] << 1) + ((keyid[256] >> (4+(keyid[256]&0xF))-(j+1)) & 1);
		nBitIndex++;
		if (nBitIndex == 8) {
			nBufferBytesInUse++;
			nBitIndex = 0;
		}
	}
	if (nBitIndex == 0) {
		nBufferBytesInUse--;
	} else {
		pOut[nBufferBytesInUse] = pOut[nBufferBytesInUse] << (8 - nBitIndex);
	}
	return nBufferBytesInUse + 1;
}


int Decrypt(BYTE* pOut, BYTE* pData, UINT nSize, PROCESSDATA pfnProcessData) {
	UINT i = 0, j = 0;
	int nBitIndex = 0;
	int nBitsProcessed = 0;
	UINT nBufferBytesInUse = 0;
	UINT uCrypt = 0;
	UINT uLastIndex = 0;
	
	while (i < nSize) {
		uCrypt = 0;
		nBitIndex = 7 - (nBitsProcessed % 8);


		for (j = 0; j < 12; j++) {
			uCrypt += ((pData[i]&(1<<nBitIndex)) >> nBitIndex) << (12-j);
			nBitIndex--;
			if (nBitIndex < 0) {
				i++;
				nBitIndex = 7;
			}
			
		}
		if (keyid[256]>>4 == uCrypt>>(12-(keyid[256]&0xf)+1)) {
			pfnProcessData(pOut, nBufferBytesInUse);

			nBitsProcessed += (keyid[256]&0xf);

			if ((nBitsProcessed % 8) != 0) {
				nBitsProcessed += 8 - (nBitsProcessed % 8);
			}

			uLastIndex = (nBitsProcessed / 8);
			nBufferBytesInUse = 0;
			
		} else {
			for (j = 0; j < 256; j++) {
				if (keyid[j]>>4 == uCrypt>>(12-(keyid[j]&0xf)+1)) {
					pOut[nBufferBytesInUse++] = (BYTE)j;
					nBitsProcessed += (keyid[j]&0xf);
					break;
				}
			}
		}
		i = (nBitsProcessed / 8);

	}

	nSize -= uLastIndex;
	memmove(pData, &pData[uLastIndex], nSize);
	memset(&pData[nSize], 0, uLastIndex);



	return nSize;
}


