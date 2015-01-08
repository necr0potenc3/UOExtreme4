#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "..\dll\uoehelper.h"
#include "uoextreme.h"
#include "objecteditor.h"
#include "objectdefinitioneditor.h"
#include "MainPage3.h"

HWND g_hwndMainPage3;

void MainPage3_Update(void) {
	HWND hwndObjects = NULL;
	OBJECT* pobj = NULL;
	OBJECTDEFINITION* pobjdef = NULL;
	LV_ITEM lvi;
	int iCount = 0;
	int i = 0;

	EnableWindow(GetDlgItem(g_hwndMainPage3, IDC_DEFINE), g_bLoggedIn);
	EnableWindow(GetDlgItem(g_hwndMainPage3, IDC_UNDEFINE), g_bLoggedIn);


	hwndObjects = GetDlgItem(g_hwndMainPage3, IDC_OBJECTS);


	iCount = ListView_GetItemCount(hwndObjects);

	lvi.mask = LVIF_PARAM;
	lvi.iSubItem = 0;
	for (i = 0; i < iCount; i++) {
		lvi.iItem = i;
		ListView_GetItem(hwndObjects, &lvi);

		pobj = (OBJECT*)lvi.lParam;

		pobjdef = ObjectDefinitions_Find(pobj->szUniqueName);
		if (pobjdef) {
			ListView_SetItemText(hwndObjects, i, 1, "Yes");
			ListView_SetItemText(hwndObjects, i, 2, pobjdef->szName);
		} else {
			ListView_SetItemText(hwndObjects, i, 1, "No");
			ListView_SetItemText(hwndObjects, i, 2, "");
		}
	}

}

