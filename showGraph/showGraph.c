#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
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
#include <Library/BaseMemoryLib.h>
#include <guid/FileInfo.h>
#include <Library/IoLib.h>
#include <Library/BmpSupportLib.h>
#include <IndustryStandard/Bmp.h>
#include <Library/ShellLib.h>
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
	if (EFI_ERROR(Status)) {
		
		return Status;
	}
	Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, Root);//打开卷，获得FAT系统上的根目录句柄
	Print(L"OpenVolume status:                   %d\n",Status);
	return Status;
}
EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;
//找到图形输出Protocol
EFI_STATUS LocateGraphicsOutput (void)
{
	EFI_STATUS                         Status;

	EFI_HANDLE                         *GraphicsOutputControllerHandles = NULL;
	UINTN                              HandleIndex = 0;
	UINTN                              HandleCount = 0;
	//get the handles which supports GraphicsOutputProtocol
	Status = gBS->LocateHandleBuffer(
		ByProtocol,
		&gEfiGraphicsOutputProtocolGuid,
		NULL,
		&HandleCount,
		&GraphicsOutputControllerHandles
		);//从系统Handle数据库中找出支持此Protocol的Handle
	if (EFI_ERROR(Status))	return Status;		//unsupport
	for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
	{
		Status = gBS->HandleProtocol(
			GraphicsOutputControllerHandles[HandleIndex],
			&gEfiGraphicsOutputProtocolGuid,
			(VOID**)&gGraphicsOutput);
		if (EFI_ERROR(Status))	continue;

		else
		{
			return EFI_SUCCESS;
		}
	}
	return Status;
}
// EFI_STATUS showGraph2(){
// 	UINT32 frameWidth,frameHeight;
// 	frameWidth = (UINT32)gGraphicsOutput->Mode->Info->HorizontalResolution;
// 	frameHeight = (UINT32)gGraphicsOutput->Mode->Info->VerticalResolution;
	
