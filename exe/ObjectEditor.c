#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "uoextreme.h"
#include "..\dll\uoehelper.h"
#include "ObjectEditor.h"

OBJECT* g_pobj;

void ObjectEditor_Validate(HWND hwnd) {
	char szText[1024];

	GetWindowText(GetDlgItem(hwnd, IDC_OBJECT_NAME), szText, sizeof(szText));

	EnableWindow(GetDlgItem(hwnd, IDOK), strlen(szText) != 0);


}



BOOL ObjectEditor_OnCommand(HWND hwndParent, HWND hwnd, WORD uCode, WORD wID) {
	BOOL bReturn = FALSE;

	switch (uCode) {
	case EN_CHANGE:
		switch (wID) {
		case IDC_OBJECT_NAME:
			ObjectEditor_Validate(hwndParent);
			break;
		}
		break;
	case BN_CLICKED:
		switch (wID) {
		case IDOK:

			GetDlgItemText(hwndParent, IDC_OBJECT_NAME, g_pobj->szUniqueName, sizeof(g_pobj->szUniqueName));

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


BOOL CALLBACK ObjectEditor_DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	BOOL bReturn = FALSE;

	switch (uMsg) {
	case WM_INITDIALOG:
		ObjectEditor_Validate(hwnd);

		break;
	case WM_COMMAND:
		bReturn = ObjectEditor_OnCommand(hwnd, (HWND)lParam, HIWORD(wParam), LOWORD(wParam));
		break;

	}

	return bReturn;
}


BOOL ObjectEditor_Run(HWND hwnd, OBJECT* pobj) {

	g_pobj = pobj;

	if (DialogBox(g_hinst, MAKEINTRESOURCE(IDD_OBJECT_EDITOR), hwnd, ObjectEditor_DlgProc) == IDOK) {

		return TRUE;
	}

	return FALSE;
}

