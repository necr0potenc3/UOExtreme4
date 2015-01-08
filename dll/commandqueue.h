
#define QCM_EXECUTE					0x7300

#define COMMAND_PRIORITY_LOWEST			0
#define COMMAND_PRIORITY_BELOW_NORMAL	5
#define COMMAND_PRIORITY_NORMAL			10
#define COMMAND_PRIORITY_HIGHEST		20

#define COMMAND_FLAG_KILLTARGETCURSOR	1
#define COMMAND_FLAG_NOWAIT				2
#define COMMAND_FLAG_LEFTEMPTY			4
#define COMMAND_FLAG_USELEFTHAND		8
#define COMMAND_FLAG_RIGHTEMPTY			16
#define COMMAND_FLAG_USERIGHTHAND		32
#define COMMAND_FLAG_SETGETOBJECT		64
#define COMMAND_FLAG_USEGETOBJECT		128



typedef struct {
	BYTE* pbaCommand;
	WORD wCommandSize;
	BYTE* pbaWaitForThis;
	WORD wWaitForThisSize;
	BYTE bRetry;
	BYTE bPriority;
	UINT uFlags;
} COMMANDQUEUEITEM;

#define CQWFTE_FAILURE			0
#define CQWFTE_SUCCESS			1

typedef struct {
	BOOL bInitialized;

	CRITICAL_SECTION cs;
	LINKLIST* pll;
	HANDLE hthread;
	HANDLE heventActivate;
	HANDLE heventExecuted;
	BOOL bExecutionSuccessful;

	struct {
		BOOL bEnabled;
		CRITICAL_SECTION cs;
		BYTE* pbaBuffer;
		WORD wSize;
		HANDLE hevents[2];
	} WaitForThis;

} COMMANDQUEUE_DATA;

extern COMMANDQUEUE_DATA CommandQueue_Data;



void CommandQueue_Initialize(void);
void CommandQueue_Add(BYTE* pbaCommand, WORD wCommandSize, BYTE* pbaWaitForThis, WORD wWaitForThisSize, BYTE bPriority, UINT uFlags);
//void CommandQueue_Flush(void);

void CommandQueue_OnExecute(COMMANDQUEUEITEM* pcqd);