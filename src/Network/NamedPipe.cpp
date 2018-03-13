#if !defined WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Network/NamedPipe.hpp>
#include <windows.h>
//---------------------------------------------------------------------
namespace w32t
{
    NamedPipe::NamedPipe(const std::string& _pipeName) :
    isConnected(false), bytesRead(0), bytesWritten(0), bufferSize(524), pipeMode(PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT),
    getError(0), pipeName(_pipeName.empty() ? "\\\\.\\pipe\\nPipe" :  ("\\\\.\\pipe\\"+_pipeName) ),
    hPipe(CreateNamedPipe(pipeName.c_str(), PIPE_ACCESS_DUPLEX, pipeMode, PIPE_UNLIMITED_INSTANCES, bufferSize, bufferSize, 0, NULL))
    {
        if(hPipe == INVALID_HANDLE_VALUE) getError = ::GetLastError();
    }

    NamedPipe::~NamedPipe()
    {
        pClose();
    }
    // Connect to named pipe
    bool NamedPipe::pConnect()
    {
        if (hPipe == INVALID_HANDLE_VALUE) return false;

        if(isConnected) pDisconnect();

        isConnected = ( ::ConnectNamedPipe(hPipe, nullptr) > 0 ) ? true : false;
        return isConnected;
    }
    // Open named pipe / Create client.
    bool NamedPipe::pOpen()
    {
        while(true)
        {
            hPipe     = ::CreateFile(pipeName.c_str(), GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
            getError  = ::GetLastError();
            // Pipe handle created successfully.
            if(hPipe == INVALID_HANDLE_VALUE) return false;

            else break;

            if(getError != ERROR_PIPE_BUSY) return false;

            // Pipe instances are busy so wait 10 seconds.
            if(!WaitNamedPipe(pipeName.c_str(), 10000))
            {
                getError = ::GetLastError();
                return false;
            }
        }
        // Pipe connected.
        return true;
    }
    // Disconnect named pipe.
    void NamedPipe::pDisconnect()
    {
        if(isConnected)
        {
            ::DisconnectNamedPipe(hPipe);
            isConnected = false;
        }
    }
    // Close pipe.
    void NamedPipe::pClose()
    {
        if(isConnected) pDisconnect();

        bytesRead   = bytesWritten    = 0;
        hPipe       = INVALID_HANDLE_VALUE;

        CloseHandle(hPipe);
    }

    void* NamedPipe::pGetPipeInstance()
    {
        return this->hPipe;
    }

    unsigned long NamedPipe::pRead(void * _readBuffer, const unsigned long& _size)
    {
        if(hPipe == INVALID_HANDLE_VALUE || isConnected == false) return 0;

        unsigned long daEnd = 0;
        int rSuccess  = 0;

        while(daEnd < _size)
        {
            rSuccess = ::ReadFile(hPipe, _readBuffer, _size, &bytesRead, nullptr);
            if(rSuccess < 1) break;
            daEnd += rSuccess;
        }

        if(rSuccess == 0 || bytesRead == 0) return 0;

        return bytesRead;
    }

    unsigned long NamedPipe::pWrite(const void * _writeBuffer, const unsigned long& _size)
    {
        if(hPipe == INVALID_HANDLE_VALUE) return 0;

        unsigned long daEnd = 0;
        int wSuccess  = 0;

        while(daEnd < _size)
        {
            wSuccess = ::WriteFile(hPipe, _writeBuffer, _size, &bytesWritten, nullptr);
            if(wSuccess < 1) break;
            daEnd += wSuccess;
        }

        if(wSuccess == 0 || bytesWritten == 0) return 0;

        return bytesWritten;
    }
} // namespace
//---------------------------------------------------------------------