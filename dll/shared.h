#define MAX_FAKECONTAINERS 256


typedef struct {
	DWORD dwThreadId;
	HWND hwnd;

	struct {
		DWORD dwId;
		char szName[128];

		struct {
			DWORD dwRightHandId;
			DWORD dwLeftHandId;
			DWORD dwBackpackId;
		} Items;

	} Character;

	struct {
		BOOL bVisible;
		BOOL bEnabled;
		DWORD dwId;
	} TargetCursor;

	struct {
		DWORD dwLastTargetId;
	} History;


	struct {
		BOOL bShowNames;
		BOOL bFastWalk;
		BOOL bAlwaysLight;
		BOOL bPreventAutoClose;

	} Options;

	struct {
		BOOL bTargetCursor;
		BOOL bBardMusic;
		BOOL bSpiritSpeakSound;
		BOOL bGuildNames;
		BOOL bGuildTitlesOnly;
		BOOL bHiddenPlayers;
		BOOL bHiddenPlayerMessages;
		BOOL bNonHumans;
		BOOL bWeather;
	} Filters;

	struct {
		int nPosition;
		DWORD dwIds[MAX_FAKECONTAINERS];


	} FakeContainers;


} ULTIMAONLINE_DATA;

extern DLL ULTIMAONLINE_DATA UltimaOnline_Data;

extern DLL HHOOK g_hhook;
extern DLL BOOL g_bLoggedIn;
extern DLL BYTE g_baOutBuffer[1024];
extern DLL WORD g_wOutBufferSize;
//extern DLL BOOL g_bMonitorIncomingData;


//extern DLL DWORD g_dwCharacterId;
//extern DLL char g_szCharacterName[128];

//extern DLL BOOL g_bIsTargetCursor;
//extern DLL DWORD g_dwTargetCursorId;
//extern DLL DWORD g_dwLastTargetId;

extern DLL DWORD g_GetObject_dwObjectId;
extern DLL WORD g_GetObject_wObjectType;
extern DLL BYTE g_GetObject_szObjectName[1024];

extern DLL BOOL g_bDisableSendCommand;

//extern DLL DWORD g_dwLeftHandId;
//extern DLL DWORD g_dwRightHandId;
//extern DLL DWORD g_dwBackpackId;
