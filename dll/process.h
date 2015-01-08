
extern BOOL g_bWantObjectName; 

extern HANDLE g_heventReady;
extern HANDLE g_heventNotReady;

//extern BYTE* g_pFakeRawOutBuffer;
//extern UINT g_nFakeRawOutBufferBytesInUse;


extern HANDLE g_heventTargetCursor;

extern HANDLE g_heventCountDone;
extern BOOL g_bCountObjects;
//extern UINT g_nNumberOfPacksWaiting;
extern UINT g_nNumberOfArrows;
extern UINT g_nNumberOfBolts;
extern UINT g_nNumberOfSulphurousAsh;
extern UINT g_nNumberOfBloodMoss;
extern UINT g_nNumberOfMandrakeRoot;
extern UINT g_nNumberOfSpiderSilk;
extern UINT g_nNumberOfGarlic;
extern UINT g_nNumberOfGinseng;
extern UINT g_nNumberOfNightshade;
extern UINT g_nNumberOfBlackPearl;

UINT ProcessNewData(BYTE* pData, UINT nSize);
void Reset(void);
void SendCommand(void);
void ProcessCommand(BYTE* pData, UINT nSize);

