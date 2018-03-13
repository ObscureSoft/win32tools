#if !defined(W32T_DRIVEINFO_HPP)
#define W32T_DRIVEINFO_HPP

#include <windows.h>
#include <string>

#pragma pack(1)
//---------------------------------------------------------------------
namespace w32t
{
    class DriveInfo
    {
    protected:
        static const size_t maxIdeDrives = 4, idBufferSize = 512;
        //IOCTL commands
        enum
        {
            GetVersion = 0x00074080,
            //SendDriveCommand = 0x0007c084,
            RecvDriveData = 0x0007c088,
            FileSCSI = 0x0000001b,
            SCSIMiniPortId = (0x0000001b << 16) + 0x0501,
            SCSIMiniPort = 0x0004D008
        };

        struct GETVERSIONOUTPARAMS
        {
           BYTE bVersion;      // Binary driver version.
           BYTE bRevision;     // Binary driver revision.
           BYTE bReserved;     // Not used.
           BYTE bIDEDeviceMap; // Bit map of IDE devices.
           DWORD fCapabilities; // Bit mask of driver capabilities.
           DWORD dwReserved[4]; // For future use.
        };

        // Bits returned in the fCapabilities member of GETVERSIONOUTPARAMS
        enum IDE_COMMAND
        {
            IDE_ATA = 1,
            IDE_ATAPI,
            IDE_NULL,
            IDE_SMART
        };
        enum IDE_ID
        {
            ATAPI_ID = 0xA1,
            ATA_ID = 0xEc
        };

        struct IDSECTOR
        {
           USHORT  wGenConfig;
           USHORT  wNumCyls;
           USHORT  wReserved;
           USHORT  wNumHeads;
           USHORT  wBytesPerTrack;
           USHORT  wBytesPerSector;
           USHORT  wSectorsPerTrack;
           USHORT  wVendorUnique[3];
           CHAR    sSerialNumber[20];
           USHORT  wBufferType;
           USHORT  wBufferSize;
           USHORT  wECCSize;
           CHAR    sFirmwareRev[8];
           CHAR    sModelNumber[40];
           USHORT  wMoreVendorUnique;
           USHORT  wDoubleWordIO;
           USHORT  wCapabilities;
           USHORT  wReserved1;
           USHORT  wPIOTiming;
           USHORT  wDMATiming;
           USHORT  wBS;
           USHORT  wNumCurrentCyls;
           USHORT  wNumCurrentHeads;
           USHORT  wNumCurrentSectorsPerTrack;
           ULONG   ulCurrentSectorCapacity;
           USHORT  wMultSectorStuff;
           ULONG   ulTotalAddressableSectors;
           USHORT  wSingleWordDMA;
           USHORT  wMultiWordDMA;
           BYTE    bReserved[128];
        };
        struct SRB_IO_CONTROL
        {
           ULONG HeaderLength;
           UCHAR Signature[8];
           ULONG Timeout;
           ULONG ControlCode;
           ULONG ReturnCode;
           ULONG Length;
        };

        BYTE IdOutCmd [sizeof (SENDCMDOUTPARAMS) + idBufferSize - 1];

    public:
        DriveInfo();

        bool Init();
        std::string getHddSerial();
        std::string getMacAddress();

        int ReadPhysicalDriveInNT(void);

        std::string Hwid(const std::string&, const std::string&);

    private:
        std::string ddToString(size_t diskdata [256], int firstIndex, int lastIndex);
        bool bIdentify(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP, PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum, unsigned long * lpcbBytesReturned);

        bool m_Init;
        std::string m_HDSerial, m_MacAddress;
    };
} // namespace
//---------------------------------------------------------------------
#endif // W32T_DRIVEINFO_HPP