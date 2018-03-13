#include <Network/Packet.hpp>

#include <cstring>
#include <iterator>
//---------------------------------------------------------------------
namespace w32t
{
    Packet::Packet() : pos(0)
    {
    }
    Packet::~Packet()
    {

    }

    Packet & Packet::operator << (const char * data)
    {
        std::uint16_t dataSize = (std::uint16_t)strlen(data) + 1;
        if(dataSize > 1)
        {
            buffer.reserve(dataSize+sizeof(data));
            // copy size of data
            std::copy(&dataSize, &dataSize + sizeof(dataSize), std::back_inserter(this->buffer));
            std::copy(&data[0], &data[0] + dataSize, std::back_inserter(this->buffer));
        }

        return *this;
    }

    Packet & Packet::operator << (const unsigned int& data)
    {
        std::uint8_t const * p = reinterpret_cast<std::uint8_t const*>(&data);
        std::copy(p, p + sizeof(data), std::back_inserter(buffer));
        return *this;
    }

    Packet& Packet::operator >> (char * data)
    {
        std::uint16_t dataSize = 0;
        if(!this->buffer.empty())
        {
            // retrieve size of data
            std::copy(&this->buffer[pos], &this->buffer[pos] + sizeof(dataSize), &dataSize);
            pos += sizeof(dataSize);

            std::copy(&this->buffer[pos], &this->buffer[pos] + dataSize, data);
            pos += dataSize;
        }
        data[dataSize] = '\0';

        return *this;
    }

    Packet& Packet::operator >> (unsigned int & data)
    {
        std::copy(&buffer[pos], &buffer[pos] + sizeof(data), reinterpret_cast<char*>(&data));
        pos += sizeof(data);
        return *this;
    }

    std::size_t Packet::getSize()
    {
        return this->buffer.size();
    }
} // namespace
//---------------------------------------------------------------------