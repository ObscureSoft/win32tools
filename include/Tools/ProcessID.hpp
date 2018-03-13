#if !defined(W32T_PROCESSID_HPP)
#define W32T_PROCESSID_HPP

#include <string>
//---------------------------------------------------------------------
namespace w32t
{
    class ProcessID
    {
    public:
        ProcessID();

        unsigned long fromWindow(const std::string&, const std::string&);
        unsigned long fromProcName(const std::string&);
    protected:
        unsigned long pID;
    }; // ProcessID
} // namespace
//---------------------------------------------------------------------
#endif // W32T_PROCESSID_HPP
