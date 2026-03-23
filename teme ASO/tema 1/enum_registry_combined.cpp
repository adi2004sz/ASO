#include <windows.h>
#include <stdio.h>
#include <tchar.h>

// Variante de chei din Registry
typedef struct {
    HKEY hKeyRoot;
    LPCTSTR pszPath;
    LPCTSTR pszDisplay;
} REGISTRY_KEY;

void EnumerateSubkeys(HKEY hKeyRoot, LPCTSTR pszPath, LPCTSTR pszDisplay) {
    HKEY hKey;
    DWORD dwResult;
    DWORD dwIndex = 0;
    TCHAR szSubKeyName[256];
    DWORD dwSubKeyNameSize;

    // Deschidem cheia Registry
    dwResult = RegOpenKeyEx(
        hKeyRoot,
        pszPath,
        0,
        KEY_READ,
        &hKey
    );

    if (dwResult != ERROR_SUCCESS) {
        _tprintf(TEXT("Eroare la deschiderea cheii: %ld\n"), dwResult);
        return;
    }

    _tprintf(TEXT("\n"));
    _tprintf(TEXT("=====================================\n"));
    _tprintf(TEXT("Subcheile din: %s\n"), pszDisplay);
    _tprintf(TEXT("=====================================\n\n"));

    // Enumerarea subcheilor
    DWORD dwCount = 0;
    while (TRUE) {
        dwSubKeyNameSize = sizeof(szSubKeyName) / sizeof(TCHAR);

        dwResult = RegEnumKeyEx(
            hKey,
            dwIndex,
            szSubKeyName,
            &dwSubKeyNameSize,
            NULL,
            NULL,
            NULL,
            NULL
        );

        if (dwResult == ERROR_NO_MORE_ITEMS) {
            break;
        }

        if (dwResult == ERROR_SUCCESS) {
            _tprintf(TEXT("%lu. %s\n"), dwIndex + 1, szSubKeyName);
            dwCount++;
        }
        else {
            _tprintf(TEXT("Eroare la enumerare (index %lu): %ld\n"), dwIndex, dwResult);
            break;
        }

        dwIndex++;
    }

    _tprintf(TEXT("\n=====================================\n"));
    _tprintf(TEXT("Total: %lu subchei\n"), dwCount);
    _tprintf(TEXT("=====================================\n"));

    RegCloseKey(hKey);
}

int main() {
    REGISTRY_KEY arrKeys[] = {
        { HKEY_CURRENT_USER, TEXT("Software"), TEXT("HKCU\\Software") },
        { HKEY_CURRENT_USER, TEXT("Control Panel\\Desktop"), TEXT("HKCU\\Control Panel\\Desktop") },
        { HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft"), TEXT("HKLM\\Software\\Microsoft") },
        { HKEY_LOCAL_MACHINE, TEXT("System\\CurrentControlSet\\Services"), TEXT("HKLM\\System\\CurrentControlSet\\Services") },
        { HKEY_LOCAL_MACHINE, TEXT("Hardware"), TEXT("HKLM\\Hardware") }
    };

    int nKeyCount = sizeof(arrKeys) / sizeof(REGISTRY_KEY);

    _tprintf(TEXT("=====================================\n"));
    _tprintf(TEXT("ENUMERATOR SUBCHEI REGISTRY - ASO\n"));
    _tprintf(TEXT("=====================================\n\n"));
    _tprintf(TEXT("Alege o cheie din Registry:\n\n"));

    for (int i = 0; i < nKeyCount; i++) {
        _tprintf(TEXT("%d. %s\n"), i + 1, arrKeys[i].pszDisplay);
    }

    _tprintf(TEXT("\nIntrodu optiunea (1-%d): "), nKeyCount);

    int nChoice;
    _tscanf_s(TEXT("%d"), &nChoice);

    if (nChoice < 1 || nChoice > nKeyCount) {
        _tprintf(TEXT("Optiune invalida!\n"));
        return 1;
    }

    // Executie enumerare pentru cheia selectata
    int nIndex = nChoice - 1;
    EnumerateSubkeys(
        arrKeys[nIndex].hKeyRoot,
        arrKeys[nIndex].pszPath,
        arrKeys[nIndex].pszDisplay
    );

    return 0;
}