BOOL MainPage3_OnCommand(HWND hwndParent, HWND hwnd, WORD uCode, WORD wID) {
	BOOL bReturn = FALSE;
	LV_ITEM lvi = { 0 };
	OBJECTDEFINITION* pobjdef = NULL;
	OBJECT* pobj = NULL;
	HWND hwndObjects = NULL;
	BOOL bTemp = FALSE;

	switch (uCode) {
	case BN_CLICKED:
		switch (wID) {
		case IDC_NEW:
			pobj = Object_Create();
			if (ObjectEditor_Run(hwndParent, pobj)) {

				Objects_Add(pobj);
				Objects_Save();

				hwndObjects = GetDlgItem(hwndParent, IDC_OBJECTS);

				lvi.mask = LVIF_TEXT | LVIF_PARAM;
				lvi.iItem = ListView_GetItemCount(hwndObjects);
				lvi.iSubItem = 0;

				lvi.pszText = pobj->szUniqueName;
				lvi.lParam = (LPARAM)pobj;
				ListView_InsertItem(hwndObjects, &lvi);
			} else {
				Object_Destroy(pobj);
			}

			MainPage3_Update();
			bReturn = TRUE;
			break;
		case IDC_DELETE:
			hwndObjects = GetDlgItem(hwndParent, IDC_OBJECTS);

			lvi.mask = LVIF_PARAM;
			lvi.iSubItem = 0;	
			lvi.iItem = ListView_GetNextItem(hwndObjects, -1, LVNI_FOCUSED);
			if (lvi.iItem == -1) {
				MessageBox(hwndParent, "Must selected object first.", "Error...", MB_OK);
			} else {
				if (MessageBox(hwndParent, "Are you sure you wish to delete this?", "Confirm", MB_YESNO) == IDYES) {
					ListView_GetItem(hwndObjects, &lvi);

					pobj = (OBJECT*)lvi.lParam;

					Objects_Delete(pobj);
					Objects_Save();

					ListView_DeleteItem(hwndObjects, lvi.iItem);
				}

			}


			MainPage3_Update();
			bReturn = TRUE;
			break;

		case IDC_DEFINE:

			hwndObjects = GetDlgItem(hwndParent, IDC_OBJECTS);

			lvi.mask = LVIF_PARAM;
			lvi.iSubItem = 0;	
			lvi.iItem = ListView_GetNextItem(hwndObjects, -1, LVNI_FOCUSED);
			if (lvi.iItem == -1) {
				MessageBox(hwndParent, "Must selected object to edit.", "Error...", MB_OK);
			} else {
				ListView_GetItem(hwndObjects, &lvi);

				pobj = (OBJECT*)lvi.lParam;

				pobjdef = ObjectDefinitions_Find(pobj->szUniqueName);

				if (!pobjdef) {
					pobjdef = ObjectDefinition_Create();
					strcpy(pobjdef->szUniqueName, pobj->szUniqueName);
					bTemp = TRUE;
				}

				if (ObjectDefinitionEditor_Run(hwndParent, pobjdef)) {
					if (g_bLoggedIn) {
						if (bTemp) {
							ObjectDefinitions_Add(pobjdef);
						}

						ObjectDefinitions_Save();
					
						PostThreadMessage(UltimaOnline_Data.dwThreadId, UOEM_OBJECTDEFINITIONSCHANGED, (WPARAM)0, (LPARAM)0);
					} else {
						if (bTemp) {
							ObjectDefinition_Destroy(pobjdef);
						}
					}

				} else {
					if (bTemp) {
						ObjectDefinition_Destroy(pobjdef);
					}
				}
			}
			MainPage3_Update();
			bReturn = TRUE;
			break;
		case IDC_UNDEFINE:
			hwndObjects = GetDlgItem(hwndParent, IDC_OBJECTS);

			lvi.mask = LVIF_PARAM;
			lvi.iSubItem = 0;	
			lvi.iItem = ListView_GetNextItem(hwndObjects, -1, LVNI_FOCUSED);
			if (lvi.iItem == -1) {
				MessageBox(hwndParent, "Must selected object to edit.", "Error...", MB_OK);
			} else {
				if (MessageBox(hwndParent, "Are you sure you wish to clear this?", "Confirm", MB_YESNO) == IDYES) {
					ListView_GetItem(hwndObjects, &lvi);

					pobj = (OBJECT*)lvi.lParam;

					pobjdef = ObjectDefinitions_Find(pobj->szUniqueName);

					ObjectDefinitions_Delete(pobjdef);
					ObjectDefinition_Destroy(pobjdef);

					ObjectDefinitions_Save();
				
					PostThreadMessage(UltimaOnline_Data.dwThreadId, UOEM_OBJECTDEFINITIONSCHANGED, (WPARAM)0, (LPARAM)0);
				}
			}

			MainPage3_Update();
			bReturn = TRUE;
			break;


		}
		break;

	}

	return bReturn;
}

BOOL CALLBACK MainPage3_DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	BOOL bReturn = FALSE;
	HWND hwndObjects = 0;
	LV_COLUMN lvc = { 0 };
	OBJECT* pobj;
	LV_ITEM lvi;



	switch (uMsg) {
	case WM_INITDIALOG:
		g_hwndMainPage3 = hwnd;
		hwndObjects = GetDlgItem(hwnd, IDC_OBJECTS);

		lvc.mask = LVCF_TEXT | LVCF_WIDTH;
		lvc.pszText = "Object Name";
		lvc.cx = 107;
		ListView_InsertColumn(hwndObjects, 0, &lvc);
		lvc.pszText = "Active";
		lvc.cx = 45;
		ListView_InsertColumn(hwndObjects, 1, &lvc);
		lvc.pszText = "Object Description";
		lvc.cx = 120;
		ListView_InsertColumn(hwndObjects, 2, &lvc);


		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iItem = 0;
		lvi.iSubItem = 0;

		pobj = Objects_GetFirst();
		while (pobj != NULL) {
			lvi.pszText = pobj->szUniqueName;
			lvi.lParam = (LPARAM)pobj;
			ListView_InsertItem(hwndObjects, &lvi);

			lvi.iItem++;
			pobj = Objects_GetNext(pobj);
		}

		MainPage3_Update();
		break;
	case WM_COMMAND:
		bReturn = MainPage3_OnCommand(hwnd, (HWND)lParam, HIWORD(wParam), LOWORD(wParam));
		break;

	}

	return bReturn;
}
