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
//���а�����Ŀ¼��mdepkg��ȥ��
/**
 * ��UEFI���ʵ�����������
 * �����UEFI̽�������Բο����ߵĲ��ͺ�ר����
 * CSDN: https://blog.csdn.net/luobing4365
 * ֪��: https://www.zhihu.com/column/c_1233025362843209728
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
	const CHAR16* TestStr0=(const CHAR16 *)L"SimpleFont: �ٱ������£���Ӱ�����ˡ�";
	const CHAR16* TestStr1=(const CHAR16 *)L"012345ABCabc";
	const CHAR16* TestStr2=(const CHAR16 *)L"Font: �Ҹ����ǻ�������Ӱ���ҡ�";
	const CHAR16* TestStr3=(const CHAR16 *)L"012345ABCabc";
    
	flag=InintGloabalProtocols(S_TEXT_INPUT_EX|GRAPHICS_OUTPUT|HII_FONT);
	Print(L"flag=%x\n",flag);
	
  	//ע��������Դ��
	LoadFont();
	LoadSimpleFont();
	
	TestStrPackage();
	Print(L"���UEFI!\n");
	Print(L"���ܧ�����  ������ܧڧ�\n");
	Print(L"???\n");
	
	Print(L"???\n");
	Print(L"???\n");
	// //����ͼ��ģʽ�������ñ���
	//SwitchGraphicsMode(TRUE);
	// SetBKG(&(gColorTable[3]));
	// WaitKey();
	//��ʾ���ֺ������ַ�
	putHiiFontStr(5,0,(CHAR16*)TestStr0,NULL,&(gColorTable[WHITE]),&(gColorTable[DARKBLACK]));
	putHiiFontStr(5,21,(CHAR16*)TestStr1,NULL,&(gColorTable[DARKBLACK]),&(gColorTable[LIGHTBLACK]));
	putHiiFontStr(5,50,(CHAR16*)TestStr2,(CHAR16*)L"LUOBING",&(gColorTable[WHITE]),&(gColorTable[DARKBLACK]));
	putHiiFontStr(5,71,(CHAR16*)TestStr3,(CHAR16*)L"LUOBING",&(gColorTable[DARKBLACK]),&(gColorTable[LIGHTBLACK]));
	// WaitKey();
	// SetMyMode(0x0);
	// SwitchGraphicsMode(FALSE);


	return 0;
}

