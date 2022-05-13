#ifndef _FILERW_H
#define _FILERW_H 
#include "common.h"

EFI_STATUS OpenFile(EFI_FILE_PROTOCOL **fileHandle, CHAR16 *fileName, UINT64 OpenMode);
EFI_STATUS ReadFile(EFI_FILE_PROTOCOL *fileHandle, UINTN *bufSize, VOID *buffer);
EFI_STATUS WriteFile(EFI_FILE_PROTOCOL *fileHandle, UINTN *bufSize, VOID *buffer);
EFI_STATUS SetFilePosition(EFI_FILE_PROTOCOL *fileHandle, UINT64 position);
EFI_STATUS GetFilePosition(EFI_FILE_PROTOCOL *fileHandle, UINT64 *position);

#endif