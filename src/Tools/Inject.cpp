#include <Tools/Inject.hpp>
#include <windows.h>
//---------------------------------------------------------------------
namespace w32t
{
    Inject::Inject() : m_dllPath(""),
    m_procAccess(PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ){}

    Inject::Inject(const std::string& _dllPath) : m_dllPath(_dllPath),
    m_procAccess(PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ){}
    
    bool Inject::byCreateRemoteThread(const uint32_t& _pID)
    {
        HANDLE hProc = OpenProcess(m_procAccess, 0, _pID);
        if(hProc == INVALID_HANDLE_VALUE) return false;

        LPTHREAD_START_ROUTINE LoadLibAddy = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
        if(LoadLibAddy == NULL) //something went wrong, use GetLastError() to find out - too lazy to do now -_-
        {
            CloseHandle(hProc);
            return false;
        }

        void* RemoteString = VirtualAllocEx(hProc, NULL, m_dllPath.length(), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
        if(!RemoteString) return false;

        if(!WriteProcessMemory(hProc, RemoteString, m_dllPath.c_str(), m_dllPath.length(), 0)) return false;

        HANDLE hrt = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibAddy, RemoteString, 0, 0);
        if(hrt == INVALID_HANDLE_VALUE) return false;

        WaitForSingleObject(hrt, INFINITE);
        VirtualFreeEx(hProc, RemoteString, m_dllPath.length(), MEM_DECOMMIT|MEM_RELEASE);

        CloseHandle(hrt);
        CloseHandle(hProc);

        return true;
    }

    bool Inject::byCreateRemoteThread(const uint32_t& _pID, const std::string& _dllPath)
    {
        if(_pID)
        {
            if(_dllPath.empty()) return false;

            m_dllPath = _dllPath;
            return this->byCreateRemoteThread(_pID);
        }
        return false;
    }

    bool Inject::setDllPath(const std::string& _dllPath)
    {
        if(_dllPath.empty()) return false;

        m_dllPath = _dllPath;
        return true;
    }

    std::string Inject::getDllPath()
    {
        return m_dllPath;
    }
} // namespace
//---------------------------------------------------------------------