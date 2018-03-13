#ifndef WINIO_H
#define WINIO_H

#define FILE_DEVICE_WINIO 0x00008010
#define WINIO_IOCTL_INDEX 0x810

// Define our own private IOCTL

#define IOCTL_WINIO_MAPPHYSTOLIN  CTL_CODE(FILE_DEVICE_WINIO, WINIO_IOCTL_INDEX, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_WINIO_UNMAPPHYSADDR CTL_CODE(FILE_DEVICE_WINIO, WINIO_IOCTL_INDEX + 1, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_WINIO_WRITEPORT CTL_CODE(FILE_DEVICE_WINIO, WINIO_IOCTL_INDEX + 2, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_WINIO_READPORT CTL_CODE(FILE_DEVICE_WINIO, WINIO_IOCTL_INDEX + 3, METHOD_BUFFERED, FILE_ANY_ACCESS)

struct tagPhys32Struct
{
  HANDLE PhysicalMemoryHandle;
  ULONG dwPhysMemSizeInBytes;
  PVOID pvPhysAddress;
  PVOID pvPhysMemLin;
};

extern struct tagPhys32Struct Phys32Struct;

struct tagPort32Struct
{
  USHORT wPortAddr;
  ULONG dwPortVal;
  UCHAR bSize;
};

extern struct tagPort32Struct Port32Struct;

#endif