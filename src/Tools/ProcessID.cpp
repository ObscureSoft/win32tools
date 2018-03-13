#if !defined WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Tools/ProcessID.hpp>

#include <windows.h>
#include <tlhelp32.h>
//---------------------------------------------------------------------
namespace w32t
{
    ProcessID::ProcessID() : pID(0){}
    // get proccess ID from window name
    unsigned long ProcessID::fromWindow(const std::string& _className, const std::string& _windowName)
    {
        HWND    wndTarget;
        if(_className.length() <= 0 && _windowName.length() <= 0) return 0;

        wndTarget = FindWindow(_className.c_str(), _windowName.c_str());
        if(wndTarget == nullptr) return 0;

        return GetWindowThreadProcessId(wndTarget, &pID) ? pID : 0 ;
    }
    // get proccess ID from exe file
    unsigned long ProcessID::fromProcName(const std::string& _processName)
    {
        PROCESSENTRY32  processInfo;
        processInfo.dwSize = sizeof(PROCESSENTRY32);

        int     processSuccess;
        HANDLE  SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if( (SnapShot == INVALID_HANDLE_VALUE) || (SnapShot == nullptr) ) return 0;

        processSuccess  = Process32First(SnapShot, &processInfo);
        if(processSuccess)
        {
            do
            {
                if ( _processName.compare(processInfo.szExeFile) == 0 )
                {
                    CloseHandle(SnapShot);
                    return processInfo.th32ProcessID;
                }
                processSuccess = Process32Next(SnapShot, &processInfo);
            } while (processSuccess);
        }
        CloseHandle(SnapShot);
        return 0;
    }
} // namespace
//---------------------------------------------------------------------