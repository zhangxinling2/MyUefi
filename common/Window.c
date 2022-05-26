//code by luobing 2018-5-15 10:19:35
/**
 * ��UEFI���ʵ�����������
 * �����UEFI̽�������Բο����ߵĲ��ͺ�ר����
 * CSDN: https://blog.csdn.net/luobing4365
 * ֪��: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "Window.h"
#include "Graphic.h"

UINT32 SYGraphicsMode;
UINT32 OldGraphicsMode;
UINT32 SYPixPerScanLine;

//============================== Global color table ================================
EFI_GRAPHICS_OUTPUT_BLT_PIXEL gColorTable[]={
  0,    0,    0,    0,        /*  BLACK */\
  255,  255,  255,  0,        /*  WHITE */\
  200,  208,  212,  0,        /*  LIGHT GRAY  */\
  167,  95, 18, 0,        /*  DEEP BLUE */\
  64, 64, 64, 0,        /*  DARK BLACK  */\
  128,  128,  128,  0,        /*  LIGHT BLACK */\
  0,    255,  255,  0,        /*  YELLOW  */\
  0,    153,  0,    0,        /*  GREEN */\
  0,    0,    153,  0,        /*  RED   */\
  192,  0,    0,    0,        /* BLUE   */  
};


//Function Name: SetBKG
//Input: color
//Output: None
//Description: set background
VOID SetBKG(EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
	GetGraphicModeNumber(SY_SCREEN_WIDTH,SY_SCREEN_HEIGHT,&SYGraphicsMode,&OldGraphicsMode,&SYPixPerScanLine);
	SetMyMode(SYGraphicsMode);
  rectblock(0,0,SY_SCREEN_WIDTH-1,SY_SCREEN_HEIGHT-1,color);
}
//Description: set mode
VOID InSYGraphicsMode(void)
{
	GetGraphicModeNumber(SY_SCREEN_WIDTH,SY_SCREEN_HEIGHT,&SYGraphicsMode,&OldGraphicsMode,&SYPixPerScanLine);
	SetMyMode(SYGraphicsMode);
}

//Description: set old mode
VOID OutSYGraphicsMode(void)
{
	//GetGraphicModeNumber(SY_SCREEN_WIDTH,SY_SCREEN_HEIGHT,&SYGraphicsMode,&OldGraphicsMode,&SYPixPerScanLine);
	SetMyMode(OldGraphicsMode);
}

