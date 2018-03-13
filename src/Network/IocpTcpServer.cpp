#include <Network/IocpTcpServer.hpp>
//---------------------------------------------------------------------
namespace w32t
{
    IoContext::IoContext() : Socket(INVALID_SOCKET), state(IoContext::State::IoAccept), bytesToSend(0), bytesSent(0){}
    IoContext::~IoContext() {}
    //====================================================================
    
    IocpTcpServer::IocpTcpServer() : NetworkSocket(NetworkSocket::Type::Tcp), IoClient(nullptr), bInit(false), hIoCp(INVALID_HANDLE_VALUE){}
    //
    IocpTcpServer::~IocpTcpServer()
    {
        if(IoClient != nullptr) delete IoClient;
    }
    
    bool IocpTcpServer::Listen(const unsigned int& portNumber, unsigned long __stdcall(*workerThread)(void*))
    {
        if(getLocalPort() != portNumber)
        {
            eraseError(33);
            if(!this->create(workerThread))
            {
                errorString.append("Failed to create listening socket! \r\n");
                close();
                return false;
            }

            sockaddr_in addr;
            int addrlen = sizeof(addr);
            memset(&addr,0,sizeof(addr));

            addr.sin_family       = AF_INET;
            addr.sin_port         = htons(portNumber);
            addr.sin_addr.s_addr  = htonl(INADDR_ANY);

            if(::bind(getSocket(), (sockaddr*)&addr, addrlen) != 0 )
            {
                errorString.append("Failed to bind listening socket! \r\n");
                close();
                return false;
            }

            if(listen(getSocket(),10) != 0)
            {
                errorString.append("Failed to listen on port! \r\n");
                close();
                return false;
            }

            int nZero = 0;
            if(setsockopt(getSocket(),SOL_SOCKET,SO_SNDBUF, (char*)&nZero,sizeof(nZero)))
                return false;

            return true;
        }
        return true;
    }

    bool IocpTcpServer::Accept(NetworkSocket& /*nsocket*/)
    {
        sockaddr_in sClient;
        int nClientSize = sizeof(sClient);
        peerSocket = WSAAccept(getSocket(),(sockaddr*)&sClient,&nClientSize,nullptr,0);

        if(peerSocket == INVALID_SOCKET)
        {
            errorString.append("WSAAccept failed!\r\n");
            return false;
        }
        peerAddress = inet_ntoa(sClient.sin_addr);
        peerPort    = ntohs(sClient.sin_port);

        IoClient = UpdateCompletionPort(peerSocket,IoContext::State::IoRecv);

        if(IoClient == nullptr)
        {
            errorString.append("Failed to create/update completeion port!\r\n");
            return false;
        }

        unsigned long dwFlags = 0, recvBytes= 0;
        if(WSARecv(peerSocket,&IoClient->wsabuf,1,&recvBytes,&dwFlags,IoClient,nullptr) &&
        (WSAGetLastError() != WSA_IO_PENDING))
        {
            return false;
        }

        return true;
    }

    IoContext * IocpTcpServer::UpdateCompletionPort(SOCKET socketHandle, IoContext::State IoState)
    {
        IoContext * uContext = new IoContext;
        memset(uContext,0,sizeof(IoContext));
        
        uContext->Socket      = socketHandle;

        uContext->Internal     = 0;
        uContext->InternalHigh = 0;
        uContext->Offset       = 0;
        uContext->OffsetHigh   = 0;
        uContext->hEvent       = nullptr;

        uContext->state = IoState;

        uContext->bytesToSend = 0;
        uContext->bytesSent   = 0;
        uContext->wsabuf.buf  = uContext->Buffer;
        uContext->wsabuf.len  = sizeof(uContext->Buffer);

        memset(uContext->wsabuf.buf,0,uContext->wsabuf.len);

        this->hIoCp = CreateIoCompletionPort((HANDLE)socketHandle,this->hIoCp,(ULONG_PTR)uContext,0);
        if(this->hIoCp == nullptr)
        {
            delete uContext;
            return nullptr;
        }

        return uContext;
    }

    bool IocpTcpServer::create(unsigned long __stdcall(*workerThread)(void*))
    {
        if(!Socket::create()) return false;

        SYSTEM_INFO systemInfo;
        GetSystemInfo(&systemInfo);

        hIoCp = CreateIoCompletionPort(INVALID_HANDLE_VALUE,nullptr,0,systemInfo.dwNumberOfProcessors);
        if (hIoCp == nullptr)
        {
            errorString.append("CreateIoCompletionPort failed!\r\n");
            WSACleanup();
            return false;
        }

        for(unsigned long i=0; i<systemInfo.dwNumberOfProcessors*2; ++i)
        {
            HANDLE hThread = CreateThread(nullptr,0,workerThread,hIoCp,0,nullptr);
            if(hThread == nullptr || hThread == INVALID_HANDLE_VALUE)
            {
                errorString.append("Failed to create thread!\r\n");
                CloseHandle(hThread);
                return false;
            }
            CloseHandle(hThread);
        }
        return true;
    }
} // namespace
//---------------------------------------------------------------------