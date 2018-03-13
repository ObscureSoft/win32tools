#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <Tools/Tools.hpp>
#include <vector>

#include <windows.h>
#include <psapi.h>
//---------------------------------------------------------------------
namespace w32t
{
    Tools::Tools(){}
    //
    bool Tools::isWindowsNT()
    {
        // Get version of windows
        unsigned long wVersion = GetVersion();
        // Major and minor versions
        //auto major = (unsigned long)(LOBYTE(LOWORD(wVersion)));
        //auto minor = (unsigned long)(HIBYTE(LOWORD(wVersion)));
        return (wVersion < 0x80000000);
    }
    //
    _MODULEINFO* Tools::GetModuleInfo( const std::string&  _module )
    {
        _MODULEINFO* modinfo = nullptr;
        HMODULE hModule = GetModuleHandle(_module.c_str());

        if(hModule)
            GetModuleInformation(GetCurrentProcess(), hModule, modinfo, sizeof(modinfo));
        return modinfo;
    }
    //
    std::string Tools::exePath()
    {
        char buffer[MAX_PATH];
        ::GetModuleFileName(::GetModuleHandle(nullptr), buffer, MAX_PATH );
        std::string::size_type pos = std::string(buffer).find_last_of( "\\/" );
        return std::string(buffer).substr(0,pos);
    }
    //
    unsigned long Tools::FindPattern(const unsigned long& _startRange, const unsigned long& _endRange, unsigned char* _pattern, const char * _pMask )
    {
        int pos = 0, sLenth = (int)strlen(_pMask) - 1;
        unsigned long address = _startRange;
        unsigned long pageProc = (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS);
        MEMORY_BASIC_INFORMATION mbi;

        while(1)
        {
            if( (VirtualQuery( (void*)address, &mbi, sizeof(mbi) ) == 0) || (address > _endRange) ) break;

            if (mbi.State == MEM_COMMIT && ( (mbi.Protect != 0) && !(mbi.Protect & pageProc)) )
            {
                for( unsigned long retAddress = address; retAddress < (unsigned long)mbi.BaseAddress+mbi.RegionSize; retAddress++ )
                {
                    if(retAddress > _endRange) break;

                    if( *(unsigned char*)retAddress == _pattern[pos] || _pMask[pos] == '?' )
                    {
                        if( _pMask[pos+1] == '\0' )
                            return (unsigned long)(retAddress - sLenth);
                        pos++;
                    }
                    else pos = 0;
                }
            }
            address = (unsigned long)mbi.BaseAddress+mbi.RegionSize;
        }
        return 0;
    }
    //
    void Tools::memcpyX(void* _destination, const void* _source, const std::size_t& _numBytes, const unsigned long& _protect )
    {
        std::vector<MEMORY_BASIC_INFORMATION> vecMem;
        unsigned char* address = (unsigned char*)_destination;
        while( address <  ((unsigned char*)_destination + _numBytes) )
        {
            MEMORY_BASIC_INFORMATION mbi;
            if(VirtualQuery( address, &mbi, sizeof(mbi) ))
                vecMem.push_back(mbi);

            address = ( (unsigned char*)mbi.BaseAddress + mbi.RegionSize );
        }

        unsigned long dwOldProtect = 0;
        for(size_t i = 0; i < vecMem.size(); i++)
            VirtualProtect( vecMem[i].BaseAddress, vecMem[i].RegionSize, _protect, &dwOldProtect );

        memcpy(_destination, _source, _numBytes);

        for(size_t i = 0; i < vecMem.size(); i++)
            VirtualProtect( vecMem[i].BaseAddress, vecMem[i].RegionSize, vecMem[i].Protect, &dwOldProtect );

        vecMem.clear();
    }
} // namespace
//---------------------------------------------------------------------