#if !defined(W32T_IOCPTCPSERVER_HPP)
#define W32T_IOCPTCPSERVER_HPP

#include <Network/NetworkSocket.hpp>
#include <windows.h>
//---------------------------------------------------------------------
namespace w32t
{
    struct IoContext : public _OVERLAPPED
    {
    public:
        IoContext();
        ~IoContext();
        enum State
        {
            IoAccept = 0,
            IoSend,
            IoRecv
        };

        SOCKET Socket;
        
        char    Buffer[8192];
        WSABUF  wsabuf;
        State   state;
        unsigned long bytesToSend;
        unsigned long bytesSent;
    }; // IoContext
    //====================================================================
    class IocpTcpServer : public NetworkSocket
    {
    public:
        IocpTcpServer();
        ~IocpTcpServer();
        
        bool        Accept(NetworkSocket& socket);
        bool        Listen(const unsigned int & portNumber, unsigned long __stdcall(*workerThread)(void*) );
    private:
        bool        create(unsigned long __stdcall(*workerThread)(void*));
        IoContext * UpdateCompletionPort(SOCKET socketHandle,IoContext::State IoState);

        IoContext * IoClient;
        bool        bInit;
        HANDLE      hIoCp;
    }; // IocpTcpServer

} // namespace
//---------------------------------------------------------------------
#endif // W32T_IOCPTCPSERVER_HPP