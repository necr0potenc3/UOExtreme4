#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "..\dll\uoehelper.h"
#include "MacroEditor.h"
#include "MainPage2.h"
#include "uoextreme.h"



BOOL MainPage2_OnCommand(HWND hwndParent, HWND hwnd, WORD uCode, WORD wID) {
	BOOL bReturn = FALSE;
	LV_ITEM lvi;
	HWND hwndMacros;
	MACRO* pmacro;
	char szText[1024];
	int iNewIndex;

	switch (uCode) {
	case BN_CLICKED:
		switch (wID) {
		case IDC_NEW:
			pmacro = Macro_Create();
			if (MacroEditor_Run(hwndParent, pmacro)) {
				// stuff

				Macros_Add(pmacro);
				Macros_Save();
				PostThreadMessage(UltimaOnline_Data.dwThreadId, UOEM_MACROSCHANGED, (WPARAM)0, (LPARAM)0);


				hwndMacros = GetDlgItem(hwndParent, IDC_MACROS);
				iNewIndex = ListView_GetItemCount(hwndMacros);

				lvi.mask = LVIF_TEXT | LVIF_PARAM;
				lvi.iItem = iNewIndex;
				lvi.iSubItem = 0;
				lvi.pszText = pmacro->szDescription;
				lvi.lParam = (LPARAM)pmacro;
				ListView_InsertItem(hwndMacros, &lvi);

				strcpy(szText, "");
				if (pmacro->bCtrl) {
					strcat(szText, "Ctrl+");
				}
				if (pmacro->bAlt) {
					strcat(szText, "Alt+");
				}
				if (pmacro->bShift) {
					strcat(szText, "Shift+");
				}
				strcat(szText, pmacro->szKeyName);


				ListView_SetItemText(hwndMacros, iNewIndex, 1, szText);		
			} else {
				Macro_Destroy(pmacro);
			}

			bReturn = TRUE;
			break;
		case IDC_EDIT:
			hwndMacros = GetDlgItem(hwndParent, IDC_MACROS);

			lvi.mask = LVIF_PARAM;
			lvi.iSubItem = 0;	
			lvi.iItem = ListView_GetNextItem(hwndMacros, -1, LVNI_FOCUSED);
			if (lvi.iItem == -1) {
				MessageBox(hwndParent, "Must selected macro to edit.", "Error...", MB_OK);
			} else {
				ListView_GetItem(hwndMacros, &lvi);

				pmacro = (MACRO*)lvi.lParam;

				if (MacroEditor_Run(hwndParent, pmacro)) {
					Macros_Save();
					PostThreadMessage(UltimaOnline_Data.dwThreadId, UOEM_MACROSCHANGED, (WPARAM)0, (LPARAM)0);


					ListView_SetItemText(hwndMacros, lvi.iItem, 0, pmacro->szDescription);
					strcpy(szText, "");
					if (pmacro->bCtrl) {
						strcat(szText, "Ctrl+");
					}
					if (pmacro->bAlt) {
						strcat(szText, "Alt+");
					}
					if (pmacro->bShift) {
						strcat(szText, "Shift+");
					}
					strcat(szText, pmacro->szKeyName);

					ListView_SetItemText(hwndMacros, lvi.iItem, 1, szText);		

				}
			}

			bReturn = TRUE;
			break;
		case IDC_DELETE:
			hwndMacros = GetDlgItem(hwndParent, IDC_MACROS);

			lvi.mask = LVIF_PARAM;
			lvi.iSubItem = 0;	
			lvi.iItem = ListView_GetNextItem(hwndMacros, -1, LVNI_FOCUSED);
			if (lvi.iItem == -1) {
				MessageBox(hwndParent, "Must selected macro first.", "Error...", MB_OK);
			} else {
				if (MessageBox(hwndParent, "Are you sure you wish to delete this?", "Confirm", MB_YESNO) == IDYES) {
					ListView_GetItem(hwndMacros, &lvi);

					pmacro = (MACRO*)lvi.lParam;

					Macros_Delete(pmacro);
					Macros_Save();

					ListView_DeleteItem(hwndMacros, lvi.iItem);
					
					PostThreadMessage(UltimaOnline_Data.dwThreadId, UOEM_MACROSCHANGED, (WPARAM)0, (LPARAM)0);
				}
			}

			bReturn = TRUE;
			break;

		}
		break;

	}

	return bReturn;
}


BOOL CALLBACK MainPage2_DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	BOOL bReturn = FALSE;
	LV_COLUMN lvc = { 0 };
	LV_ITEM lvi = { 0 };
	HWND hwndMacros = NULL;
	MACRO* pmacro = NULL;
	char szText[1024] = { 0 };

	switch (uMsg) {
	case WM_INITDIALOG:
		hwndMacros = GetDlgItem(hwnd, IDC_MACROS);

		lvc.mask = LVCF_TEXT | LVCF_WIDTH;
		lvc.pszText = "Macro Description";
		lvc.cx = 162;
		ListView_InsertColumn(hwndMacros, 0, &lvc);
		lvc.pszText = "Hotkey";
		lvc.cx = 110;
		ListView_InsertColumn(hwndMacros, 1, &lvc);

		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iItem = 0;
		lvi.iSubItem = 0;

		pmacro = Macros_GetFirst();
		while (pmacro != NULL) {
			lvi.pszText = pmacro->szDescription;
			lvi.lParam = (LPARAM)pmacro;
			ListView_InsertItem(hwndMacros, &lvi);

			strcpy(szText, "");
			if (pmacro->bCtrl) {
				strcat(szText, "Ctrl+");
			}
			if (pmacro->bAlt) {
				strcat(szText, "Alt+");
			}
			if (pmacro->bShift) {
				strcat(szText, "Shift+");
			}
			strcat(szText, pmacro->szKeyName);


			ListView_SetItemText(hwndMacros, lvi.iItem, 1, szText);		

			lvi.iItem++;
			pmacro = Macros_GetNext(pmacro);
		}
		break;

	case WM_COMMAND:
		bReturn = MainPage2_OnCommand(hwnd, (HWND)lParam, HIWORD(wParam), LOWORD(wParam));
		break;

	}

	return bReturn;
}

