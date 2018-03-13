#include <winsock2.h>
#include <Tools/DriveInfo.hpp>

#include <iphlpapi.h>
#include <sstream>

#include <Network/port32.h>
#include <Tools/winio.h>
//---------------------------------------------------------------------
namespace w32t
{
    DriveInfo::DriveInfo() : m_Init(false), m_HDSerial(""), m_MacAddress("")
    {
    }

    bool DriveInfo::Init()
    {
        if(!m_Init)
        {
            if(ReadPhysicalDriveInNT() < 1)
                m_Init = false;
                
            else m_Init = true;
        }
        return m_Init;
    }

    std::string DriveInfo::getHddSerial()
    {
        m_Init = Init();

        if(!m_Init)
            return "";
        return m_HDSerial;
    }

    std::string DriveInfo::getMacAddress()
    {
        if(m_MacAddress.empty())
        {
            IP_ADAPTER_INFO adapterInfo[16];
            unsigned long adapterSize = sizeof(adapterInfo);

            if(GetAdaptersInfo(adapterInfo, &adapterSize) != ERROR_SUCCESS)
                return std::string(0);

            std::stringstream ss;
            ss << std::hex;

            for(int i = 0; i < 6; i++)
                ss << (int)adapterInfo->Address[i];

            m_MacAddress = ss.str();
        }
        return m_MacAddress;
    }

    std::string DriveInfo::ddToString(size_t diskdata [256], int firstIndex, int lastIndex)
    {
       static char string [1024];
       int index = 0, position = 0;

       for (index = firstIndex; index <= lastIndex; index++)
       {
          //  get high byte for 1st character
          string [position] = (char) (diskdata [index] / 256);
          position++;
          //  get low byte for 2nd character
          string [position] = (char) (diskdata [index] % 256);
          position++;
       }
       string [position] = '\0';

       //  cut off the trailing blanks
       for (index = position - 1; index > 0 && ' ' == string [index]; index--)
          string [index] = '\0';

       return std::string(string);
    }
    // FUNCTION: Send an IDENTIFY command to the drive
    bool DriveInfo::bIdentify(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP, PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum, unsigned long * lpcbBytesReturned)
    {
       // Set up data structures for IDENTIFY command.
       pSCIP -> cBufferSize = idBufferSize;
       pSCIP -> irDriveRegs.bFeaturesReg = 0;
       pSCIP -> irDriveRegs.bSectorCountReg = 1;
       pSCIP -> irDriveRegs.bSectorNumberReg = 1;
       pSCIP -> irDriveRegs.bCylLowReg = 0;
       pSCIP -> irDriveRegs.bCylHighReg = 0;

       // Compute the drive number.
       pSCIP -> irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);

       // IDE identify or ATAPI identify.
       pSCIP -> irDriveRegs.bCommandReg = bIDCmd;
       pSCIP -> bDriveNumber = bDriveNum;
       pSCIP -> cBufferSize = idBufferSize;

        bool ret = (::DeviceIoControl(hPhysicalDriveIOCTL, RecvDriveData, (LPVOID) pSCIP, sizeof(SENDCMDINPARAMS) - 1,
            (LPVOID) pSCOP, sizeof(SENDCMDOUTPARAMS) + idBufferSize - 1, lpcbBytesReturned, nullptr) ) ? true : false;

       return ret;
    }

    int DriveInfo::ReadPhysicalDriveInNT (void)
    {
       size_t done = 0,drive = 0;

        for (drive = 0; drive < maxIdeDrives; drive++)
        {
            std::string driveName("\\\\.\\PhysicalDrive"+std::to_string((long long)drive));

            HANDLE hPhysicalDriveIOCTL = CreateFile (driveName.c_str(), GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, nullptr);

            if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
            {
                GETVERSIONOUTPARAMS VersionParams;
                unsigned long cbBytesReturned = 0;
                memset(&VersionParams, 0, sizeof(VersionParams));

             if (! ::DeviceIoControl (hPhysicalDriveIOCTL, GetVersion, NULL, 0, &VersionParams,
                                      sizeof(VersionParams), &cbBytesReturned, NULL) )
             {
                // getVersion failed // continue;
             }

             if (VersionParams.bIDEDeviceMap > 0)
             {
                BYTE bIDCmd = 0;
                SENDCMDINPARAMS  scip;
                // Now, get the ID sector for all IDE devices in the system.
                // If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
                // otherwise use the IDE_ATA_IDENTIFY command
                bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ?  ATAPI_ID : ATA_ID;

                memset (&scip, 0, sizeof(scip));
                memset (IdOutCmd, 0, sizeof(IdOutCmd));

                if ( bIdentify(hPhysicalDriveIOCTL, &scip, (PSENDCMDOUTPARAMS)&IdOutCmd,(BYTE) bIDCmd, (BYTE) drive, &cbBytesReturned))
                {
                   size_t diskdata [256];
                   USHORT * pIdSector = (USHORT *)((PSENDCMDOUTPARAMS) IdOutCmd)->bBuffer;

                   for (int ijk = 0; ijk < 256; ijk++)
                   {
                      diskdata [ijk] = pIdSector [ijk];
                   }

                   // HDD Serial
                   m_HDSerial = (ddToString(diskdata, 10, 19));

                   done = 1;
                }
            }
             ::CloseHandle (hPhysicalDriveIOCTL);
          }
       }
       return done;
    }

    std::string DriveInfo::Hwid(const std::string & sFirst, const std::string & sSecond)
    {
        std::string ret;
        ret.resize(sFirst.length()+sSecond.length());

        for(size_t i = 0, pos = 0; i < ret.size(); i++)
        {
            if(i < sFirst.length())
            {
                ret[pos] = sFirst[i];
                pos++;
            }
            ret[pos] = sSecond[i];
            pos++;
        }
        return ret;
    }
} // namespace
//---------------------------------------------------------------------