#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "..\dll\uoehelper.h"
#include "debug.h"
#include "MainPage1.h"
#include "MainPage2.h"
#include "MainPage3.h"
#include "ObjectDefinitionEditor.h"
#include "MacroCommandEditor.h"
#include "MacroEditor.h"
#include "UoExtreme.h"

HINSTANCE g_hinst;
//DWORD g_dwUltimaOnlineThreadId;
WNDPROC g_pfnMainWndProc;
HWND g_hwndMain;

HANDLE g_heventTargetCursor;

char g_szClientFileName[256];

void Extreme_Begin(char* pszCharacterName) {
	strcpy(UltimaOnline_Data.Character.szName, pszCharacterName);
	ResetEvent(g_heventTargetCursor);
	ObjectDefinitions_Load();
	g_bLoggedIn = TRUE;
	MainPage3_Update();
	PostThreadMessage(UltimaOnline_Data.dwThreadId, UOEM_BEGIN, (WPARAM)0, (LPARAM)0);
	PostThreadMessage(UltimaOnline_Data.dwThreadId, UOEM_MACROSCHANGED, (WPARAM)0, (LPARAM)0);
	PostThreadMessage(UltimaOnline_Data.dwThreadId, UOEM_OBJECTDEFINITIONSCHANGED, (WPARAM)0, (LPARAM)0);

}

void Extreme_End(void) {
	strcpy(UltimaOnline_Data.Character.szName, "");
	g_bLoggedIn = FALSE;
	UltimaOnline_Data.TargetCursor.dwId = 0;
	ObjectDefinitions_RemoveAll();
	MainPage3_Update();
	PostThreadMessage(UltimaOnline_Data.dwThreadId, UOEM_END, (WPARAM)0, (LPARAM)0);
	
}



void Options_Load(void) {
	FILE* fp;

	strcpy(g_szClientFileName, "c:\\games\\uo\\client.exe");

	UltimaOnline_Data.Options.bShowNames = TRUE;
	UltimaOnline_Data.Options.bFastWalk = FALSE;
	UltimaOnline_Data.Filters.bBardMusic = TRUE;
	UltimaOnline_Data.Filters.bSpiritSpeakSound = TRUE;
	UltimaOnline_Data.Filters.bGuildNames = FALSE;
	UltimaOnline_Data.Filters.bGuildTitlesOnly = FALSE;
	UltimaOnline_Data.Filters.bHiddenPlayers = FALSE;
	UltimaOnline_Data.Filters.bHiddenPlayerMessages = TRUE;
	UltimaOnline_Data.Filters.bNonHumans = TRUE;
	UltimaOnline_Data.Filters.bWeather = TRUE;
	UltimaOnline_Data.Options.bAlwaysLight = TRUE;
	UltimaOnline_Data.Options.bPreventAutoClose = FALSE;

	fp = fopen("options.txt", "rt");
	if (fp) {
		fgets(g_szClientFileName, sizeof(g_szClientFileName), fp);
		g_szClientFileName[strlen(g_szClientFileName) - 1] = 0;
		fscanf(fp, "%d\n", &UltimaOnline_Data.Options.bShowNames);
		fscanf(fp, "%d\n", &UltimaOnline_Data.Options.bFastWalk);
		fscanf(fp, "%d\n", &UltimaOnline_Data.Filters.bBardMusic);
		fscanf(fp, "%d\n", &UltimaOnline_Data.Filters.bSpiritSpeakSound);
		fscanf(fp, "%d\n", &UltimaOnline_Data.Filters.bGuildNames);
		fscanf(fp, "%d\n", &UltimaOnline_Data.Filters.bGuildTitlesOnly);
		fscanf(fp, "%d\n", &UltimaOnline_Data.Filters.bHiddenPlayers);
		fscanf(fp, "%d\n", &UltimaOnline_Data.Filters.bHiddenPlayerMessages);
		fscanf(fp, "%d\n", &UltimaOnline_Data.Filters.bNonHumans);
		fscanf(fp, "%d\n", &UltimaOnline_Data.Filters.bWeather);
		fscanf(fp, "%d\n", &UltimaOnline_Data.Options.bAlwaysLight);
		fscanf(fp, "%d\n", &UltimaOnline_Data.Options.bPreventAutoClose);
		fclose(fp);
	} 

}

void Options_Save(void) {
	FILE* fp;

	fp = fopen("options.txt", "wt");
	if (fp) {
		fprintf(fp, "%s\n", g_szClientFileName);
		fprintf(fp, "%d\n", UltimaOnline_Data.Options.bShowNames);
		fprintf(fp, "%d\n", UltimaOnline_Data.Options.bFastWalk);
		fprintf(fp, "%d\n", UltimaOnline_Data.Filters.bBardMusic);
		fprintf(fp, "%d\n", UltimaOnline_Data.Filters.bSpiritSpeakSound);
		fprintf(fp, "%d\n", UltimaOnline_Data.Filters.bGuildNames);
		fprintf(fp, "%d\n", UltimaOnline_Data.Filters.bGuildTitlesOnly);
		fprintf(fp, "%d\n", UltimaOnline_Data.Filters.bHiddenPlayers);
		fprintf(fp, "%d\n", UltimaOnline_Data.Filters.bHiddenPlayerMessages);
		fprintf(fp, "%d\n", UltimaOnline_Data.Filters.bNonHumans);
		fprintf(fp, "%d\n", UltimaOnline_Data.Filters.bWeather);
		fprintf(fp, "%d\n", UltimaOnline_Data.Options.bAlwaysLight);
		fprintf(fp, "%d\n", UltimaOnline_Data.Options.bPreventAutoClose);		
		fclose(fp);
	} 
}

