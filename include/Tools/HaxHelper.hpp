#if !defined(W32T_HAXHELPER_HPP)
#define W32T_HAXHELPER_HPP

#include <Tools/Tools.hpp>
//---------------------------------------------------------------------
namespace w32t
{
    class HaxHelper : public Tools
    {
    public:
        explicit HaxHelper();
        explicit HaxHelper(const std::string&);
        virtual ~HaxHelper();

        unsigned long   pIDFromWindow(const std::string&, const std::string&);
        unsigned long   pIDFromProcess(const std::string& );
        bool            injectDll(const unsigned long&, const std::string& );
        unsigned long   getAddress(const unsigned long&, const unsigned long&, unsigned char*, const char* );
        void            memCopy(const unsigned long&, const unsigned long& _source, const std::size_t& );

        void            setMemProtect(const unsigned long&);
        void            setProcessName(const std::string&);
        void            setpID(const unsigned long&);

        std::string&    getProcessName();
        unsigned long&  getpID();
        unsigned long&  getMemProtect();
        virtual void    cleanup();

    private:
        std::string  processName;
        unsigned long pID, memProtect;
    }; // HaxHelper
} // namespace
//---------------------------------------------------------------------
#endif // W32T_HAXHELPER_HPP