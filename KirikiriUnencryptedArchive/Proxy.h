#pragma once

class Proxy
{
public:
	static void Init();

	static inline decltype(GetFileVersionInfoA)*		OriginalGetFileVersionInfoA{};
	static inline void*									OriginalGetFileVersionInfoByHandle{};
	static inline decltype(GetFileVersionInfoExA)*		OriginalGetFileVersionInfoExA{};
	static inline decltype(GetFileVersionInfoExW)*		OriginalGetFileVersionInfoExW{};
	static inline decltype(GetFileVersionInfoSizeA)*	OriginalGetFileVersionInfoSizeA{};
	static inline decltype(GetFileVersionInfoSizeExA)*	OriginalGetFileVersionInfoSizeExA{};
	static inline decltype(GetFileVersionInfoSizeExW)*	OriginalGetFileVersionInfoSizeExW{};
	static inline decltype(GetFileVersionInfoSizeW)*	OriginalGetFileVersionInfoSizeW{};
	static inline decltype(GetFileVersionInfoW)*		OriginalGetFileVersionInfoW{};
	static inline decltype(VerFindFileA)*				OriginalVerFindFileA{};
	static inline decltype(VerFindFileW)*				OriginalVerFindFileW{};
	static inline decltype(VerInstallFileA)*			OriginalVerInstallFileA{};
	static inline decltype(VerInstallFileW)*			OriginalVerInstallFileW{};
	static inline decltype(VerLanguageNameA)*			OriginalVerLanguageNameA{};
	static inline decltype(VerLanguageNameW)*			OriginalVerLanguageNameW{};
	static inline decltype(VerQueryValueA)*				OriginalVerQueryValueA{};
	static inline decltype(VerQueryValueW)*				OriginalVerQueryValueW{};
};
