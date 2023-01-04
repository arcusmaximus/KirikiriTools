#include "stdafx.h"

using namespace std;

void Debugger::AddMemoryBreakpoint(void* address, int size)
{
    Log(L"Setting memory breakpoint at %08X with size %X", address, size);
	
    if (((DWORD)address & 0xFFF) != 0)
        throw exception("Memory breakpoint base address must be page-aligned");

    for (const MemoryBreakpoint& breakpoint : MemoryBreakpoints)
    {
        if (address < (BYTE*)breakpoint.GetAddress() + breakpoint.GetSize() && (BYTE*)address + size > breakpoint.GetAddress())
            throw exception("Memory breakpoint overlaps with an existing one");
    }
	
    MemoryBreakpoints.emplace_back(address, size);
    EnsureExceptionHandler();
}

void Debugger::RemoveMemoryBreakpoint(void* address)
{
    for (auto it = MemoryBreakpoints.begin(); it != MemoryBreakpoints.end(); ++it)
    {
        if (it->GetAddress() == address)
        {
            MemoryBreakpoints.erase(it);
            break;
        }
    }
    CleanExceptionHandler();
}

void Debugger::ClearMemoryBreakpoints()
{
    MemoryBreakpoints.clear();
    CleanExceptionHandler();
}

void Debugger::SetMemoryBreakpointHandler(const function<void(CONTEXT*)>& handler)
{
    MemoryBreakpointHandler = handler;
}

void Debugger::AddHardwareBreakpoint(void* address, HWBreakpointSize size, HWBreakpointMode mode, const function<void (CONTEXT*)>& handler)
{
    Log(L"Setting hardware breakpoint at %08X", address);
	
    vector<int> availableIndexes = { 0, 1, 2, 3 };
    for (const HardwareBreakpoint& breakpoint : HardwareBreakpoints)
    {
    	if (breakpoint.GetAddress() == address)
            throw exception("A hardware breakpoint is already active at the specified address.");

        availableIndexes.erase(find(availableIndexes.begin(), availableIndexes.end(), breakpoint.GetIndex()));
    }

    if (availableIndexes.empty())
        throw exception("The maximum amount of hardware breakpoints has already been placed.");

    HardwareBreakpoints.emplace_back(availableIndexes[0], address, size, mode, handler);
    EnsureExceptionHandler();
}

void Debugger::RemoveHardwareBreakpoint(void* address)
{
    for (auto it = HardwareBreakpoints.begin(); it != HardwareBreakpoints.end(); ++it)
    {
        if (it->GetAddress() == address)
        {
            HardwareBreakpoints.erase(it);
            break;
        }
    }

    CleanExceptionHandler();
}

void Debugger::ClearHardwareBreakpoints()
{
    HardwareBreakpoints.clear();
    CleanExceptionHandler();
}

void Debugger::RegisterDllLoadHandler(const function<void (const wchar_t*, HMODULE)>& handler)
{
    DllLoadHandlers.push_back(handler);

    if (OriginalLoadLibraryW == nullptr)
    {
        OriginalVirtualProtect = VirtualProtect;
        OriginalLoadLibraryA = LoadLibraryA;
        OriginalLoadLibraryW = LoadLibraryW;
        OriginalLoadLibraryExA = LoadLibraryExA;
        OriginalLoadLibraryExW = LoadLibraryExW;

        DetourTransactionBegin();
        DetourAttach((void**)&OriginalVirtualProtect, VirtualProtectHook);
        DetourAttach((void**)&OriginalLoadLibraryA, LoadLibraryAHook);
        DetourAttach((void**)&OriginalLoadLibraryW, LoadLibraryWHook);
        DetourAttach((void**)&OriginalLoadLibraryExA, LoadLibraryExAHook);
        DetourAttach((void**)&OriginalLoadLibraryExW, LoadLibraryExWHook);
        DetourTransactionCommit();
    }
}

void* Debugger::FindExport(HMODULE hModule, const char* pszName)
{
    FindExportContext context{};
    context.pszName = pszName;
    DetourEnumerateExports(hModule, &context, CheckModuleExport);
    return context.pFunction;
}