// }
// VOID line(UINTN x1,UINTN y1,UINTN x2,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color){
//     INTN d,dx,dy,dx2,dy2,dxy;
//     INTN xinc,yinc;
//     dx=(INTN)((x2>x1)?(x2-x1):(x1-x2));//直线在x轴的差值
//     dx=dx<<1;//得到两倍的差值用以计算
//     dy=(INTN)((y2>y1)?(y2-y1):(y1-y2));
//     dy2=dy<<1;
//     xinc=(x2>x1)?1:((x2==x1)?0:-1);//正向1 平0 反向-1
//     yinc=(y2>y1)?1:((y2==y1)?0:-1);
//     if(dx>=dy){//斜率小于1
//         d=dy2-dx;//计算第一步的决策参数 2△y-△x
//         dxy=dy2-dx2;//决策参数中的2△y-2△x
//         while (dx--)
//         {
//             if(d<=0)d+=dy2;//如果决策参数d<0则y距离下侧较近 此时dk+1 = dk + 2△y y不变
//             else{//如果决策参数d>0 则y距离上侧较近 此时 dk+1 = dk+2△y-2△x y移动一位
//                 d+=dxy;
//                 y1+=yinc;
//             }
//             putpixel(x1+=xinc,y1,color);//每次x增1
//         }
//     }else{//斜率大于1
//         d=dx2-dy;//计算第一步的决策参数 2△x-△y
//         dxy=dx2-dy2;//决策参数中的2△x-2△y
//         while (dy--)
//         {
//             if(d<=0)d+=dx2;//如果决策参数d<0则x距离左侧侧较近 此时dk+1 = dk + 2△x x不变
//             else{//如果决策参数d>0 则x距离右侧较近 此时 dk+1 = dk+2△x-2△y x移动一位
//                 d+=dxy;
//                 x1+=xinc;
//             }
//             putpixel(x1,y1+=yinc,color);
//         }
//     }
// }
VOID putpixel(UINTN x,UINTN y,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color){
    gGraphicsOutput->Blt(gGraphicsOutput,color,EfiBltVideoFill,0,0,x,y,1,1,0);
}
//实现256色BMP图像显示的函数
VOID putBMP256(UINTN x,UINTN y,UINTN Width,UINTN Heigth,//位置和图像宽高
EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,//BMP颜色表数据
UINT8 *Picture,//位图点阵数据
UINT8 MaskColor//掩码颜色，指定不显示的索引值
){
	UINTN i,j;
	UINT32 index=0;
	UINT8 color_number;
	for(j=y;j<Heigth+y;j++){
		for(i=x;i<Width+x;i++){
			color_number=Picture[index];
			if(color_number!=MaskColor)
				putpixel(i,j,&(ColorTable[color_number]));
			++index;
		}
	}
}
EFI_STATUS ShowBMP256(CHAR16 *fileName,UINTN x,UINTN y){
	EFI_FILE_PROTOCOL *root,*bmpFile;
	EFI_STATUS Status;
	//
	Status = GetFileIo(&root);
	Status = root->Open(root,&bmpFile,(CHAR16*)fileName,EFI_FILE_MODE_READ,0);
	VOID *buffBMP=NULL;//存储图像的缓冲器
	BMP_IMAGE_HEADER imageHeader;//位图头文件
	UINTN bufLength=sizeof(BMP_IMAGE_HEADER);//文件头长度
	UINTN index,j,middleValue;//交换数据时使用的变量
	UINT8 *middleBuffer,*bmpdata,*bmpplatte;
	Status = bmpFile->Read(bmpFile,&bufLength,&imageHeader);//读取BMP文件头
	buffBMP = AllocateZeroPool(imageHeader.Size);//分配存储BMP文件所需要的内存
	bmpFile->SetPosition(bmpFile,0);//设置位置到文件最开始处
	bufLength = (UINTN)(imageHeader.Size);
	Status = bmpFile->Read(bmpFile,&bufLength,buffBMP);//全部读取BMP文件
	middleValue = imageHeader.PixelHeight/2;
	bmpdata = (UINT8 *)buffBMP + imageHeader.ImageOffset;//BMP图像点阵数据位置
	bmpplatte = (UINT8 *)buffBMP + sizeof(BMP_IMAGE_HEADER);//BMP位图颜色表位置
	middleBuffer = AllocateZeroPool(imageHeader.PixelWidth);
	for(j=0;j<middleValue;j++){
		index = imageHeader.PixelHeight -1 -j;
		CopyMem(middleBuffer,bmpdata+j*imageHeader.PixelWidth,imageHeader.PixelWidth);
		CopyMem(bmpdata+j*imageHeader.PixelWidth,bmpdata+index*imageHeader.PixelWidth,imageHeader.PixelWidth);
		CopyMem(bmpdata+index*imageHeader.PixelWidth,middleBuffer,imageHeader.PixelWidth);
	}
	LocateGraphicsOutput();
	//给定颜色表，点阵数据，进行256色BMP图像显示
	putBMP256(x,y,imageHeader.PixelWidth,imageHeader.PixelHeight,(EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)bmpplatte,bmpdata,0);
	FreePool(buffBMP);
	FreePool(middleBuffer);
	bmpFile->Close(bmpFile);
	return EFI_SUCCESS;
}
//输出分辨率
VOID showInfo(){
	//显示模式信息结构体
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info=NULL;
	UINTN infosize;//模式信息大小
	//使用QueryMode查询
	gGraphicsOutput->QueryMode(gGraphicsOutput,gGraphicsOutput->Mode->Mode,&infosize,&info);
	Print(L"Horizontal:%x\n",info->HorizontalResolution);
	Print(L"Vertical:%x\n",info->VerticalResolution);
}
EFI_STATUS BuffBlt(CHAR16 *fileName){
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *GopBlt=NULL;			//颜色结构体
	EFI_FILE_PROTOCOL *root,*bmpFile;					//文件PROTOCOL
	EFI_FILE_INFO          *ReadFileInfo = NULL;		//文件信息
	EFI_STATUS Status;
	Status = GetFileIo(&root);							//得到文件句柄
	Status = root->Open(root,&bmpFile,(CHAR16*)fileName,EFI_FILE_MODE_READ,0);//打开指定的file
	Print(L"file status:                   %d\n",Status);
	VOID *DataBuffer=NULL;			//保存图片的缓冲区
	UINT64 DataLength;				//图片缓冲区长度
	UINTN BltSize,Height,Width;		//GOPBlt的大小，图片长，图片宽
	ReadFileInfo = ShellGetFileInfo((SHELL_FILE_HANDLE)bmpFile);	//检索有关指定句柄的文件的信息并将其存储在分配的池内存中。
	DataLength=ReadFileInfo->FileSize;		//将图片缓冲区长度设为文件大小
	DataBuffer = (UINT8 *)AllocateZeroPool(DataLength);				//给图片缓冲区分配内存
	Status = bmpFile->Read(bmpFile,&DataLength,DataBuffer);			//读取图片文件到DataBuffer
	Print(L"Read file status:                   %d\n",Status);
	Status = TranslateBmpToGopBlt(DataBuffer,DataLength,&GopBlt,&BltSize,&Height,&Width);	//将图片转换到GOP blt 缓冲区中。
	Print(L"Translate status:                   %d\n",Status);
	LocateGraphicsOutput();			//得到画图PROTOCOL
	showInfo();						//输出信息
	//画图
	Status = gGraphicsOutput->Blt(gGraphicsOutput,GopBlt,EfiBltBufferToVideo,0,0,0x0,0x0,Width,Height,Width*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
	Print(L"Blt status:                   %d\n",Status);
	Status = gST->ConOut->EnableCursor(gST->ConOut,TRUE);
	FreePool(GopBlt);
	bmpFile->Close(bmpFile);
	FreePool(DataBuffer);
	return Status;
}

VOID BresenhamLine(UINTN x1,UINTN y1,UINTN x2,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color){
	INTN d,dx,dy,dx2,dy2,dxy;
	INTN xinc,yinc;
	dx=(INTN)((x2>x1)?(x2-x1):(x1-x2));//计算x差值
	dx2=dx<<1;						   //左移一位，x差值乘2
	dy=(INTN)((y2>y1)?(y2-y1):(y1-y2));//计算y差值
	dy2=dy<<2;						   //左移一位，y差值乘2
	xinc=(x2>x1)?1:((x2==x1)?0:-1);	   //目标地大于起始地则返回1，持平则返回0，小于则返回-1
	yinc=(y2>y1)?1:((y2==y1)?0:-1);
	putpixel(x1,y1,color);			   //画出起始点
	Print(L"put thr first point\n");
	if(dx>dy){						   //斜率小于1
		d=dy2-dx;					   //确定第一步决策参数
		dxy=dy2-dx2;				   //决策参数中的常数  决策参数为pk+1=pk+2(dy-dx)
		while (dx--)
		{
			if(d<0) d+=dy2;			   //上一步决策参数小于0时的公式
			else{					   //大于0时的公式
				d+=dxy;
				y1+=yinc;
			}
			putpixel(x1+=xinc,y1,color);
		}
	}else{
		d=dx2-dy;					   
		dxy=dx2-dy2;				   
		while (dy--)
		{
			if(d<0) d+=dx2;			   
			else{					   
				d+=dxy;
				x1+=xinc;
			}
			putpixel(x1,y1+=yinc,color);
		}
	}
}
INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  ){
	  //EFI_STATUS Status;
	  //Status = BuffBlt(Argv[1]);
	  //Status=ShowBMP256(Argv[1],0,0);
	  BresenhamLine(0,0,80,80,0);
	  return 0;
}