LRESULT CALLBACK Main_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static HANDLE hthread;
	DWORD dwThreadId;

	switch (uMsg) {
	case WM_COMMAND:
		switch (HIWORD(wParam)) {
		case BN_CLICKED:
			switch (LOWORD(wParam)) {
			case IDC_LAUNCH:
				hthread = CreateThread(NULL, 0, (PTHREAD_START_ROUTINE)threadMonitorDebugEvents, g_szClientFileName, 0, &dwThreadId);

				return 0;
			default:
				return 0;
			}
			break;
		}
		break;
	case WM_SYSCOMMAND:
		switch (wParam) {
		case SC_CLOSE:
			if (WaitForSingleObject(hthread, 0) == WAIT_TIMEOUT) {
				return 0;
			}
		}
		break;
	}

	return CallWindowProc(g_pfnMainWndProc, hwnd, uMsg, wParam, lParam);

}

int CALLBACK Main_PropSheetProc(HWND hwnd, UINT uMsg, LPARAM lParam) {
	RECT rc;

	switch (uMsg) {
	case PSCB_PRECREATE:
		((DLGTEMPLATE*)lParam)->style &= ~DS_CONTEXTHELP;
		((DLGTEMPLATE*)lParam)->style |= WS_MINIMIZEBOX;
		((DLGTEMPLATE*)lParam)->dwExtendedStyle &= ~WS_EX_CONTEXTHELP;
		//((DLGTEMPLATE*)lParam)->x = 20;
		//((DLGTEMPLATE*)lParam)->y = 20;
		//((DLGTEMPLATE*)lParam)->cy = 122;
		break;
	case PSCB_INITIALIZED:
		DestroyWindow(GetDlgItem(hwnd, IDOK));
		DestroyWindow(GetDlgItem(hwnd, IDCANCEL));

		rc.top = 173;
		rc.left = 43;
		rc.bottom = rc.top + 18;
		rc.right = rc.left + 136;
		MapDialogRect(hwnd, &rc);


		CreateWindow("BUTTON", "Launch Ultima Online", WS_CHILD| WS_VISIBLE, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, hwnd, (HMENU)IDC_LAUNCH, g_hinst, NULL);
		g_pfnMainWndProc = (WNDPROC)SetWindowLong(hwnd, GWL_WNDPROC, (LONG)Main_WndProc);
		g_hwndMain = hwnd;
		break;
	}

	return 0;
}
 


int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, PSTR pszCommand, int iShowCmd) {
	PROPSHEETHEADER psh = { 0 };
	PROPSHEETPAGE psp[3] = { 0 };

	g_hinst = hinst;

	psp[0].dwSize = sizeof(PROPSHEETPAGE);
	psp[0].dwFlags = 0;
	psp[0].hInstance = hinst;
	psp[0].pszTemplate = MAKEINTRESOURCE(IDD_MAIN_PAGE1);
	psp[0].pfnDlgProc = MainPage1_DlgProc;

	psp[1].dwSize = sizeof(PROPSHEETPAGE);
	psp[1].dwFlags = 0;
	psp[1].hInstance = hinst;
	psp[1].pszTemplate = MAKEINTRESOURCE(IDD_MAIN_PAGE2);
	psp[1].pfnDlgProc = MainPage2_DlgProc;

	psp[2].dwSize = sizeof(PROPSHEETPAGE);
	psp[2].dwFlags = 0;
	psp[2].hInstance = hinst;
	psp[2].pszTemplate = MAKEINTRESOURCE(IDD_MAIN_PAGE3);
	psp[2].pfnDlgProc = MainPage3_DlgProc;

	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_NOAPPLYNOW | PSH_PROPSHEETPAGE | PSH_USECALLBACK | PSH_USEICONID;
	psh.hwndParent = NULL;
	psh.pszCaption = "UO Extreme - Release 4";
	psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
	psh.nStartPage = 0;
	psh.hInstance = hinst;
	psh.ppsp = psp;
	psh.pfnCallback = Main_PropSheetProc;
	psh.pszIcon = MAKEINTRESOURCE(IDI_MAIN);

	g_heventTargetCursor = CreateEvent(NULL, TRUE, FALSE, "g_heventTargetCursor");

	Options_Load();

	ObjectDefinitions_Initialize();
	Objects_Initialize();
	ObjectDefinitionEditor_Initialize();

	Macros_Initialize();
	MacroCommandEditor_Initialize();
	MacroEditor_Initialize();

	Objects_Load();
	Macros_Load();



	return PropertySheet(&psh); 
    /*STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

    si.cb = sizeof(STARTUPINFO);
	CreateProcess(NULL, "c:\\games\\uo\\client.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

		
	do {
		g_hhook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, GetModuleHandle("uoehelper.dll"), pi.dwThreadId);
		Sleep(500);
	} while (!g_hhook);

	PostThreadMessage(pi.dwThreadId, UOED_SEND, (WPARAM)0, (LPARAM)0);

	Sleep(INFINITE);

	return 0;*/
}
