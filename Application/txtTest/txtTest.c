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
#include <Protocol/SimpleFileSystem.h>
#include <Library/BaseLib.h>
#include <Protocol/EfiShell.h>
#include <guid/FileInfo.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>

//#include <Uutil.h>
EFI_STATUS
GetFileIo(EFI_FILE_PROTOCOL** Root)
{
	EFI_STATUS  Status = 0;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;//用于操作FAT文件系统，建立在DiskIO之上
	Status = gBS->LocateProtocol(
		&gEfiSimpleFileSystemProtocolGuid,
		NULL,
		(VOID**)&SimpleFileSystem
		);
	printf("locate protocol status:%d\n",Status);
	if (EFI_ERROR(Status)) {
		
		return Status;
	}
	Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, Root);//打开卷，获得FAT系统上的根目录句柄
	printf("open volume status:%d\n",Status);
	return Status;
}
EFI_STATUS TestWrite(EFI_FILE_PROTOCOL *Root, CHAR16 *p) {
	EFI_STATUS  Status = 0;
	EFI_FILE_INFO *info;//文件的详细信息
	UINTN                           FileInfoSize;//文件详细信息的大小
	UINT64                           BufSize = 0;//Buf的大小
	EFI_FILE_PROTOCOL *ReadMe = 0;//文件句柄，文件被视为一种特殊设备
	Status = Root->Open(Root, &ReadMe, (CHAR16*)L"Log.txt", EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);//打开文件，返回文件句柄ReadMe
	printf("open file status:%d\n",Status);
	if (!EFI_ERROR(Status)) {
		info = NULL;
		FileInfoSize = 0;
		Status = ReadMe->GetInfo(ReadMe, &gEfiFileInfoGuid, &FileInfoSize, info);//取得文件信息大小，若空间足够，读取到info中
		printf("get info status:%d\n",Status);
		if (Status == EFI_BUFFER_TOO_SMALL) {
			Status = gBS->AllocatePool(EfiBootServicesCode, FileInfoSize, &info);//给存储文件信息分配内存
			if (info == NULL) {
				Status = EFI_OUT_OF_RESOURCES;
				printf("Open file info fail\n");
				return Status;
			}
			else {
				Status = ReadMe->GetInfo(ReadMe, &gEfiFileInfoGuid, &FileInfoSize, info);
			}
		}
	}
	else {
		printf("Open Log fail\n");
		return Status;
	}
	printf("filesize:         %d\n", info->FileSize);
	if (ReadMe&&!EFI_ERROR(Status)) {
		// Status = ReadMe->Read(ReadMe,
		// 	&info->FileSize,
		// 	Buf
		// 	);
		//strcat(Buf,p);
		Status = ReadMe->SetPosition(ReadMe,info->FileSize);//通过设置文件写入位置追加写入文件
		BufSize = StrLen(p)*2;//Unicode码需要2个字节
		Print(L"time:%s",p);
		Status = ReadMe->Write(ReadMe, &BufSize, p);//写入文件
		ReadMe->Flush(ReadMe);
		printf("bufsize:                %d\n",BufSize);
		printf("filesize:         %d\n", info->FileSize);
		Status = gBS->FreePool(info);
		Status = ReadMe->Close(ReadMe);
	}
	return EFI_SUCCESS;
}

CHAR16* readTime(char *p) {
	char *tmp;
	CHAR16 *result=L"";
	EFI_TIME time;//时钟时间
	EFI_STATUS  Status = 0;
	OUT EFI_TIME_CAPABILITIES cap;//时钟性能
	Status = gRT->GetTime(&time, &cap);//运行时服务提供的获取硬件时间服务
	//StrCat(result, (CHAR16*)time.Year);
	//printf("%s\n", result);
	sprintf(tmp, "%d-", time.Year);//itoa只提供给Windows 所以用sprintf进行字符转换
	strcat(p, tmp);
	sprintf(tmp, "%d-", time.Month);
	strcat(p, tmp);
	sprintf(tmp, "%d----", time.Day);
	strcat(p, tmp);
	sprintf(tmp, "%d:", time.Hour);
	strcat(p, tmp);
	sprintf(tmp, "%d:", time.Minute);
	strcat(p, tmp);
	sprintf(tmp, "%d\n", time.Second);
	strcat(p, tmp);
	printf("time:  %s",p);
	AsciiStrToUnicodeStr(p,result);//通过此函数将char类型转化为CHAR16类型
	return result;
}
//控制系统变量，系统变量需在dec inf中文件设置
UINTN changeVariable(){
	EFI_STATUS status=0;
	UINTN size=10,initTimes=3;
	UINTN oldtimes=0;
	status = gRT->GetVariable((CHAR16 *)L"resetTimes",&gEfiResetTimesVariableGuid,NULL,&size,&oldtimes);//resetTime为自定义的系统变量来设置开机次数，存储在非易失性存储器中
	if(status==EFI_NOT_FOUND){
		status = gRT->SetVariable((CHAR16 *)L"resetTimes",&gEfiResetTimesVariableGuid,EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,size,&initTimes);
	}else{
		oldtimes--;
		status = gRT->SetVariable((CHAR16 *)L"resetTimes",&gEfiResetTimesVariableGuid,EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,size,&oldtimes);
	}
	printf("last times:%d\n",oldtimes);
	if(oldtimes>1){
		gRT->ResetSystem(EfiResetWarm, EFI_SUCCESS,0,NULL);
	}else{
		oldtimes=3;
	}
	return oldtimes;
}
char* readTime2(char *p) {
	char *tmp;
	EFI_TIME time;
	EFI_STATUS  Status = 0;
	OUT EFI_TIME_CAPABILITIES cap;
	Status = gRT->GetTime(&time, &cap);
	sprintf(tmp, "%d-", time.Year);//itoa for windows
	printf("%s\n",tmp);
	strcat(p, tmp);
	printf("%s\n",p);
	sprintf(tmp, "%d-", time.Month);
	strcat(p, tmp);
	sprintf(tmp, "%d----", time.Day);
	strcat(p, tmp);
	sprintf(tmp, "%d:", time.Hour);
	strcat(p, tmp);
	sprintf(tmp, "%d:", time.Minute);
	strcat(p, tmp);
	sprintf(tmp, "%d", time.Second);
	strcat(p, tmp);
	return p;
}
int main(
	int        Argc,
	char     **Argv
	)
{
	char p[50]="";//AsciiStrToUnicodeStr需要const char
	CHAR16* t=L"";
	//UINTN times=0;
	EFI_FILE_PROTOCOL               *Root;
	EFI_STATUS  Status = 0;
	Status = GetFileIo(&Root);
	t=readTime(p);
	//readTime2(p);
	Print(L"time:%s\n",t);
	Status = TestWrite(Root,t);
	if(Status!=EFI_SUCCESS){
		printf("unable to write\n");
	}
	//times=changeVariable();
	return 0;
}