BOOL Debugger::CheckModuleExport(PVOID pContext, ULONG nOrdinal, LPCSTR pszName, PVOID pCode)
{
    FindExportContext* pFindContext = (FindExportContext*)pContext;
    if (pszName != nullptr && strcmp(pszName, pFindContext->pszName) == 0)
    {
        pFindContext->pFunction = pCode;
        return false;
    }
    return true;
}

void Debugger::Log(const wchar_t* pMessage, ...)
{
    va_list args;
    va_start(args, pMessage);
    wstring message = StringUtil::Format(pMessage, args);
    OutputDebugString(message.c_str());
}

Debugger::MemoryBreakpoint::MemoryBreakpoint(void* address, int size)
{
    _address = address;
    _size = size;
    Reapply();
}

Debugger::MemoryBreakpoint::~MemoryBreakpoint()
{
    BYTE* pFrom = (BYTE*)_address;
    BYTE* pTo = pFrom + _size;

    for (BYTE* pPage = pFrom; pPage < pTo; pPage += 0x1000)
    {
        MEMORY_BASIC_INFORMATION info;
        VirtualQuery(pPage, &info, sizeof(info));
        VirtualProtect(pPage, 0x1000, info.Protect & ~PAGE_GUARD, &info.Protect);
    }
}

void Debugger::MemoryBreakpoint::Reapply() const
{
    BYTE* pFrom = (BYTE*)_address;
    BYTE* pTo = pFrom + _size;

    for (BYTE* pPage = pFrom; pPage < pTo; pPage += 0x1000)
    {
        MEMORY_BASIC_INFORMATION info;
        VirtualQuery(pPage, &info, sizeof(info));
        VirtualProtect(pPage, 0x1000, info.Protect | PAGE_GUARD, &info.Protect);
    }
}

Debugger::HardwareBreakpoint::HardwareBreakpoint(int index, void* address, HWBreakpointSize size, HWBreakpointMode mode, const function<void(CONTEXT*)>& handler)
{
    _index = index;
    _address = address;
    _handler = handler;

    CONTEXT context;
    context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    GetThreadContext(GetCurrentThread(), &context);

    (&context.Dr0)[index] = (DWORD)address;
    context.Dr7 |= 1 << (2 * index);
    context.Dr7 |= (DWORD)mode << (16 + 4 * index);
    context.Dr7 |= (DWORD)size << (18 + 4 * index);

    SetThreadContext(GetCurrentThread(), &context);
}

Debugger::HardwareBreakpoint::~HardwareBreakpoint()
{
    CONTEXT context;
    context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    GetThreadContext(GetCurrentThread(), &context);

    (&context.Dr0)[_index] = 0;
    context.Dr7 &= ~(1 << (2 * _index));
    context.Dr7 &= ~(0xF << (16 + 4 * _index));

    SetThreadContext(GetCurrentThread(), &context);
}

void Debugger::ApplyMemoryBreakpoints()
{
    for (const MemoryBreakpoint& breakpoint : MemoryBreakpoints)
    {
        breakpoint.Reapply();
    }
}

void Debugger::EnsureExceptionHandler()
{
    if (ExceptionHandlerHandle == nullptr)
        ExceptionHandlerHandle = AddVectoredExceptionHandler(true, HandleException);
}

void Debugger::CleanExceptionHandler()
{
    if (ExceptionHandlerHandle == nullptr || !MemoryBreakpoints.empty() || !HardwareBreakpoints.empty())
        return;

    RemoveVectoredExceptionHandler(ExceptionHandlerHandle);
    ExceptionHandlerHandle = nullptr;
}

long Debugger::HandleException(EXCEPTION_POINTERS* pException)
{
    bool handled = false;
    switch (pException->ExceptionRecord->ExceptionCode)
    {
		case STATUS_GUARD_PAGE_VIOLATION:
            handled = HandleMemoryBreakpoint(pException->ContextRecord);
            break;
    	
		case STATUS_SINGLE_STEP:
            handled = HandleHardwareBreakpoint(pException->ContextRecord);
            break;
    }

    return handled ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_CONTINUE_SEARCH;
}

