#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "..\dll\uoehelper.h"
#include "uoextreme.h"
#include "MacroCommandEditor.h"
#include "MacroEditor.h"

MACRO* g_pmacro;

WNDPROC g_pfnOldWndProc;

BOOL MacroEditor_Validate(HWND hwnd) {
	char szText[1024];
	HWND hwndOk;

	hwndOk = GetDlgItem(hwnd, IDOK);

	GetDlgItemText(hwnd, IDC_MACRO_NAME, szText, sizeof(szText));

	if (strlen(szText) == 0) {
		EnableWindow(hwndOk, FALSE);
		return FALSE;
	} else {
		if (strlen(g_pmacro->szKeyName) != 0) {
			if (Macro_Commands_GetFirst(g_pmacro)) {
				EnableWindow(hwndOk, TRUE);
				return TRUE;
			} else {
				EnableWindow(hwndOk, FALSE);
				return FALSE;
			}
		}
	}

}

LRESULT CALLBACK MacroKey_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		GetKeyNameText(lParam, g_pmacro->szKeyName, sizeof(g_pmacro->szKeyName));
		SetWindowText(hwnd, g_pmacro->szKeyName);

		MacroEditor_Validate(GetParent(hwnd));
		return 0;
	}


	return CallWindowProc(g_pfnOldWndProc, hwnd, uMsg, wParam, lParam);

}



BOOL MacroEditor_OnCommand(HWND hwndParent, HWND hwnd, WORD uCode, WORD wID) {
	BOOL bReturn = FALSE;
	MACROCOMMAND* pmc = NULL;
	int iIndex = 0;

	switch (uCode) {
	case EN_CHANGE:
		switch (wID) {
		case IDC_MACRO_NAME:
			MacroEditor_Validate(hwndParent);
			break;

		}
		break;
	case BN_CLICKED:
		switch (wID) {
		case IDC_INSERT:
			pmc = Macro_Commands_Create();
			if (MacroCommandEditor_Run(hwndParent, pmc)) {
				Macro_Commands_Add(g_pmacro, pmc);
				iIndex = SendDlgItemMessage(hwndParent, IDC_MACRO, LB_ADDSTRING, (WPARAM)0, (LPARAM)g_pszaMacroCommandTypeNames[pmc->iType]);
				SendDlgItemMessage(hwndParent, IDC_MACRO, LB_SETITEMDATA, (WPARAM)iIndex, (LPARAM)pmc);

				MacroEditor_Validate(hwndParent);
			} else {
				Macro_Commands_Destroy(pmc);
			}
			break;
		case IDC_EDIT:
			iIndex = SendDlgItemMessage(hwndParent, IDC_MACRO, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
			if (iIndex == -1) {
				MessageBox(hwndParent, "Must selected object to edit.", "Error...", MB_OK);
			} else {
				pmc = (MACROCOMMAND*)SendDlgItemMessage(hwndParent, IDC_MACRO, LB_GETITEMDATA, (WPARAM)iIndex, (LPARAM)0);
				if (MacroCommandEditor_Run(hwndParent, pmc)) {
					SendDlgItemMessage(hwndParent, IDC_MACRO, LB_DELETESTRING, (WPARAM)iIndex, (LPARAM)0);
					SendDlgItemMessage(hwndParent, IDC_MACRO, LB_INSERTSTRING, (WPARAM)iIndex, (LPARAM)g_pszaMacroCommandTypeNames[pmc->iType]);
					SendDlgItemMessage(hwndParent, IDC_MACRO, LB_SETITEMDATA, (WPARAM)iIndex, (LPARAM)pmc);
				}
			}
			break;
		case IDC_DELETE:
			iIndex = SendDlgItemMessage(hwndParent, IDC_MACRO, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
			if (iIndex == -1) {
				MessageBox(hwndParent, "Must selected object to edit.", "Error...", MB_OK);
			} else {
				if (MessageBox(hwndParent, "Are you sure you wish to delete this?", "Confirm", MB_YESNO) == IDYES) {
					pmc = (MACROCOMMAND*)SendDlgItemMessage(hwndParent, IDC_MACRO, LB_GETITEMDATA, (WPARAM)iIndex, (LPARAM)0);

					Macro_Commands_Delete(g_pmacro, pmc);

					SendDlgItemMessage(hwndParent, IDC_MACRO, LB_DELETESTRING, (WPARAM)iIndex, (LPARAM)0);
				}
			}
			break;
		case IDOK:
			g_pmacro->bCtrl = IsDlgButtonChecked(hwndParent, IDC_CTRL);
			g_pmacro->bAlt = IsDlgButtonChecked(hwndParent, IDC_ALT);
			g_pmacro->bShift = IsDlgButtonChecked(hwndParent, IDC_SHIFT);
			GetDlgItemText(hwndParent, IDC_MACRO_NAME, g_pmacro->szDescription, sizeof(g_pmacro->szDescription));
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

BOOL CALLBACK MacroEditor_DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	BOOL bReturn = FALSE;
	HWND hwndMacroKey;
	MACROCOMMAND* pmc;
	int iIndex;

	switch (uMsg) {
	case WM_INITDIALOG:
		hwndMacroKey = GetDlgItem(hwnd, IDC_MACRO_KEY);
		g_pfnOldWndProc = (WNDPROC)SetWindowLong(hwndMacroKey, GWL_WNDPROC, (LONG)MacroKey_WndProc);
		SetWindowText(hwndMacroKey, g_pmacro->szKeyName);

		CheckDlgButton(hwnd, IDC_CTRL, g_pmacro->bCtrl);
		CheckDlgButton(hwnd, IDC_ALT, g_pmacro->bAlt);
		CheckDlgButton(hwnd, IDC_SHIFT, g_pmacro->bShift);
		SetDlgItemText(hwnd, IDC_MACRO_NAME, g_pmacro->szDescription);

		pmc = Macro_Commands_GetFirst(g_pmacro);
		while (pmc != NULL) {
			iIndex = SendDlgItemMessage(hwnd, IDC_MACRO, LB_ADDSTRING, (WPARAM)0, (LPARAM)g_pszaMacroCommandTypeNames[pmc->iType]);
			SendDlgItemMessage(hwnd, IDC_MACRO, LB_SETITEMDATA, (WPARAM)iIndex, (LPARAM)pmc);

			pmc = Macro_Commands_GetNext(g_pmacro, pmc);
		}


		MacroEditor_Validate(hwnd);
		break;
	case WM_COMMAND:
		bReturn = MacroEditor_OnCommand(hwnd, (HWND)lParam, HIWORD(wParam), LOWORD(wParam));
		break;

	}

	return bReturn;
}

void MacroEditor_Initialize(void) {

}

BOOL MacroEditor_Run(HWND hwnd, MACRO* pmacro) {

	g_pmacro = pmacro;

	if (DialogBox(g_hinst, MAKEINTRESOURCE(IDD_MACRO_EDITOR), hwnd, MacroEditor_DlgProc) == IDOK) {
		return TRUE;
	}

	return FALSE;
}
