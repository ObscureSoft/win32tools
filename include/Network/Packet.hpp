#if !defined(W32T_PACKET_HPP)
#define W32T_PACKET_HPP

#include <vector>
//---------------------------------------------------------------------
namespace w32t
{
    class NetworkSocket;
    class TcpServer;

    class Packet
    {

    public:
        Packet();
        virtual ~Packet();

        Packet& operator << (const char *);
        Packet& operator << (const unsigned int&);

        Packet& operator >> (char *);
        Packet& operator >> (unsigned int&);

        std::size_t  getSize();

    protected:
        friend class NetworkSocket;
        friend class TcpServer;

    private:
        bool operator == (const Packet& cmp) const;
        bool operator != (const Packet& cmp) const;

        std::vector<unsigned char> buffer;
        std::size_t pos;
    }; // Packet
} // namespace
//---------------------------------------------------------------------
#endif // W32T_PACKET_HPP