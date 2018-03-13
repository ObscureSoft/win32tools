#include <Network/NetworkSocket.hpp>
#include <Network/Packet.hpp>

//---------------------------------------------------------------------
namespace w32t
{
    unsigned short NetworkSocket::getPeerPort()
    {
        return m_connected ? peerPort : 0;
    }

    std::string NetworkSocket::getPeerAddress()
    {
        return  m_connected ? peerAddress : "Not connected\n";
    }

    bool NetworkSocket::conn(const std::string& remoteAddress, const unsigned short& remotePort)
    {
        eraseError(33);
        if(!create())
        {
            errorString.append("Failed to create listening socket! \n");
            return false;
        }
        sockaddr_in addr;
        memset(&addr,0,sizeof(addr));
        hostent * host;

        if( (host = gethostbyname(remoteAddress.c_str()) ) == nullptr )
        {
            errorString.append("Failed to get get hostname!\n");
            return false;
        }

        addr.sin_family       = AF_INET;
        addr.sin_port         = htons(remotePort);
        addr.sin_addr.s_addr  = *((unsigned long*)host->h_addr_list[0]);

        if(::connect(getSocket(), (sockaddr*)(&addr), sizeof(addr)) < 0)
        {
            errorString.append("Failed to establish a connection!\n");
            m_connected = false;
            return false;
        }
        peerSocket  = getSocket();
        peerAddress = inet_ntoa(addr.sin_addr);
        peerPort    = ntohs(addr.sin_port);
        m_connected = true;

        return true;
    }

    int NetworkSocket::send(const void * data, const size_t & dataSize)
    {
        int ret = 0;

        do
        {
            ret += ::send(getSocket(),(char*)data, dataSize-ret, 0);
            if(ret == SOCKET_ERROR)
            {
                errorString.append("send() error!\n");
                break;
            }

        } while (ret < (int)dataSize);

        return ret;
    }

    int NetworkSocket::send(Packet& tcpPacket)
    {
        if(tcpPacket.buffer.empty()) return 0;
        int ret = this->send(&tcpPacket.buffer[0],tcpPacket.buffer.size());

        return ret;
    }

    int NetworkSocket::recv(void * data, size_t dataSize)
    {
        int totalBytes = ::recv(getSocket(),(char*)data, dataSize, 0);
        return totalBytes;
    }

    int NetworkSocket::recv(Packet& tcpPacket)
    {
         tcpPacket.buffer.resize(8000);
         int ret = this->recv(&tcpPacket.buffer[0],tcpPacket.getSize());

         std::uint16_t dataSize = 0;
         if(ret > 0)
         {
             std::copy(&tcpPacket.buffer[tcpPacket.pos], &tcpPacket.buffer[tcpPacket.pos] + sizeof(dataSize), &dataSize);

             while(ret-sizeof(dataSize) < dataSize )
             {
                 ret = this->recv(&tcpPacket.buffer[0],tcpPacket.getSize());
             }
         }

         return ret;
    }

    void NetworkSocket::close()
    {
        ::shutdown(peerSocket,2);
        Socket::close();
    }

    NetworkSocket::NetworkSocket(Type socket_type) : Socket(socket_type), peerSocket(-1), m_connected(false)
    {
    }

    NetworkSocket::~NetworkSocket()
    {

    }
} //namespace
//---------------------------------------------------------------------