//Description: ImagePointer,ָ��Width*Height��С��ͼ�񻺳��������С�ض�Ϊ(UINT32)(Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL))
//ʵ����򵥵�͸��Ч������ʹ��Alpha��ʽ��AlphaֵΪ1/2
VOID SimpleTransparent(IN UINTN x,IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer){
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
  UINT32                        BltBufferSize;
  UINT32                        i;
  //����ͬ����С���ڴ�׼��ʹ��
  BltBufferSize = (UINT32)(Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  BltBuffer = AllocateZeroPool(BltBufferSize);
  //Ҫ���������ͼ����ȡ
  getRectImage(x,y,Width,Height,BltBuffer);
  //�������ڴ���д���ѡȡAlpha=0.5
  //Alpha��Ϲ�ʽ���£�(A������B��)
	//                  R(C)=(1-alpha)*R(B) + alpha*R(A)
	//                  G(C)=(1-alpha)*G(B) + alpha*G(A)
	//                  B(C)=(1-alpha)*B(B) + alpha*B(A)
  for(i=0;i<(UINT32)Width*(UINT32)Height;i++){
    BltBuffer[i].Blue=(BltBuffer[i].Blue>>1)+(ImagePointer[i].Blue>>1);
    BltBuffer[i].Green=(BltBuffer[i].Green>>1)+(ImagePointer[i].Green>>1);
    BltBuffer[i].Red=(BltBuffer[i].Red>>1)+(ImagePointer[i].Red>>1);
    BltBuffer[i].Reserved=(BltBuffer[i].Reserved>>1)+(ImagePointer[i].Reserved>>1);
  }
  //��ʾ
  putRectImage(x,y,Width,Height,BltBuffer);
  FreePool(BltBuffer);
}
//ʵ��͸��Ч����ʹ��Alpha��ʽ��Alphaֵ�ɵ�,��0-16�ɵ�
VOID AlphaTransparent(IN UINTN x,IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer,IN UINT16 Alpha){
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
  UINT32                        BltBufferSize;
  UINTN                        i;
  //����ͬ����С���ڴ�׼��ʹ��
  BltBufferSize = (UINT32)((UINT32)(Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)));
  BltBuffer = AllocateZeroPool(BltBufferSize);
  //Ҫ���������ͼ����ȡ
  getRectImage(x,y,Width,Height,BltBuffer);
  //�������ڴ���д���ѡȡAlpha=0.5
  //Alpha��Ϲ�ʽ���£�(A������B��)
	//                  R(C)=(1-alpha)*R(B) + alpha*R(A)
	//                  G(C)=(1-alpha)*G(B) + alpha*G(A)
	//                  B(C)=(1-alpha)*B(B) + alpha*B(A)
  for(i=0;i<Width*Height;i++){
    BltBuffer[i].Blue=(BltBuffer[i].Blue*(16-(UINT8)Alpha))+(ImagePointer[i].Blue*(UINT8)Alpha);
    BltBuffer[i].Green=(BltBuffer[i].Green*(16-(UINT8)Alpha))+(ImagePointer[i].Green*(UINT8)Alpha);
    BltBuffer[i].Red=(BltBuffer[i].Red*(16-(UINT8)Alpha))+(ImagePointer[i].Red*(UINT8)Alpha);
    BltBuffer[i].Reserved=(BltBuffer[i].Reserved*(16-(UINT8)Alpha))+(ImagePointer[i].Reserved*(UINT8)Alpha);
  }
  //��ʾ
  putRectImage(x,y,Width,Height,BltBuffer);
  FreePool(BltBuffer);
}
//Description:��ָ�������ͼ��ת��Ϊ�Ҷ�
VOID TransferToGray(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height){
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBufferSrc;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBufferDst;
  UINT32                        BltBufferSize;
  UINT32                        i;
  UINT8                         temp;
  //������ͬ��С���ڴ�ʹ��
  BltBufferSize = (UINT32)(Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  BltBufferSrc = AllocateZeroPool(BltBufferSize);
  BltBufferDst = AllocateZeroPool(BltBufferSize);
  //��ȡͼ��
  getRectImage(x,y,Width,Height,BltBufferSrc);
  //�Ҷȴ�������ʾ
  //һ��ľ���ֵ�Ҷ�ת��Ϊ���Ҷ�ֵ=0.3x��ɫ����  + 0.59x��ɫ���� + 0.11x��ɫ����
	//�޸�һ��:Red*32/128    Green*64/128   Blue*16/128
	//Ҳ��:  Red>>2   Green>>1   Blue>>3
  for(i=0;i<Width*Height;i++){
    temp = ((BltBufferSrc[i].Red) >> 2) + ((BltBufferSrc[i].Green) >> 1) + ((BltBufferSrc[i].Blue) >> 3);
    BltBufferDst[i].Red = temp;
    BltBufferDst[i].Green = temp;
    BltBufferDst[i].Blue = temp;
    BltBufferDst[i].Reserved = 0;
  }
  putRectImage(x,y,Width,Height,BltBufferDst);
  FreePool(BltBufferSrc);
  FreePool(BltBufferDst);
}

//��ָ�������ͼ��ߵ� Flag=1:���µߵ���Flag=2:���ҵߵ�
VOID ReverseImage(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,UINT8 flag){
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBufferSrc;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBufferDst;
  UINT32                        BltBufferSize;
  UINT32                        position;
  INTN                          xx,yy;
  //������ͬ��С���ڴ�ʹ��
  BltBufferSize = (UINT32)(Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  BltBufferSrc = AllocateZeroPool(BltBufferSize);
  BltBufferDst = AllocateZeroPool(BltBufferSize);
  //��ȡͼ��
  getRectImage(x,y,Width,Height,BltBufferSrc);

  position = 0;
  if(flag&0x01){
    for(yy=(UINTN)Height - 1;yy>=0;y--){
      CopyMem(BltBufferDst+position,BltBufferSrc+yy*Width,Width*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
      position+=(UINT32)Width;
    }
  }
  if(flag&0x02){
    gBS->SetMem(BltBufferDst,BltBufferSize,0);
    for (yy = 0; yy < (INTN)Height; yy++)
		{
			for (xx = (INTN)Width - 1; xx >= 0; xx--)
			{
				CopyMem(&(BltBufferDst[yy * Width + Width - 1 - xx]), &(BltBufferSrc[yy * Width + xx]), sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
			}
		}
  }
  putRectImage(x, y, Width, Height, BltBufferDst);
			
 	gBS->FreePool(BltBufferSrc);
 	gBS->FreePool(BltBufferDst);
}
//Description:������ˮƽ��ͼ������Ļ��Ч��
VOID hOpenCurtain(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer){
  UINTN i;
  for(i=0;i<Width/2;i++){
    putPartRectImage(0,0,Width,Height,Width/2+i,0,1,Height,ImagePointer);
    putPartRectImage(0,0,Width,Height,Width/2-i-1,0,1,Height,ImagePointer);
    Delayms(6);
  }
}
//Description:�����봹ֱ��ͼ������Ļ��Ч��
VOID vOpenCurtain(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer){
  UINTN i;
  for(i=0;i<Height/2;i++){
    putPartRectImage(0,0,Width,Height,0,Height/2+i,Width,1,ImagePointer);
    putPartRectImage(0,0,Width,Height,0,Height/2-i-1,Width,1,ImagePointer);
    Delayms(6);
  }
}
//Description:ͼ���������Ч����ʾ�����µ���
VOID RainFallShow(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer){
  INTN i,j;
  if((i=(INTN)(y+Height-1))>(INTN)(Height -1))
    i = (INTN)(Height - 1);
  for (; i > (INTN)y; i--)    //i��ʾ�ڼ���
		for (j = (INTN)y; j <= i;j++) //j�ӵ�һ�е���i��
			putPartRectImage(x, j, Width, Height, 0, i - y, Width, 1, ImagePointer);//ѡ��ͼƬ��(0,i-j)���ڵ���  ��(x,j+i-y)������
  
}
//Description:ͼ���������Ч����ʾ�����ϵ���
VOID RainAscendShow(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer){
  INTN i, j;

	for (i = (INTN)y; i < (INTN)(y + Height);i++)
		for (j = (INTN)(y + Height - 1); j >= i;j--)
			putPartRectImage(x, j, Width, Height, 0, i - y, Width, 1, ImagePointer);
}

//Function Name: hWindowShadesShow
//Input: x,y,Width,Height,ImagePointer
//Output:
//Description:������ʾ��Ҷ��Ч��
VOID hWindowShadesShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i, j;

	for (j = 0; j < Width / 10;j++) //Width��һ���ܱ�10����������Ļ��޷���ʾ������Ӧ�����ж�һ�£���������+1���ο�vWindowShadesShow
		for (i = 0; i < 10;i++)
		{
			//putPartRectImage(i * Width / 10 + j, 0, Width, Height, i * Width / 10 + j, 0, 1, Height, ImagePointer);
			putPartRectImage(0, 0, Width, Height, i * Width / 10 + j, 0, 1, Height, ImagePointer);
			Delayms(2);
		}
}

//Function Name: vWindowShadesShow
//Input: x,y,Width,Height,ImagePointer
//Output:
//Description:������ʾ��Ҷ��Ч��
VOID vWindowShadesShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i, j;

	for (j = 0; j < Height / 10+1;j++)
		for (i = 0; i < 10;i++)
		{
			//putPartRectImage(i * Width / 10 + j, 0, Width, Height, i * Width / 10 + j, 0, 1, Height, ImagePointer);
			putPartRectImage(0, 0, Width, Height, 0, i*Height/10+j, Width, 1, ImagePointer);
			Delayms(2);
		}
}
//Function Name: CornerToCenter
//Input: x,y,Width,Height,ImagePointer
//Output:
//Description: ���Ľ����м���ʾͼ��
VOID CornerToCenter(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i,j;

	if(Width > Height)
	{
		for(i=0;i<=Height/2;i++)
		{
			putPartRectImage(x,y,Width,Height,0,0,i*Width/Height,i,ImagePointer);
			putPartRectImage(x,y,Width,Height,Width-1-i*Width/Height,0,i*Width/Height,i,ImagePointer);
			putPartRectImage(x,y,Width,Height,0,Height-1-i,i*Width/Height,i,ImagePointer);
			putPartRectImage(x,y,Width,Height,Width-1-i*Width/Height,Height-1-i,i*Width/Height,i,ImagePointer);
			Delayms(2);
		}
	}
	else
	{
		for(j=0;j<=Width/2;i++)
		{
			putPartRectImage(x,y,Width,Height,0,0,j,j*Height/Width,ImagePointer);
			putPartRectImage(x,y,Width,Height,0,Height-1-j*Height/Width,j,j*Height/Width,ImagePointer);
			putPartRectImage(x,y,Width,Height,Width-1-j,0,j,j*Height/Width,ImagePointer);
			putPartRectImage(x,y,Width,Height,Width-1-j,Height-1-i*Height/Width,j,j*Height/Width,ImagePointer);
			Delayms(2);
		}
	}		
}

//Description: ���м����Ľ���ʾͼ�� ��δ��ɣ��е����ˣ���ʱ��д���������
VOID CenterToCorner(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	INTN xx, yy, i;

	for (xx = (INTN)(Width / 2), yy = (INTN)(Height / 2), i = 1; i <= (INTN)(Height / 2);xx--,yy--,i++)
	{
		putPartRectImage(x, y, Width, Height, xx, yy, (Width-Height) + 2 * i, 2 * i, ImagePointer);
		Delayms(2);
	}
		
}

//Description:ˮƽ������ʾ
VOID hInterlaceShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i,j;

	for (i = 1; i < Width - 1; i++)
		for (j = 0; j < Height - 1; j += 2)
		{
			putPartRectImage(x, y, Width, Height, Width - 1 - i, j, i, 1, ImagePointer);
			putPartRectImage(x, y, Width, Height, 0, j + 1, i, 1, ImagePointer);	
		}
}


//Description: ��ֱ������ʾ
VOID vInterlaceShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i,j;

	for (j = 1; j < Height - 1;j++)
		for (i = 0; i < Width - 1;i+=8)
		{
			putPartRectImage(x, y, Width, Height, i,Height - 1 - j, 4, j, ImagePointer);
			putPartRectImage(x, y, Width, Height, i + 4, 0, 4, j, ImagePointer);
		}			
}

