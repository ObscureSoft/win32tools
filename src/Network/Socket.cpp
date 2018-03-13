#if !defined WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Network/Socket.hpp>
//---------------------------------------------------------------------
namespace w32t
{
    Socket::~Socket()
    {
        closesocket(m_socket);
        WSACleanup();
        m_socket = INVALID_SOCKET;
    }

    Socket::Socket(Type socket_type) : m_type(socket_type), m_socket(INVALID_SOCKET)
    {
        WSADATA wsaData;

        memset(&wsaData,0,sizeof(WSADATA));
        if(WSAStartup(MAKEWORD(2,2),&wsaData) )
        {
            errorString.append("Could not load Winsock 2.2 dll!\n");
            WSACleanup();
        }
    }

    std::string Socket::getError() const
    {
        return this->errorString;
    }

    void Socket::eraseError(const size_t& lenth)
    {
        size_t tmpLenth = lenth;
        if(tmpLenth > errorString.size())
        {
            tmpLenth = errorString.size();
        }
        errorString.erase(errorString.size()-tmpLenth);
    }

    unsigned short Socket::getLocalPort()
    {
        sockaddr_in addr;
        int addr_len = sizeof(addr);
        memset(&addr,0,addr_len);

        if (getsockname(getSocket(), (sockaddr*)&addr, &addr_len) < 0)
        {
            errorString.append("Failed to get local port number!\n");
            return 0;
        }
        return ntohs(addr.sin_port);
    }

    std::string Socket::getLocalAddress()
    {
        sockaddr_in addr;
        int addr_len = sizeof(addr);
        memset(&addr,0,addr_len);

        if (getsockname(getSocket(), (sockaddr*) &addr, &addr_len) < 0)
        {
            errorString.append("Failed to get local address!\n");
            return errorString;
        }
        return inet_ntoa(addr.sin_addr);
    }

    bool Socket::create()
    {
        if( m_type == Socket::Type::Tcp)
        {
            if( (m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET)
            {
               errorString.append("Could not create tcp socket!\n");
                return false;
            }
        }
        else if(m_type == Socket::Type::Udp)
        {
            if( (m_socket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) == INVALID_SOCKET)
            {
                errorString.append("Could not create udp socket!\n");
                return false;
            }
        }
        return true;
    }

    void Socket::create(SOCKET hSocket)
    {
        this->m_socket = hSocket;
    }

    void Socket::close()
    {
        shutdown(m_socket,SD_BOTH);
    }

    SOCKET Socket::getSocket() const
    {
        return this->m_socket;
    }

    bool Socket::setBlock(bool blockingFlag)
    {
        if(blockingFlag)
        {
            u_long iMode = 0;
            if(ioctlsocket(getSocket(), FIONBIO, &iMode) == 0) return true;
        }
        else
        {
            u_long iMode = 1;
            if(ioctlsocket(getSocket(), FIONBIO, &iMode) == 0) return true;
        }
        return false;
    }
} // namespace
//---------------------------------------------------------------------