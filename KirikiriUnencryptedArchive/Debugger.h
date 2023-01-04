#pragma once

enum class HWBreakpointMode
{
    Execute   = 0b00,
    Write     = 0b01,
    ReadWrite = 0b11
};

enum class HWBreakpointSize
{
    Byte    = 0b00,
    Word    = 0b01,
    Dword   = 0b11,
    Qword   = 0b10
};

class Debugger
{
public:
    static void             AddMemoryBreakpoint             (void* address, int size);
    static void             RemoveMemoryBreakpoint          (void* address);
    static void             ClearMemoryBreakpoints          ();
    static void             SetMemoryBreakpointHandler      (const std::function<void (CONTEXT*)>& handler);

    static void             AddHardwareBreakpoint           (void* address, HWBreakpointSize size, HWBreakpointMode mode, const std::function<void (CONTEXT*)>& handler);
    static void             RemoveHardwareBreakpoint        (void* address);
    static void             ClearHardwareBreakpoints        ();

    static void             RegisterDllLoadHandler          (const std::function<void (const wchar_t*, HMODULE)>& handler);

    static void*            FindExport                      (HMODULE hModule, const char* pszName);

    static void             Log                             (const wchar_t* pMessage, ...);

private:
    class MemoryBreakpoint
    {
    public:
                                MemoryBreakpoint                (void* address, int size);
                                ~MemoryBreakpoint               ();

        void*                   GetAddress                      () const { return _address; }
        int                     GetSize                         () const { return _size; }
        
        void                    Reapply                         () const;
        
    private:
        void* _address;
        int _size;
    };
    
    class HardwareBreakpoint
    {
    public:
                                HardwareBreakpoint              (int index, void* address, HWBreakpointSize size, HWBreakpointMode mode, const std::function<void (CONTEXT*)>& handler);
                                ~HardwareBreakpoint             ();

        int                     GetIndex                        () const { return _index; }
        void*                   GetAddress                      () const { return _address; }
        const std::function<void (CONTEXT*)>& GetHandler        () const { return _handler; }

    private:
        int _index;
        void* _address;
        std::function<void (CONTEXT*)> _handler;
    };

    struct FindExportContext
    {
        const char* pszName;
        void* pFunction;
    };

    static BOOL __stdcall   CheckModuleExport               (PVOID pContext, ULONG nOrdinal, LPCSTR pszName, PVOID pCode);

    static void             ApplyMemoryBreakpoints          ();

    static void             EnsureExceptionHandler          ();
    static void             CleanExceptionHandler           ();
    
    static long __stdcall   HandleException                 (EXCEPTION_POINTERS* pException);
    static bool             HandleMemoryBreakpoint          (CONTEXT* pContext);
    static bool             HandleHardwareBreakpoint        (CONTEXT* pContext);

    static void             ScheduleBreakpointHandler       (const std::function<void (CONTEXT*)>& handler, CONTEXT* pContext);
    static void             ExecuteBreakpointHandlerWrap    ();
    static void             ExecuteBreakpointHandler        ();

    static BOOL WINAPI      VirtualProtectHook              (LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);

    static HMODULE WINAPI   LoadLibraryAHook                (LPCSTR  lpLibFileName);
    static HMODULE WINAPI   LoadLibraryWHook                (LPCWSTR lpLibFileName);
    static HMODULE WINAPI   LoadLibraryExAHook              (LPCSTR  lpLibFileName, HANDLE hFile, DWORD dwFlags);
    static HMODULE WINAPI   LoadLibraryExWHook              (LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);

    static inline void* ExceptionHandlerHandle{};
    static inline std::vector<MemoryBreakpoint> MemoryBreakpoints{};
    static inline std::function<void (CONTEXT*)> MemoryBreakpointHandler{};
    static inline std::vector<HardwareBreakpoint> HardwareBreakpoints{};

    static inline std::function<void (CONTEXT*)> CurrentBreakpointHandler{};
    static inline CONTEXT CurrentBreakpointContext{};

    static inline std::vector<std::function<void (const wchar_t*, HMODULE)>> DllLoadHandlers{};
    static inline decltype(VirtualProtect)* OriginalVirtualProtect{};
    static inline decltype(LoadLibraryA)* OriginalLoadLibraryA{};
    static inline decltype(LoadLibraryW)* OriginalLoadLibraryW{};
    static inline decltype(LoadLibraryExA)* OriginalLoadLibraryExA{};
    static inline decltype(LoadLibraryExW)* OriginalLoadLibraryExW{};
};
