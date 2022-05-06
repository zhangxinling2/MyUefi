#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/SimpleTextInEx.h>

EFI_STATUS setTime(char *p) {
	EFI_STATUS Status;
	UINTN index = 0;
	long int times = 5;
	times = strtol(p, NULL, 10);
	EFI_EVENT event;
	Status = gBS->CreateEvent(EVT_TIMER, TPL_APPLICATION, (EFI_EVENT_NOTIFY)NULL, (VOID *)NULL, &event);
	Status = gBS->SetTimer(event, TimerPeriodic, 10 * 1000 * 1000);
	while (1)
	{
		Status = gBS->WaitForEvent(1, &event, &index);
		printf("Time Wait %d\n", times);
		times--;
		if (times<0) {//> < ?
			break;
		}
	}
	Status = gBS->CloseEvent(event);

	return Status;
}
// INIT CharToInt(IN CHAR16 *time){//char 转化为int
//     return 0;
// }
//程序原理：首先用 LocateProtocol 取得EFI_SIMPLE_POINTER_PROTOCOL （这里假设系统中只有一个），然后做一次 Reset ，通过这个动作也确定设备是否可以使用。
//之后，不断使用 GetState 轮询，如果发现前后两次获得的信息不同那么就输出解析结果，打印当前鼠标的状态。
EFI_STATUS
testMouseSimple()
{
	EFI_STATUS  Status;
	EFI_SIMPLE_POINTER_PROTOCOL* mouse = 0;
	EFI_SIMPLE_POINTER_STATE     State;
	EFI_EVENT events[2]; // = {0, gST->ConIn->WaitForKey};
						 //显示光标
	gST->ConOut->EnableCursor(gST->ConOut, TRUE);
	//找出鼠标设备
	Status = gBS->LocateProtocol(
		&gEfiSimplePointerProtocolGuid,
		NULL,
		(VOID**)&mouse
		);
	// 重置鼠标设备
	Status = mouse->Reset(mouse, TRUE);
	// 将鼠标事件放到等待事件数组
	events[0] = mouse->WaitForInput;
	// 将键盘事件放到等待数组
	events[1] = gST->ConIn->WaitForKey;
	while (1)
	{
		EFI_INPUT_KEY	   Key;
		UINTN index;
		// 等待events中的任一事件发生
		Status = gBS->WaitForEvent(2, events, &index);
		if (index == 0) {
			// 获取鼠标状态并输出
			Status = mouse->GetState(mouse, &State);
			Print(L"X:%d Y:%d Z:%d L:%d R:%d\n",
				State.RelativeMovementX,
				State.RelativeMovementY,
				State.RelativeMovementZ,
				State.LeftButton,
				State.RightButton
				);
		}
		else {
			Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
			// 按’q’键退出
			if (Key.UnicodeChar == 'q')
				break;
		}
	}
	return EFI_SUCCESS;
}
int main(
	int        Argc,
	char     **Argv
	)
{
	EFI_STATUS Status;
	Status=setTime(Argv[1]);//命名不能与已有函数重合 time已经包含在头文件中。
	Status = testMouseSimple();
	return 0;
}