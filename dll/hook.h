
PROC WINAPI HookImportedFunction(HMODULE hFromModule, PSTR pszFunctionModule, PSTR pszFunctionName, PROC pfnNewProc);
PROC WINAPI UnhookImportedFunction(HMODULE hFromModule, PSTR pszFunctionModule, PROC pfnHookProc, PROC pfnOriginalProc);