bool Debugger::HandleMemoryBreakpoint(CONTEXT* pContext)
{
    ScheduleBreakpointHandler(MemoryBreakpointHandler, pContext);
    return true;
}

bool Debugger::HandleHardwareBreakpoint(CONTEXT* pContext)
{
    int index = -1;
    for (int i = 0; i < 4; i++)
    {
        if (pContext->Dr6 & (1 << i))
        {
            index = i;
            break;
        }
    }

    if (index < 0)
        return false;

    const HardwareBreakpoint* pBreakpoint = nullptr;
    for (const HardwareBreakpoint& breakpoint : HardwareBreakpoints)
    {
        if (breakpoint.GetIndex() == index)
        {
            pBreakpoint = &breakpoint;
            break;
        }
    }

    if (pBreakpoint == nullptr)
        return false;

    pContext->Dr6 = 0;
    ScheduleBreakpointHandler(pBreakpoint->GetHandler(), pContext);
    return true;
}

void Debugger::ScheduleBreakpointHandler(const function<void(CONTEXT*)>& handler, CONTEXT* pContext)
{
    CurrentBreakpointHandler = handler;
    CurrentBreakpointContext = *pContext;

    pContext->Esp -= 4;
    *(DWORD*)pContext->Esp = pContext->Eip;
    pContext->Eip = (DWORD)&ExecuteBreakpointHandlerWrap;
}

void __declspec(naked) Debugger::ExecuteBreakpointHandlerWrap()
{
    __asm
    {
        pushfd
        pushad
    	cld
        call ExecuteBreakpointHandler
        popad
        popfd
        ret
    }
}

void Debugger::ExecuteBreakpointHandler()
{
    CurrentBreakpointHandler(&CurrentBreakpointContext);
    CurrentBreakpointHandler = nullptr;

    ApplyMemoryBreakpoints();
}

BOOL Debugger::VirtualProtectHook(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{
    if (!OriginalVirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect))
        return false;

    if (flNewProtect != PAGE_EXECUTE_READ)
        return true;

    BYTE* pModule = (BYTE*)lpAddress - 0x1000;
    MEMORY_BASIC_INFORMATION memInfo;
    if (VirtualQuery(pModule, &memInfo, sizeof(memInfo)) == 0)
        return true;

    if (memInfo.AllocationBase != pModule ||
        memInfo.RegionSize != 0x1000 ||
        memInfo.State != MEM_COMMIT ||
        memInfo.Type != MEM_PRIVATE ||
        pModule[0] != 'M' ||
        pModule[1] != 'Z')
    {
        return true;
    }

    for (auto handler : DllLoadHandlers)
    {
        handler(nullptr, (HMODULE)pModule);
    }

    return true;
}

HMODULE Debugger::LoadLibraryAHook(LPCSTR lpLibFileName)
{
    HMODULE hModule = OriginalLoadLibraryA(lpLibFileName);
    if (hModule == nullptr)
        return nullptr;

    wstring filePath = StringUtil::ToUTF16(lpLibFileName);
    for (auto handler : DllLoadHandlers)
    {
        handler(filePath.c_str(), hModule);
    }
    return hModule;
}

HMODULE Debugger::LoadLibraryWHook(LPCWSTR lpLibFileName)
{
    HMODULE hModule = OriginalLoadLibraryW(lpLibFileName);
    if (hModule == nullptr)
        return nullptr;

    for (auto handler : DllLoadHandlers)
    {
        handler(lpLibFileName, hModule);
    }
    return hModule;
}

HMODULE Debugger::LoadLibraryExAHook(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE hModule = OriginalLoadLibraryExA(lpLibFileName, hFile, dwFlags);
    if (hModule == nullptr)
        return nullptr;

    wstring filePath = StringUtil::ToUTF16(lpLibFileName);
    for (auto handler : DllLoadHandlers)
    {
        handler(filePath.c_str(), hModule);
    }
    return hModule;
}

HMODULE Debugger::LoadLibraryExWHook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE hModule = OriginalLoadLibraryExW(lpLibFileName, hFile, dwFlags);
    if (hModule == nullptr)
        return nullptr;

    for (auto handler : DllLoadHandlers)
    {
        handler(lpLibFileName, hModule);
    }
    return hModule;
}
