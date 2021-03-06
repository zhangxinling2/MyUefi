
#ifndef _WINDOW_H
#define _WINDOW_H	
#include "common.h"


//***************************  Macro **********************************************
//#define SYGraphicsMode  0x2         //800X600
#define SY_SCREEN_WIDTH   1024 
#define SY_SCREEN_HEIGHT  768

#define BLACK       0
#define WHITE       1
#define LIGHTGRAY   2
#define DEEPBLUE    3
#define DARKBLACK   4
#define LIGHTBLACK  5
#define YELLOW      6
#define GREEN       7
#define RED         8
#define BLUE        9
VOID InSYGraphicsMode(void);
VOID OutSYGraphicsMode(void);
VOID SetBKG(EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);
VOID SimpleTransparent(IN UINTN x,IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer);
VOID AlphaTransparent(IN UINTN x,IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer,IN UINT16 Alpha);
VOID TransferToGray(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height);
VOID ReverseImage(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,UINT8 flag);
VOID hOpenCurtain(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer);
VOID vOpenCurtain(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer);

VOID RainFallShow(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer);
VOID RainAscendShow(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer);

VOID hWindowShadesShow(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer);
VOID vWindowShadesShow(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer);

VOID CornerToCenter(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer);
VOID CenterToCorner(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer);

VOID hInterlaceShow(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer);
VOID vInterlaceShow(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer);

VOID hInteractiveShow(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer);
VOID vInteractiveShow(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer);

VOID SpiralShow(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, UINTN maxX, UINTN maxY, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer);
VOID SpiralShowClear(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color);
#endif  