#include "BMP.h"




extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;

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
	//画图
	Status = gGraphicsOutput->Blt(gGraphicsOutput,GopBlt,EfiBltBufferToVideo,0,0,0x0,0x0,Width,Height,Width*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
	Print(L"Blt status:                   %d\n",Status);
	Status = gST->ConOut->EnableCursor(gST->ConOut,TRUE);
	FreePool(GopBlt);
	bmpFile->Close(bmpFile);
	FreePool(DataBuffer);
	return Status;
}