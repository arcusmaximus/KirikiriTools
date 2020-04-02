#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS
#include <shellapi.h>

using namespace std;

bool IsKirikiriExe(const wstring& filePath)
{
    DWORD handle;
    DWORD versionInfoSize = GetFileVersionInfoSize(filePath.c_str(), &handle);
    if (versionInfoSize == 0)
        return false;

    vector<byte> versionInfo(versionInfoSize);
    if (!GetFileVersionInfo(filePath.c_str(), 0, versionInfo.size(), versionInfo.data()))
        return false;

    void* pLanguages;
    uint languagesSize;
    if (!VerQueryValue(versionInfo.data(), L"\\VarFileInfo\\Translation", &pLanguages, &languagesSize) || languagesSize == 0)
        return false;

    uint language = ((ushort *)pLanguages)[0];
    uint codepage = ((ushort *)pLanguages)[1];
    wstring subBlockName = StringUtil::Format(L"\\StringFileInfo\\%04x%04x\\ProductName", language, codepage);
    void* pProductName;
    uint productNameSize;
    if (!VerQueryValue(versionInfo.data(), subBlockName.c_str(), &pProductName, &productNameSize))
        return false;

    wstring productName((wchar_t *)pProductName, productNameSize);
    return productName.find(L"KIRIKIRI", 0) != string::npos;
}

wstring GetKirikiriExePath()
{
    wstring folderPath = Path::GetModuleFolderPath(nullptr);
    vector<wstring> exePaths = Directory::GetFiles(folderPath, L"*.exe");
    for (wstring& exePath : exePaths)
    {
        if (IsKirikiriExe(exePath))
            return exePath;
    }
    return wstring();
}

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* pszCmdLine, int cmdShow)
{
    int numArgs;
    wstring exePath;
    PWSTR *ppszArgs = CommandLineToArgvW(pszCmdLine, &numArgs);

    if (numArgs == 1)
    {
        exePath = ppszArgs[0];
    } 
    else
    {
        exePath = GetKirikiriExePath();
    }

    if (exePath.empty())
    {
        MessageBox(nullptr, L"No Kirikiri .exe found.", L"", MB_ICONEXCLAMATION);
        return 0;
    }

    // For Steam games, uncomment the lines below and replace the "1234" by the game's AppID (the number in the store page URL).
    //SetEnvironmentVariable(L"SteamAppId", L"1234");
    //SetEnvironmentVariable(L"SteamGameId", L"1234");

    STARTUPINFO startupInfo;
    memset(&startupInfo, 0, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);

    PROCESS_INFORMATION procInfo;

    DetourCreateProcessWithDllEx(
        exePath.c_str(),
        nullptr,
        nullptr,
        nullptr,
        false,
        0,
        nullptr,
        nullptr,
        &startupInfo,
        &procInfo,
        "plugin\\1KirikiriUnencryptedArchive.tpm",
        nullptr
    );
    CloseHandle(procInfo.hProcess);
    CloseHandle(procInfo.hThread);
    if (ppszArgs)
    {
        LocalFree(ppszArgs);
    }

    return 0;
}
