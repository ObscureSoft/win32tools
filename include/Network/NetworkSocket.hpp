#if !defined(W32T_NETWORKSOCKET_HPP)
#define W32T_NETWORKSOCKET_HPP

#include <Network/Socket.hpp>
//---------------------------------------------------------------------
namespace w32t
{
    class Packet;
    
    class NetworkSocket : public Socket
    {
    public:

        unsigned short  getPeerPort();
        std::string     getPeerAddress();
        //
        bool conn(const std::string & remoteAddress, const unsigned short& remotePort);

        int  send(const void * data, const size_t & dataSize);
        int  send(Packet& tcpPacket);
        int  recv(void * data, size_t dataSize);
        int  recv(Packet& tcpPacket);

        void close();

    protected:
        friend class TcpServer;

        NetworkSocket(Socket::Type socket_type);
        ~NetworkSocket();

        SOCKET          peerSocket;
        // peer
        std::string     peerAddress;
        unsigned int    peerPort;
        bool            m_connected;
    }; // NetworkSocket
    
    //===============================================================
    class TcpSocket : public NetworkSocket
    {
    public:
        TcpSocket() : NetworkSocket(Socket::Type::Tcp){}
    };
    //-------------------------------------
    class UdpSocket : public NetworkSocket
    {
    public:
        UdpSocket() : NetworkSocket(Socket::Type::Udp){}
    };

} // namespace
//---------------------------------------------------------------------
#endif // W32T_NETWORKSOCKET_HPP