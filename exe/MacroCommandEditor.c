#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "..\dll\common.h"
#include "uoextreme.h"
#include "MacroCommandEditor.h"

MACROCOMMAND* g_pmc;



BOOL MacroCommandEditor_OnCommand(HWND hwndParent, HWND hwnd, WORD uCode, WORD wID) {
	BOOL bReturn = FALSE;
	HWND hwndOptions;
	OBJECT* pobj;
	UINT i;
	int iIndex;

	switch (uCode) {
	case CBN_SELCHANGE:
		switch (wID) {
		case IDC_COMMAND_OPTIONS:
			EnableWindow(GetDlgItem(hwndParent, IDOK), TRUE);
			break;
		case IDC_COMMAND_TYPES:

			hwndOptions = GetDlgItem(hwndParent, IDC_COMMAND_OPTIONS);
			SendMessage(hwndOptions, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
			SendMessage(hwndOptions, CB_SETCURSEL, (WPARAM)-1, (LPARAM)0);

			EnableWindow(GetDlgItem(hwndParent, IDOK), FALSE);

			switch (SendMessage(hwnd, CB_GETCURSEL, (WPARAM)0, (LPARAM)0)) {
			case MCT_EQUIP_LEFT:
			case MCT_EQUIP_RIGHT:
			case MCT_TARGETOBJECT:
				EnableWindow(hwndOptions, TRUE);
				pobj = Objects_GetFirst();
				while (pobj != NULL) {
					SendMessage(hwndOptions, CB_ADDSTRING, (WPARAM)0, (LPARAM)pobj->szUniqueName);

					pobj = Objects_GetNext(pobj);
				}
				break;

			case MCT_CASTSPELL:
				EnableWindow(hwndOptions, TRUE);
				for (i = 0; i < MAX_SPELLS; i++) {
					SendMessage(hwndOptions, CB_ADDSTRING, (WPARAM)0, (LPARAM)g_pszaSpellNames[i]);
				}
				break;
			case MCT_UNEQUIP_LEFT:
			case MCT_UNEQUIP_RIGHT:
			case MCT_LASTTARGET:
			case MCT_TARGETSELF:
			case MCT_ARCHERYCOUNT:
			case MCT_REAGENTCOUNT:
			case MCT_WAITFORTARGETCURSOR:
			case MCT_OPENBANK:
				EnableWindow(hwndOptions, FALSE);
				EnableWindow(GetDlgItem(hwndParent, IDOK), TRUE);
				break;

			}
			break;
		}
		break;
	case BN_CLICKED:
		switch (wID) {
		case IDOK:

			g_pmc->iType = SendMessage(GetDlgItem(hwndParent, IDC_COMMAND_TYPES), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
			iIndex = SendMessage(GetDlgItem(hwndParent, IDC_COMMAND_OPTIONS), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
			switch (g_pmc->iType) {
			case MCT_EQUIP_LEFT:
			case MCT_EQUIP_RIGHT:
			case MCT_TARGETOBJECT:
				SendMessage(GetDlgItem(hwndParent, IDC_COMMAND_OPTIONS), CB_GETLBTEXT, (WPARAM)iIndex, (LPARAM)g_pmc->u.szObjectUniqueName);
				break;

			case MCT_CASTSPELL:
				g_pmc->u.bSpellNumber = iIndex + 1;
				break;

			case MCT_UNEQUIP_LEFT:
			case MCT_UNEQUIP_RIGHT:
			case MCT_LASTTARGET:
			case MCT_TARGETSELF:
			case MCT_ARCHERYCOUNT:
			case MCT_REAGENTCOUNT:
			case MCT_WAITFORTARGETCURSOR:
			case MCT_OPENBANK:
				break;

			}
			EndDialog(hwndParent, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hwndParent, IDCANCEL);
			break;

		}
		break;

	}

	return bReturn;

}

BOOL CALLBACK MacroCommandEditor_DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	BOOL bReturn = FALSE;
	HWND hwndTypes = 0;
	UINT i = 0;
	int iIndex = 0;

	switch (uMsg) {
	case WM_INITDIALOG:
		hwndTypes = GetDlgItem(hwnd, IDC_COMMAND_TYPES);
		for (i = 0; i < MAX_MACRO_COMMAND_TYPES; i++) {
			SendMessage(hwndTypes, CB_ADDSTRING, (WPARAM)0, (LPARAM)g_pszaMacroCommandTypeNames[i]);
		}
		if (!g_pmc) {
			EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
		}
		
		SendMessage(hwndTypes, CB_SETCURSEL, (WPARAM)g_pmc->iType, (LPARAM)0);

		break;
	case WM_COMMAND:
		bReturn = MacroCommandEditor_OnCommand(hwnd, (HWND)lParam, HIWORD(wParam), LOWORD(wParam));
		break;

	}

	return bReturn;
}

void MacroCommandEditor_Initialize(void) {

}

BOOL MacroCommandEditor_Run(HWND hwnd, MACROCOMMAND* pmc) {

	g_pmc = pmc;

	if (DialogBox(g_hinst, MAKEINTRESOURCE(IDD_MACRO_COMMAND_EDITOR), hwnd, MacroCommandEditor_DlgProc) == IDOK) {
		return TRUE;
	}

	return FALSE;
}
