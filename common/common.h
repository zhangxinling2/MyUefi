#ifndef _COMMON_H
#define _COMMON_H
//所有包含的目录到mdepkg下去找

#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>
#include  <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/AbsolutePointer.h>
#include <guid/FileInfo.h>
#include <Library/IoLib.h>
#include <Library/BmpSupportLib.h>
#include <IndustryStandard/Bmp.h>
#include <Library/ShellLib.h>
#include <Library/BaseMemoryLib.h>

#include <Protocol/HiiFont.h>
#include <Protocol/HiiDatabase.h>
#include <Library/HiiLib.h>
#include <Library/UefiHiiServicesLib.h>


#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/SimpleTextInEx.h>
//#include <Library/PciLib.h>
//#include <Library/PciExpressLib.h>
//#include <IndustryStandard/Pci.h>
#include <Protocol/SimplePointer.h>		//获取鼠标?
#include <Protocol/SerialIo.h>	//串口？
#include <Protocol/PciIo.h>						//获取PciIO protocol所需
#include <Protocol/PciRootBridgeIo.h>	//获取PciRootBridgeIO protocol所需
#include <IndustryStandard/Pci.h>  //pci访问所需的头文件，包含pci22.h,pci23.h...

#define S_TEXT_INPUT_EX  		0x0000000000000001
#define GRAPHICS_OUTPUT  		0x0000000000000002
#define PCI_ROOTBRIDGE_IO		0x0000000000000004
#define PCI_IO							0x0000000000000008
#define FILE_IO             0x0000000000000010  //2019-06-10 19:53:27 luobing
#define HII_FONT						0x0000000000000020  //2019-6-8 7:39:47 luobing
#define HII_IMAGE           0x0000000000000040  //2020-9-27 16:32:22 luobing
#define HII_IMAGE_EX        0x0000000000000080  //2020-9-27 16:32:23 luobing
#define HII_IMAGE_DECODER   0x0000000000000100  //2020-9-27 16:32:24 luobing
#define RNG_OUT             0x0000000000000200  //2019-08-31 11:33:12 robin
#define SIMPLE_POINTER      0x0000000000000400  //2020-09-31 13:31:18 robin
#define MOUSE_POINT         0x0000000000000800
UINT64 InintGloabalProtocols(UINT64 flag);
EFI_STATUS LocateSimpleTextInputEx(void);
EFI_STATUS LocateGraphicsOutput (void);
EFI_STATUS LocatePCIRootBridgeIO(void);
EFI_STATUS LocatePCIIO(void);

EFI_STATUS LocateMouse(void);
VOID Delayms(IN UINTN ms);


#endif