#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "..\dll\uoehelper.h"
#include "uoextreme.h"
#include "ObjectDefinitionEditor.h"


HANDLE g_heventGetObjectId;
HANDLE g_heventGotObjectId;
HANDLE g_heventGotObjectName;

OBJECTDEFINITION* g_pobjdef;

void ObjectDefinitionEditor_Validate(HWND hwnd) {
	char szText[1024];

	GetWindowText(GetDlgItem(hwnd, IDC_OBJECT_DESCRIPTION), szText, sizeof(szText));

	EnableWindow(GetDlgItem(hwnd, IDOK), strlen(szText) != 0);

	EnableWindow(GetDlgItem(hwnd, IDC_USEANY), g_bLoggedIn);
	EnableWindow(GetDlgItem(hwnd, IDC_CHECKNESTED), IsDlgButtonChecked(hwnd, IDC_USEANY) && g_bLoggedIn);


}


BOOL ObjectDefinitionEditor_OnCommand(HWND hwndParent, HWND hwnd, WORD uCode, WORD wID) {
	MSG msg = { 0 };
	BOOL bReturn = FALSE;
	char szText[1024];
	static BOOL bQuitChecking = FALSE;

	switch (uCode) {
	case EN_CHANGE:
		switch (wID) {
		case IDC_OBJECT_DESCRIPTION:
			ObjectDefinitionEditor_Validate(hwndParent);
			break;
		}
		break;
	case BN_CLICKED:
		switch (wID) {
		case IDC_USEANY:
			ObjectDefinitionEditor_Validate(hwndParent);

			break;
		case IDC_CHECKNESTED:
			break;
		case IDC_RECORD:
			EnableWindow(hwnd, FALSE);

			bQuitChecking = FALSE;

			ResetEvent(g_heventGetObjectId);
			ResetEvent(g_heventGotObjectId);
			ResetEvent(g_heventGotObjectName);

			SetDlgItemText(hwndParent, IDC_OBJECT_ID, "????????");
			SetDlgItemText(hwndParent, IDC_OBJECT_TYPE, "????");
			SetDlgItemText(hwndParent, IDC_OBJECT_NAME, "Unknown");

			g_bDisableSendCommand = TRUE;

			SetEvent(g_heventGetObjectId);
			do {
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			} while (!bQuitChecking && WaitForSingleObject(g_heventGotObjectId, 0) == WAIT_TIMEOUT);

			g_bDisableSendCommand = FALSE;

			if (!bQuitChecking) {
				wsprintf(szText, "%08x", g_GetObject_dwObjectId);
				SetDlgItemText(hwndParent, IDC_OBJECT_ID, szText);


				PostThreadMessage(UltimaOnline_Data.dwThreadId, UOEM_GETOBJECTNAME, (WPARAM)0, (LPARAM)0);
				do {
					if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				} while (!bQuitChecking && WaitForSingleObject(g_heventGotObjectName, 0) == WAIT_TIMEOUT);

				if (!bQuitChecking) {
					SetDlgItemText(hwndParent, IDC_OBJECT_NAME, g_GetObject_szObjectName);

					wsprintf(szText, "%04x", g_GetObject_wObjectType);
					SetDlgItemText(hwndParent, IDC_OBJECT_TYPE, szText);

					GetDlgItemText(hwndParent, IDC_OBJECT_DESCRIPTION, szText, sizeof(szText));
					if (strlen(szText) == 0) {
						SetDlgItemText(hwndParent, IDC_OBJECT_DESCRIPTION, g_GetObject_szObjectName);
					}
					EnableWindow(hwnd, TRUE);
				}
			
			}
			break;
		case IDOK:

			GetDlgItemText(hwndParent, IDC_OBJECT_DESCRIPTION, g_pobjdef->szUniqueName, sizeof(g_pobjdef->szUniqueName));
			strcpy(g_pobjdef->szName, g_GetObject_szObjectName);
			g_pobjdef->dwId = g_GetObject_dwObjectId;
			g_pobjdef->wType = g_GetObject_wObjectType;
			g_pobjdef->bUseAny = IsDlgButtonChecked(hwndParent, IDC_USEANY);
			g_pobjdef->bCheckNested = IsDlgButtonChecked(hwndParent, IDC_CHECKNESTED);

			bQuitChecking = TRUE;
			EndDialog(hwndParent, IDOK);
			break;
		case IDCANCEL:
			bQuitChecking = TRUE;
			EndDialog(hwndParent, IDCANCEL);
			break;

		}
		break;

	}

	return bReturn;

}

BOOL CALLBACK ObjectDefinitionEditor_DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	BOOL bReturn = FALSE;
	char szText[1024];

	switch (uMsg) {
	case WM_INITDIALOG:

//		EnableWindow(GetDlgItem(hwnd, IDC_RECORD), g_bLoggedIn);

		SetDlgItemText(hwnd, IDC_OBJECT_DESCRIPTION, g_pobjdef->szUniqueName);

		SetDlgItemText(hwnd, IDC_OBJECT_NAME, g_pobjdef->szName);

		wsprintf(szText, "%08x", g_pobjdef->dwId);
		SetDlgItemText(hwnd, IDC_OBJECT_ID, szText);

		wsprintf(szText, "%04x", g_pobjdef->wType);
		SetDlgItemText(hwnd, IDC_OBJECT_TYPE, szText);

		CheckDlgButton(hwnd, IDC_USEANY, g_pobjdef->bUseAny);
		CheckDlgButton(hwnd, IDC_CHECKNESTED, g_pobjdef->bCheckNested);


		ObjectDefinitionEditor_Validate(hwnd);

		break;
	case WM_COMMAND:
		bReturn = ObjectDefinitionEditor_OnCommand(hwnd, (HWND)lParam, HIWORD(wParam), LOWORD(wParam));
		break;

	}

	return bReturn;
}

void ObjectDefinitionEditor_Initialize(void) {
	g_heventGetObjectId = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_heventGotObjectId = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_heventGotObjectName = CreateEvent(NULL, FALSE, FALSE, "g_heventGotObjectName");

}

BOOL ObjectDefinitionEditor_Run(HWND hwnd, OBJECTDEFINITION* pobj) {

	g_pobjdef = pobj;

	if (DialogBox(g_hinst, MAKEINTRESOURCE(IDD_OBJECT_DEFINITION_EDITOR), hwnd, ObjectDefinitionEditor_DlgProc) == IDOK) {

		return TRUE;
	}

	return FALSE;
}

