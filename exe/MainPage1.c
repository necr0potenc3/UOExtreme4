#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "..\dll\uoehelper.h"
#include "uoextreme.h"
#include "MainPage1.h"


BOOL MainPage1_OnCommand(HWND hwndParent, HWND hwnd, WORD uCode, WORD wID) {
	BOOL bReturn = FALSE;

	switch (uCode) {
	case EN_CHANGE:
		switch (wID) {
		case IDC_FILENAME:
			GetDlgItemText(hwndParent, IDC_FILENAME, g_szClientFileName, sizeof(g_szClientFileName));
			Options_Save();
			break;
		}
		break;
	case BN_CLICKED:
		switch (wID) {
		case IDC_SHOWNAMES:
			UltimaOnline_Data.Options.bShowNames = IsDlgButtonChecked(hwndParent, IDC_SHOWNAMES);
			EnableWindow(GetDlgItem(hwndParent, IDC_HUMANSONLY), UltimaOnline_Data.Options.bShowNames);
			Options_Save();
			break;
		case IDC_HUMANSONLY:
			UltimaOnline_Data.Filters.bNonHumans = IsDlgButtonChecked(hwndParent, IDC_HUMANSONLY);
			Options_Save();
			break;
		case IDC_FASTWALK:
			UltimaOnline_Data.Options.bFastWalk = IsDlgButtonChecked(hwndParent, IDC_FASTWALK);
			Options_Save();
			break;
		case IDC_FILTERSPIRITSPEAK:
			UltimaOnline_Data.Filters.bSpiritSpeakSound = IsDlgButtonChecked(hwndParent, IDC_FILTERSPIRITSPEAK);
			Options_Save();
			break;
		case IDC_FILTERBARD:
			UltimaOnline_Data.Filters.bBardMusic = IsDlgButtonChecked(hwndParent, IDC_FILTERBARD);
			Options_Save();
			break;
		case IDC_FILTERGUILDNAMES:
			UltimaOnline_Data.Filters.bGuildNames = IsDlgButtonChecked(hwndParent, IDC_FILTERGUILDNAMES);
			EnableWindow(GetDlgItem(hwndParent, IDC_TITLESONLY), UltimaOnline_Data.Filters.bGuildNames);
			Options_Save();
			break;
		case IDC_TITLESONLY:
			UltimaOnline_Data.Filters.bGuildTitlesOnly = IsDlgButtonChecked(hwndParent, IDC_TITLESONLY);
			Options_Save();
			break;
		case IDC_SHOWHIDDEN:
			UltimaOnline_Data.Filters.bHiddenPlayers = IsDlgButtonChecked(hwndParent, IDC_SHOWHIDDEN);
			EnableWindow(GetDlgItem(hwndParent, IDC_SHOWMESSAGES), UltimaOnline_Data.Filters.bHiddenPlayers);
			Options_Save();
			break;
		case IDC_SHOWMESSAGES:
			UltimaOnline_Data.Filters.bHiddenPlayerMessages = !IsDlgButtonChecked(hwndParent, IDC_SHOWMESSAGES);
			Options_Save();
			break;
		case IDC_ALWAYSLIGHT:
			UltimaOnline_Data.Options.bAlwaysLight = IsDlgButtonChecked(hwndParent, IDC_ALWAYSLIGHT);
			Options_Save();
			break;
		case IDC_FILTERWEATHER:
			UltimaOnline_Data.Filters.bWeather = IsDlgButtonChecked(hwndParent, IDC_FILTERWEATHER);
			Options_Save();
			break;
		case IDC_PREVENTAUTOCLOSE:
			UltimaOnline_Data.Options.bPreventAutoClose = IsDlgButtonChecked(hwndParent, IDC_PREVENTAUTOCLOSE);
			Options_Save();
			break;
		}
		break;

	}

	return bReturn;
}


BOOL CALLBACK MainPage1_DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	BOOL bReturn = FALSE;

	switch (uMsg) {
	case WM_INITDIALOG:
		SetDlgItemText(hwnd, IDC_FILENAME, g_szClientFileName);
		CheckDlgButton(hwnd, IDC_SHOWNAMES, UltimaOnline_Data.Options.bShowNames);
		EnableWindow(GetDlgItem(hwnd, IDC_HUMANSONLY), UltimaOnline_Data.Options.bShowNames);
		CheckDlgButton(hwnd, IDC_HUMANSONLY, UltimaOnline_Data.Filters.bNonHumans);
		CheckDlgButton(hwnd, IDC_FASTWALK, UltimaOnline_Data.Options.bFastWalk);
		CheckDlgButton(hwnd, IDC_FILTERWEATHER, UltimaOnline_Data.Filters.bWeather);
		CheckDlgButton(hwnd, IDC_ALWAYSLIGHT, UltimaOnline_Data.Options.bAlwaysLight);
		CheckDlgButton(hwnd, IDC_SHOWHIDDEN, UltimaOnline_Data.Filters.bHiddenPlayers);
		CheckDlgButton(hwnd, IDC_SHOWMESSAGES, !UltimaOnline_Data.Filters.bHiddenPlayerMessages);
		EnableWindow(GetDlgItem(hwnd, IDC_SHOWMESSAGES), UltimaOnline_Data.Filters.bHiddenPlayers);
		CheckDlgButton(hwnd, IDC_FILTERSPIRITSPEAK, UltimaOnline_Data.Filters.bSpiritSpeakSound);
		CheckDlgButton(hwnd, IDC_FILTERBARD, UltimaOnline_Data.Filters.bBardMusic);
		CheckDlgButton(hwnd, IDC_FILTERGUILDNAMES, UltimaOnline_Data.Filters.bGuildNames);
		CheckDlgButton(hwnd, IDC_TITLESONLY, UltimaOnline_Data.Filters.bGuildTitlesOnly);
		EnableWindow(GetDlgItem(hwnd, IDC_TITLESONLY), UltimaOnline_Data.Filters.bGuildNames);
		CheckDlgButton(hwnd, IDC_PREVENTAUTOCLOSE, UltimaOnline_Data.Options.bPreventAutoClose);		
		break;
	case WM_COMMAND:
		bReturn = MainPage1_OnCommand(hwnd, (HWND)lParam, HIWORD(wParam), LOWORD(wParam));
		break;

	}

	return bReturn;
}
