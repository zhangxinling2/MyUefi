#include "BMP.h"




extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;

EFI_STATUS
GetFileIo(EFI_FILE_PROTOCOL** Root)
{
	EFI_STATUS  Status = 0;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;//���ڲ���FAT�ļ�ϵͳ��������DiskIO֮��
	Status = gBS->LocateProtocol(
		&gEfiSimpleFileSystemProtocolGuid,
		NULL,
		(VOID**)&SimpleFileSystem
		);
	if (EFI_ERROR(Status)) {
		
		return Status;
	}
	Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, Root);//�򿪾����FATϵͳ�ϵĸ�Ŀ¼���
	Print(L"OpenVolume status:                   %d\n",Status);
	return Status;
}
//ʵ��256ɫBMPͼ����ʾ�ĺ���
VOID putBMP256(UINTN x,UINTN y,UINTN Width,UINTN Heigth,//λ�ú�ͼ����
EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,//BMP��ɫ������
UINT8 *Picture,//λͼ��������
UINT8 MaskColor//������ɫ��ָ������ʾ������ֵ
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
	VOID *buffBMP=NULL;//�洢ͼ��Ļ�����
	BMP_IMAGE_HEADER imageHeader;//λͼͷ�ļ�
	UINTN bufLength=sizeof(BMP_IMAGE_HEADER);//�ļ�ͷ����
	UINTN index,j,middleValue;//��������ʱʹ�õı���
	UINT8 *middleBuffer,*bmpdata,*bmpplatte;
	Status = bmpFile->Read(bmpFile,&bufLength,&imageHeader);//��ȡBMP�ļ�ͷ
	buffBMP = AllocateZeroPool(imageHeader.Size);//����洢BMP�ļ�����Ҫ���ڴ�
	bmpFile->SetPosition(bmpFile,0);//����λ�õ��ļ��ʼ��
	bufLength = (UINTN)(imageHeader.Size);
	Status = bmpFile->Read(bmpFile,&bufLength,buffBMP);//ȫ����ȡBMP�ļ�
	middleValue = imageHeader.PixelHeight/2;
	bmpdata = (UINT8 *)buffBMP + imageHeader.ImageOffset;//BMPͼ���������λ��
	bmpplatte = (UINT8 *)buffBMP + sizeof(BMP_IMAGE_HEADER);//BMPλͼ��ɫ��λ��
	middleBuffer = AllocateZeroPool(imageHeader.PixelWidth);
	for(j=0;j<middleValue;j++){
		index = imageHeader.PixelHeight -1 -j;
		CopyMem(middleBuffer,bmpdata+j*imageHeader.PixelWidth,imageHeader.PixelWidth);
		CopyMem(bmpdata+j*imageHeader.PixelWidth,bmpdata+index*imageHeader.PixelWidth,imageHeader.PixelWidth);
		CopyMem(bmpdata+index*imageHeader.PixelWidth,middleBuffer,imageHeader.PixelWidth);
	}
	LocateGraphicsOutput();
	//������ɫ���������ݣ�����256ɫBMPͼ����ʾ
	putBMP256(x,y,imageHeader.PixelWidth,imageHeader.PixelHeight,(EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)bmpplatte,bmpdata,0);
	FreePool(buffBMP);
	FreePool(middleBuffer);
	bmpFile->Close(bmpFile);
	return EFI_SUCCESS;
}

EFI_STATUS BuffBlt(CHAR16 *fileName){
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *GopBlt=NULL;			//��ɫ�ṹ��
	EFI_FILE_PROTOCOL *root,*bmpFile;					//�ļ�PROTOCOL
	EFI_FILE_INFO          *ReadFileInfo = NULL;		//�ļ���Ϣ
	EFI_STATUS Status;
	Status = GetFileIo(&root);							//�õ��ļ����
	Status = root->Open(root,&bmpFile,(CHAR16*)fileName,EFI_FILE_MODE_READ,0);//��ָ����file
	Print(L"file status:                   %d\n",Status);
	VOID *DataBuffer=NULL;			//����ͼƬ�Ļ�����
	UINT64 DataLength;				//ͼƬ����������
	UINTN BltSize,Height,Width;		//GOPBlt�Ĵ�С��ͼƬ����ͼƬ��
	ReadFileInfo = ShellGetFileInfo((SHELL_FILE_HANDLE)bmpFile);	//�����й�ָ��������ļ�����Ϣ������洢�ڷ���ĳ��ڴ��С�
	DataLength=ReadFileInfo->FileSize;		//��ͼƬ������������Ϊ�ļ���С
	DataBuffer = (UINT8 *)AllocateZeroPool(DataLength);				//��ͼƬ�����������ڴ�
	Status = bmpFile->Read(bmpFile,&DataLength,DataBuffer);			//��ȡͼƬ�ļ���DataBuffer
	Print(L"Read file status:                   %d\n",Status);
	Status = TranslateBmpToGopBlt(DataBuffer,DataLength,&GopBlt,&BltSize,&Height,&Width);	//��ͼƬת����GOP blt �������С�
	Print(L"Translate status:                   %d\n",Status);
	LocateGraphicsOutput();			//�õ���ͼPROTOCOL
	//��ͼ
	Status = gGraphicsOutput->Blt(gGraphicsOutput,GopBlt,EfiBltBufferToVideo,0,0,0x0,0x0,Width,Height,Width*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
	Print(L"Blt status:                   %d\n",Status);
	Status = gST->ConOut->EnableCursor(gST->ConOut,TRUE);
	FreePool(GopBlt);
	bmpFile->Close(bmpFile);
	FreePool(DataBuffer);
	return Status;
}