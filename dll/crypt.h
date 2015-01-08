
typedef void (__stdcall *PROCESSDATA)(BYTE* pData, UINT nSize);

int Encrypt(BYTE* pOut, BYTE* pData, UINT nSize);
int Decrypt(BYTE* pOut, BYTE* pData, UINT nSize, PROCESSDATA pfnProcessData);