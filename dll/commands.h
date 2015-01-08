
void Commands_Initialize(void);

void Commands_Fake_Add(BYTE* pbaCommand, DWORD nSize);
BOOL Commands_Fake_IsAvailable(void);
int Commands_Fake_Get(BYTE* pbaBuffer);

void Commands_EquipObject(BYTE bSlot, DWORD dwObjectId, UINT uFlags);
void Commands_GetObject(DWORD dwObjectId, UINT uFlags);
void Commands_PutObject(DWORD dwObjectId, DWORD dwWhereId, UINT uFlags);
void Commands_CastSpell(BYTE bSpellNumber);
void Commands_Target(DWORD dwObjectId);
void Commands_OpenContainer(DWORD dwObjectId);
void Commands_OpenBank(DWORD dwObjectId);
void Commands_ShowName(DWORD dwObjectId);

void Commands_Say(DWORD dwWhoId, char* pszText, WORD wColor);
void Commands_SystemMessage(char* pszSystemMessage);
void Commands_WalkOkay(BYTE bNumber);
void Commands_ItemEquipped(BYTE bSlot, DWORD dwObjectId, WORD dwObjectType);