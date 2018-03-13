#if !defined(W32T_SOCKET_HPP)
#define W32T_SOCKET_HPP

#include <string>
#include <winsock2.h>
//---------------------------------------------------------------------
namespace w32t
{
    class Socket
    {
    public:

        ~Socket();

        std::string     getError() const;
        void            eraseError(const size_t & lenth);
        unsigned short  getLocalPort();
        std::string     getLocalAddress();
        bool            setBlock(bool blockingFlag);

        enum class Type
        {
            Tcp = 1,
            Udp
        };
        
    protected:
        Socket(Type);

        virtual bool create();

        void        create(SOCKET hSocket);
        void        close();
        SOCKET      getSocket() const;
        std::string errorString;

    private:
        Type        m_type;
        SOCKET      m_socket;
    }; // Socket
} // namespace
//---------------------------------------------------------------------
#endif // W32T_SOCKET_HPP