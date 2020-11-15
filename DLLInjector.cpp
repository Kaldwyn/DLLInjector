#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

DWORD getPIDFromName(LPCSTR procName)
{
    // Loops through processes to find one created from the specified executable.
    DWORD procId = NULL;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (!_stricmp(entry.szExeFile, procName))
            {
                procId = entry.th32ProcessID;
                break;
            }
        }
    }
    CloseHandle(snapshot);
    return procId;
}

int main()
{
    LPCSTR dllPath = "C:\\Path\\to\\dll.dll";   //Path to DLL goes here
    LPCSTR procName = "target.exe";             //target executable
    DWORD procId = 0;

    while (!procId)
    {
        procId = getPIDFromName(procName);
        Sleep(50);
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);

    if (hProc && hProc != INVALID_HANDLE_VALUE)
    {
        LPVOID addr = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        if (addr)
        {
            WriteProcessMemory(hProc, addr, dllPath, strlen(dllPath) + 1, 0);
        }

        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, addr, 0, 0);

        WaitForSingleObject(hThread, INFINITE);

        CloseHandle(hThread);
    }

    if (hProc)
    {
        CloseHandle(hProc);
    }

    return 0;
}
