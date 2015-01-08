
typedef struct {
	DWORD dwThreadId;
	BYTE* pbaCommand;
	WORD wCommandSize;
} QUEUECOMMAND;

extern LINKLIST* g_pllContainers;
extern CRITICAL_SECTION g_csContainers;

void Processors_QueueCommand(BYTE* pbaCommand, WORD wCommandSize);
BOOL Processors_Incoming_MoveItem(BYTE* pbaStream);
BOOL Processors_Incoming_PutItem(BYTE* pbaStream);
BOOL Processors_Incoming_Walk(BYTE* pbaStream);
BOOL Processors_Incoming_Speech(BYTE* pbaStream);
BOOL Processors_Incoming_PlaySound(BYTE* pbaStream);
void Processors_Incoming_LoadPlayer(BYTE* pbaStream);
void Processors_Incoming_WearItem(BYTE* pbaStream);

BOOL Processors_Incoming_ContainerInfo(BYTE* pbaStream);
BOOL Processors_Incoming_ItemsInContainer(BYTE* pbaStream);


void Processors_Initialize(void);