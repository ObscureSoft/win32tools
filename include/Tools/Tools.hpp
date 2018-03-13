#if !defined(W32T_TOOLS_HPP)
#define W32T_TOOLS_HPP

#include <string>
struct _MODULEINFO;
//---------------------------------------------------------------------
namespace w32t
{
    class Tools
    {
    public:
        Tools();
        
        bool            isWindowsNT();
        _MODULEINFO*    GetModuleInfo( const std::string& );
        std::string     exePath();
        unsigned long   FindPattern(const unsigned long&, const unsigned long&, unsigned char*, const char* );
        void            memcpyX(void*, const void*, const std::size_t&, const unsigned long&);
    };
} //namespace
//---------------------------------------------------------------------
#endif // W32T_TOOLS_HPP