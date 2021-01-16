#include "stdafx.h"

void Proxy::Init()
{
	wchar_t realDllPath[MAX_PATH];
	GetSystemDirectory(realDllPath, MAX_PATH);
	wcscat_s(realDllPath, L"\\version.dll");
	HMODULE hDll = LoadLibrary(realDllPath);
	if (hDll == nullptr)
	{
		MessageBox(nullptr, L"Cannot load original version.dll library", L"KirikiriUnencryptedArchive", MB_ICONERROR);
		ExitProcess(0);
	}

#define RESOLVE(fn) Original##fn = reinterpret_cast<decltype(Original##fn)>(GetProcAddress(hDll, #fn))
	RESOLVE(GetFileVersionInfoA);
	RESOLVE(GetFileVersionInfoByHandle);
	RESOLVE(GetFileVersionInfoExA);
	RESOLVE(GetFileVersionInfoExW);
	RESOLVE(GetFileVersionInfoSizeA);
	RESOLVE(GetFileVersionInfoSizeExA);
	RESOLVE(GetFileVersionInfoSizeExW);
	RESOLVE(GetFileVersionInfoSizeW);
	RESOLVE(GetFileVersionInfoW);
	RESOLVE(VerFindFileA);
	RESOLVE(VerFindFileW);
	RESOLVE(VerInstallFileA);
	RESOLVE(VerInstallFileW);
	RESOLVE(VerLanguageNameA);
	RESOLVE(VerLanguageNameW);
	RESOLVE(VerQueryValueA);
	RESOLVE(VerQueryValueW);
#undef RESOLVE
}

__declspec(naked) void FakeGetFileVersionInfoA()		{ __asm { jmp [Proxy::OriginalGetFileVersionInfoA] } }
__declspec(naked) void FakeGetFileVersionInfoByHandle() { __asm { jmp [Proxy::OriginalGetFileVersionInfoByHandle] } }
__declspec(naked) void FakeGetFileVersionInfoExA()		{ __asm { jmp [Proxy::OriginalGetFileVersionInfoExA] } }
__declspec(naked) void FakeGetFileVersionInfoExW()		{ __asm { jmp [Proxy::OriginalGetFileVersionInfoExW] } }
__declspec(naked) void FakeGetFileVersionInfoSizeA()	{ __asm { jmp [Proxy::OriginalGetFileVersionInfoSizeA] } }
__declspec(naked) void FakeGetFileVersionInfoSizeExA()	{ __asm { jmp [Proxy::OriginalGetFileVersionInfoSizeExA] } }
__declspec(naked) void FakeGetFileVersionInfoSizeExW()	{ __asm { jmp [Proxy::OriginalGetFileVersionInfoSizeExW] } }
__declspec(naked) void FakeGetFileVersionInfoSizeW()	{ __asm { jmp [Proxy::OriginalGetFileVersionInfoSizeW] } }
__declspec(naked) void FakeGetFileVersionInfoW()		{ __asm { jmp [Proxy::OriginalGetFileVersionInfoW] } }
__declspec(naked) void FakeVerFindFileA()				{ __asm { jmp [Proxy::OriginalVerFindFileA] } }
__declspec(naked) void FakeVerFindFileW()				{ __asm { jmp [Proxy::OriginalVerFindFileW] } }
__declspec(naked) void FakeVerInstallFileA()			{ __asm { jmp [Proxy::OriginalVerInstallFileA] } }
__declspec(naked) void FakeVerInstallFileW()			{ __asm { jmp [Proxy::OriginalVerInstallFileW] } }
__declspec(naked) void FakeVerLanguageNameA()			{ __asm { jmp [Proxy::OriginalVerLanguageNameA] } }
__declspec(naked) void FakeVerLanguageNameW()			{ __asm { jmp [Proxy::OriginalVerLanguageNameW] } }
__declspec(naked) void FakeVerQueryValueA()				{ __asm { jmp [Proxy::OriginalVerQueryValueA] } }
__declspec(naked) void FakeVerQueryValueW()				{ __asm { jmp [Proxy::OriginalVerQueryValueW] } }
