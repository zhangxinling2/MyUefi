#ifndef _BMP256_H
#define _BMP256_H
#include "common.h"
#include "FileRW.h"
#include "Graphic.h"

VOID putBMP256(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor);
EFI_STATUS BuffBlt(CHAR16 *fileName);
EFI_STATUS GetFileIo(EFI_FILE_PROTOCOL** Root);
#endif