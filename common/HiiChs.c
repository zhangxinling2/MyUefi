/** @file
    A simple, basic, application showing how the Hello application could be
    built using the "Standard C Libraries" from StdLib.

    Copyright (c) 2010 - 2011, Intel Corporation. All rights reserved.<BR>
    This program and the accompanying materials
    are licensed and made available under the terms and conditions of the BSD License
    which accompanies this distribution. The full text of the license may be found at
    http://opensource.org/licenses/bsd-license.

    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/
//#include  <Uefi.h>
//#include  <Library/UefiLib.h>
//#include  <Library/ShellCEntryLib.h>
//所有包含的目录到mdepkg下去找
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "common.h"
#include "Keyboard.h"
#include "Graphic.h"
#include "Window.h"
#include "HiiFont.h"



extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;
extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL gColorTable[];


INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
	UINT64 flag;
	// const CHAR16* TestStr0=(const CHAR16 *)L"SimpleFont: 举杯邀明月，对影成三人。";
	// const CHAR16* TestStr1=(const CHAR16 *)L"012345ABCabc";
	//const CHAR16* TestStr2=(const CHAR16 *)L"Font: 我歌月徘徊，我舞影零乱。";
	//const CHAR16* TestStr3=(const CHAR16 *)L"012345ABCabc";
    
	flag=InintGloabalProtocols(S_TEXT_INPUT_EX|GRAPHICS_OUTPUT|HII_FONT);
	Print(L"flag=%x\n",flag);
	
  	//注册字体资源包
	LoadFont();
	LoadSimpleFont();
	
	TestStrPackage();
	// //进入图形模式，并设置背景
	//SwitchGraphicsMode(TRUE);
	// SetBKG(&(gColorTable[3]));
	// WaitKey();
	// //显示汉字和其他字符
	// putHiiFontStr(5,0,(CHAR16*)TestStr0,NULL,&(gColorTable[WHITE]),&(gColorTable[DARKBLACK]));
	// putHiiFontStr(5,21,(CHAR16*)TestStr1,NULL,&(gColorTable[DARKBLACK]),&(gColorTable[LIGHTBLACK]));
	// putHiiFontStr(5,50,(CHAR16*)TestStr2,(CHAR16*)L"LUOBING",&(gColorTable[WHITE]),&(gColorTable[DARKBLACK]));
	// putHiiFontStr(5,71,(CHAR16*)TestStr3,(CHAR16*)L"LUOBING",&(gColorTable[DARKBLACK]),&(gColorTable[LIGHTBLACK]));
	// WaitKey();
	Print((const CHAR16*)L"显示文字\n");
	// SetMyMode(0x0);
	// SwitchGraphicsMode(FALSE);


	return 0;
}

