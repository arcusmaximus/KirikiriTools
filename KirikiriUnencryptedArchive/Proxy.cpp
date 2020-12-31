#include "stdafx.h"

struct version_dll {
	FARPROC OriginalGetFileVersionInfoA;
	FARPROC OriginalGetFileVersionInfoByHandle;
	FARPROC OriginalGetFileVersionInfoExA;
	FARPROC OriginalGetFileVersionInfoExW;
	FARPROC OriginalGetFileVersionInfoSizeA;
	FARPROC OriginalGetFileVersionInfoSizeExA;
	FARPROC OriginalGetFileVersionInfoSizeExW;
	FARPROC OriginalGetFileVersionInfoSizeW;
	FARPROC OriginalGetFileVersionInfoW;
	FARPROC OriginalVerFindFileA;
	FARPROC OriginalVerFindFileW;
	FARPROC OriginalVerInstallFileA;
	FARPROC OriginalVerInstallFileW;
	FARPROC OriginalVerLanguageNameA;
	FARPROC OriginalVerLanguageNameW;
	FARPROC OriginalVerQueryValueA;
	FARPROC OriginalVerQueryValueW;
} version;

void Proxy::Init()
{
	wchar_t path[MAX_PATH];
	GetSystemDirectory(path, MAX_PATH);
	wcscat_s(path, L"\\version.dll");
	HMODULE hDll = LoadLibrary(path);
	if (hDll == nullptr)
	{
		MessageBox(nullptr, L"Cannot load original version.dll library", L"KirikiriUnencryptedArchive", MB_ICONERROR);
		ExitProcess(0);
	}
	version.OriginalGetFileVersionInfoA = GetProcAddress(hDll, "GetFileVersionInfoA");
	version.OriginalGetFileVersionInfoByHandle = GetProcAddress(hDll, "GetFileVersionInfoByHandle");
	version.OriginalGetFileVersionInfoExA = GetProcAddress(hDll, "GetFileVersionInfoExA");
	version.OriginalGetFileVersionInfoExW = GetProcAddress(hDll, "GetFileVersionInfoExW");
	version.OriginalGetFileVersionInfoSizeA = GetProcAddress(hDll, "GetFileVersionInfoSizeA");
	version.OriginalGetFileVersionInfoSizeExA = GetProcAddress(hDll, "GetFileVersionInfoSizeExA");
	version.OriginalGetFileVersionInfoSizeExW = GetProcAddress(hDll, "GetFileVersionInfoSizeExW");
	version.OriginalGetFileVersionInfoSizeW = GetProcAddress(hDll, "GetFileVersionInfoSizeW");
	version.OriginalGetFileVersionInfoW = GetProcAddress(hDll, "GetFileVersionInfoW");
	version.OriginalVerFindFileA = GetProcAddress(hDll, "VerFindFileA");
	version.OriginalVerFindFileW = GetProcAddress(hDll, "VerFindFileW");
	version.OriginalVerInstallFileA = GetProcAddress(hDll, "VerInstallFileA");
	version.OriginalVerInstallFileW = GetProcAddress(hDll, "VerInstallFileW");
	version.OriginalVerLanguageNameA = GetProcAddress(hDll, "VerLanguageNameA");
	version.OriginalVerLanguageNameW = GetProcAddress(hDll, "VerLanguageNameW");
	version.OriginalVerQueryValueA = GetProcAddress(hDll, "VerQueryValueA");
	version.OriginalVerQueryValueW = GetProcAddress(hDll, "VerQueryValueW");
}

__declspec(naked) void FakeGetFileVersionInfoA() { _asm { jmp[version.OriginalGetFileVersionInfoA] } }
__declspec(naked) void FakeGetFileVersionInfoByHandle() { _asm { jmp[version.OriginalGetFileVersionInfoByHandle] } }
__declspec(naked) void FakeGetFileVersionInfoExA() { _asm { jmp[version.OriginalGetFileVersionInfoExA] } }
__declspec(naked) void FakeGetFileVersionInfoExW() { _asm { jmp[version.OriginalGetFileVersionInfoExW] } }
__declspec(naked) void FakeGetFileVersionInfoSizeA() { _asm { jmp[version.OriginalGetFileVersionInfoSizeA] } }
__declspec(naked) void FakeGetFileVersionInfoSizeExA() { _asm { jmp[version.OriginalGetFileVersionInfoSizeExA] } }
__declspec(naked) void FakeGetFileVersionInfoSizeExW() { _asm { jmp[version.OriginalGetFileVersionInfoSizeExW] } }
__declspec(naked) void FakeGetFileVersionInfoSizeW() { _asm { jmp[version.OriginalGetFileVersionInfoSizeW] } }
__declspec(naked) void FakeGetFileVersionInfoW() { _asm { jmp[version.OriginalGetFileVersionInfoW] } }
__declspec(naked) void FakeVerFindFileA() { _asm { jmp[version.OriginalVerFindFileA] } }
__declspec(naked) void FakeVerFindFileW() { _asm { jmp[version.OriginalVerFindFileW] } }
__declspec(naked) void FakeVerInstallFileA() { _asm { jmp[version.OriginalVerInstallFileA] } }
__declspec(naked) void FakeVerInstallFileW() { _asm { jmp[version.OriginalVerInstallFileW] } }
__declspec(naked) void FakeVerLanguageNameA() { _asm { jmp[version.OriginalVerLanguageNameA] } }
__declspec(naked) void FakeVerLanguageNameW() { _asm { jmp[version.OriginalVerLanguageNameW] } }
__declspec(naked) void FakeVerQueryValueA() { _asm { jmp[version.OriginalVerQueryValueA] } }
__declspec(naked) void FakeVerQueryValueW() { _asm { jmp[version.OriginalVerQueryValueW] } }
