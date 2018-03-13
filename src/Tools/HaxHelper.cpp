#if !defined WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Tools/HaxHelper.hpp>
#include <Tools/ProcessID.hpp>
#include <Tools/Inject.hpp>
//---------------------------------------------------------------------
namespace w32t
{
    HaxHelper::HaxHelper() : processName(""), pID(0), memProtect(0x40){}
    HaxHelper::HaxHelper(const std::string& _processName) : processName(_processName), pID(0), memProtect(0x40){}
    HaxHelper::~HaxHelper(){}

    unsigned long HaxHelper::pIDFromWindow(const std::string& _className, const std::string& _windowName)
    {
        if(_className.empty() && _windowName.empty()) return 0;

        ProcessID tpID;
        pID = tpID.fromWindow(_className, _windowName);

        return pID;
    }

    unsigned long HaxHelper::pIDFromProcess(const std::string& _processName )
    {
        ProcessID tpID;

        if(_processName.empty())
        {
            if(processName.empty()) return 0;

            pID = tpID.fromProcName(processName);
            if(pID) return pID;
        }
        else
        {
            pID = tpID.fromProcName(_processName);
            if(pID) return pID;
        }

        return 0;
    }

    bool HaxHelper::injectDll(const unsigned long& _pID, const std::string& _dllPath )
    {
        if( (_pID == 0 || _pID == 4) || _dllPath.empty()) return false;

        Inject tInject(_dllPath);

        bool tmpInject = tInject.byCreateRemoteThread(_pID);

        if(tmpInject) return true;

        return false;
    }

    unsigned long HaxHelper::getAddress(const unsigned long& _startAddress, const unsigned long& _endAddress,
                                      unsigned char* _pattern, const char* _pMask )
    {
        unsigned long tmpAddress = FindPattern(_startAddress, _endAddress, _pattern, _pMask);
        return tmpAddress;
    }

    void HaxHelper::memCopy(const unsigned long& _destination, const unsigned long& _source,
                          const size_t& _numBytes )
    {
        if(_numBytes <= 0) return;

        memcpyX((void*)_destination, (void*)_source, _numBytes, memProtect);
    }

    void HaxHelper::setMemProtect(const unsigned long& _newProtection)
    {
        memProtect = _newProtection;
    }

    void HaxHelper::setProcessName(const std::string& _processName)
    {
        processName = _processName;
    }

    void HaxHelper::setpID(const unsigned long& _pID)
    {
        pID = _pID;
    }

    std::string& HaxHelper::getProcessName()
    {
        return processName;
    }

    unsigned long& HaxHelper::getpID()
    {
        return pID;
    }

    unsigned long& HaxHelper::getMemProtect()
    {
        return memProtect;
    }
} //namespace
//---------------------------------------------------------------------