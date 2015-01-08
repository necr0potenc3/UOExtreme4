
#define IDC_LAUNCH 0x600

void Extreme_Begin(char* pszCharacterName);

void Extreme_End(void);

void Options_Load(void);
void Options_Save(void);

extern char g_szClientFileName[256];

extern HANDLE g_heventTargetCursor;

//extern DWORD g_dwUltimaOnlineThreadId;
//extern HWND g_hwnd;

extern HWND g_hwndMain;
extern HINSTANCE g_hinst;