//Description: 
VOID hInteractiveShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i, j;

	//ˮƽ������ʽ��ʾ,��lines��Ϊ��λ
	for (i = 0; i < Width - 1; i++)
	{
		for (j = 0; j < (Height - 1) / 3; j += 2)  //for (j = 0; j < Height - 1 / lines; j += 2)
			putPartRectImage(x, y, Width, Height, i, j * 3, 1, 3, ImagePointer); //putPartRectImage(x, y, Width, Height, i, j * lines, 1, lines, ImagePointer);
		
	}

	for (i = 0; i < Width - 1; i++)
	{
		for (j = 1; j < (Height - 1) / 3; j += 2) //for (j = 1; j < (Height - 1) / lines; j += 2)
			putPartRectImage(x, y, Width, Height, Width - 1 - i, j * 3, 1, 3, ImagePointer); //putpartImage(x, y, Width, Height, Width - 1 - i, j * lines, 1, lines, ImagePointer);	
	}
}


//Description:
VOID vInteractiveShow(IN UINTN x, IN UINTN y, IN UINTN Width,IN UINTN Height,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	UINTN i, j;

	//��ֱ������ʽ��ʾ,��lines��Ϊ��λ
	for (i = 0; i < Height - 1; i++)
	{
		for (j = 0; j < (Width - 1) / 3; j += 2)  //for (j = 0; j < Height - 1 / lines; j += 2)
			putPartRectImage(x, y, Width, Height, j * 3, i,  3, 1, ImagePointer); //putPartRectImage(x, y, Width, Height, j * lines, i,  lines, 1, ImagePointer); 
		
	}

	for (i = 0; i < Height - 1; i++)
	{
		for (j = 1; j < (Width - 1) / 3; j += 2) //for (j = 1; j < (Width - 1) / lines; j += 2)
			putPartRectImage(x, y, Width, Height, j * 3, Height - 1 - i, 3, 1, ImagePointer); //putPartRectImage(x, y, Width, Height, j * lines, Height - 1 - i, lines, 1, ImagePointer); 	
	}
}

