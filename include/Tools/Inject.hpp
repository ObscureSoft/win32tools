#if !defined(W32T_INJECT_HPP)
#define W32T_INJECT_HPP

#include <string>
//---------------------------------------------------------------------
namespace w32t
{
    class Inject
    {
    public:
        Inject();
        Inject(const std::string&);

        bool byCreateRemoteThread(const uint32_t&);
        bool byCreateRemoteThread(const uint32_t&, const std::string&);

        bool  setDllPath(const std::string&);
        std::string getDllPath();

    private:
        std::string     m_dllPath;
        unsigned long   m_procAccess;
    };
} // namespace
//---------------------------------------------------------------------
#endif // W32T_INJECT_HPP