/*

typedef struct {
	DWORD dwThreadId;
	HWND hwnd;
	MACRO* pmacro;
} PROCESSMACROINFO;
*/

DWORD threadProcessMacro(MACRO* pmacro);
void EquipItem(BYTE bSlot, DWORD dwObjectId);
void UnequipItem(DWORD dwObjectId);

void Processors_Macro_Target(DWORD dwObjectId, HWND hwnd);
void Processors_Macro_GetObject(DWORD dwObjectId);
void Processors_Macro_PutObject(DWORD dwObjectId, DWORD dwWhereId);
void Processors_Macro_CastSpell(BYTE bSpellNumber);
void Processors_Macro_DoubleClick(DWORD dwObjectId);