//Description: ������ʾ
VOID SpiralShow(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, UINTN maxX, UINTN maxY, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImagePointer)
{
	INTN i;

	for (i = (INTN)((Width - 1) >> 1); i > 0; i--)
		putImageLine(x, y, Width, Height, i, 0, (Width - 1) >> 1, (Height - 1) >> 1, maxX, maxY, ImagePointer);
	for (i = 0; i < (INTN)(Height - 1); i++)
		putImageLine(x, y, Width, Height, 0, i, (Width - 1) >> 1, (Height - 1) >> 1, maxX, maxY, ImagePointer);
	for (i = 0; i < (INTN)(Width - 1); i++)
		putImageLine(x, y, Width, Height, i, Height - 1 - 1, (Width - 1) >> 1, (Height - 1) >> 1, maxX, maxY, ImagePointer);
	for (i = (INTN)(Height - 1 - 1); i > 0; i--)
		putImageLine(x, y, Width, Height, Width - 1 - 1, i, (Width - 1) >> 1, (Height - 1) >> 1, maxX, maxY, ImagePointer);
	for (i = (INTN)(Width - 1 - 1); i > (INTN)((Width - 1) >> 1); i--)
		putImageLine(x, y, Width, Height, i, 0, (Width - 1) >> 1, (Height - 1) >> 1, maxX, maxY, ImagePointer);
}

//Description: ������ʾ���
VOID SpiralShowClear(IN UINTN x, IN UINTN y, IN UINTN Width, IN UINTN Height, IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
	INTN i;

	for (i = (INTN)((Width - 1) >> 1); i < (INTN)(Width - 1); i++)
		Line(i+x, 0+y, ((Width - 1) >> 1) +x, ((Height - 1) >> 1)+y, color);
	for (i = 0; i < (INTN)(Height - 1); i++)
		Line(Width - 1+x, i+y, ((Width - 1) >> 1)+x, ((Height - 1) >> 1)+y, color);
	for (i = (INTN)(Width - 1); i > 0; i--)
		Line(i+x, Height - 1+y, ((Width - 1) >> 1)+x, ((Height - 1) >> 1)+y, color);
	for (i = (INTN)(Height - 1); i > 0; i--)
		Line(0+x, i+y, ((Width - 1) >> 1) +x, ((Height - 1) >> 1)+y,  color);
	for (i = 0; i < (INTN)((Width - 1) >> 1); i++)
		Line(i+x, 0+y, ((Width - 1) >> 1)+x, ((Height - 1) >> 1)+y, color);
}