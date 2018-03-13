#if !defined(W32T_NAMEDPIPE_HPP)
#define W32T_NAMEDPIPE_HPP

#include <string>
//---------------------------------------------------------------------
namespace w32t
{
    class NamedPipe
    {
    public:
        NamedPipe(const std::string&);
        ~NamedPipe();

        bool    pConnect();
        void    pDisconnect();
        bool    pOpen();
        void    pClose();

        bool    isConnected;
        void*   pGetPipeInstance();

        unsigned long pRead(void* _readBuffer, const unsigned long&);
        unsigned long pWrite(const void* _writeBuffer, const unsigned long&);
        
    private:
        unsigned long bytesRead, bytesWritten;
        unsigned long bufferSize, pipeMode, getError;

        std::string   pipeName;
        void*         hPipe;
    }; // NamedPipe
} // namespace
//---------------------------------------------------------------------
#endif // W32T_NAMEDPIPE_HPP
