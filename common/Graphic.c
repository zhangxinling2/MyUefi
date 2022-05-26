//code by luobing 
/**
 * ��UEFI���ʵ�����������
 * �����UEFI̽�������Բο����ߵĲ��ͺ�ר����
 * CSDN: https://blog.csdn.net/luobing4365
 * ֪��: https://www.zhihu.com/column/c_1233025362843209728
 * **/

#include "Graphic.h"

extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;

//===========================================Graphic function begin====================================
//Name: GetGraphicModeNumber
//Input:
//Output:
EFI_STATUS GetGraphicModeNumber(
	IN	UINT32 HorRes,	//ˮƽ
	IN 	UINT32 VerRes,	//��ֱ
	OUT UINT32 *ModeNum,
	OUT UINT32 *CurrentModeNum,
	OUT UINT32 *PixelsPerScanLine
)
{
	EFI_STATUS Status;
	UINT32 ModeIndex;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   *Info;
	UINTN                                  SizeOfInfo;

	*CurrentModeNum=gGraphicsOutput->Mode->Mode;	//store current mode 
	for(ModeIndex=0;ModeIndex<gGraphicsOutput->Mode->MaxMode;ModeIndex++)
	{
		Status=gGraphicsOutput->QueryMode(gGraphicsOutput,ModeIndex,&SizeOfInfo,&Info);
		if(!EFI_ERROR(Status))
		{
			if((Info->HorizontalResolution == HorRes) && (Info->VerticalResolution == VerRes))
			{
				*ModeNum=ModeIndex;
				return Status;
			}
		}
	}
	return EFI_NOT_FOUND;
}
//Name: SwitchGraphicsMode(BOOLEAN flag)
//Input:
//Output:
EFI_STATUS SwitchGraphicsMode(BOOLEAN flag)
{
	EFI_STATUS                         Status;
	if(flag)
	{
		Status=gST->ConOut->EnableCursor (gST->ConOut, FALSE);
	}
	else
	{
		Status=gST->ConOut->EnableCursor (gST->ConOut, TRUE);
	}
	return Status;
}
//Function Name:  SetMyMode
//Input: ModeNumber
//Output: Void
//Description: Set Specify mode
VOID SetMyMode(UINT32 ModeNumber)
{
   gGraphicsOutput->SetMode(gGraphicsOutput,ModeNumber);
   return;
}
/**
 * @brief Put pixel to display screen
 * @param (x,y):  position of pixel
 * @param *color  color(IN)
 * @return  void
 */
VOID putpixel(UINTN x,UINTN y,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  gGraphicsOutput->Blt (gGraphicsOutput,color,EfiBltVideoFill,0,0,x,y,1,1,0);
}
/**
 * @brief get pixel from display screen
 * @param (x,y):  position of pixel
 * @param *color  color(OUT)
 * @return  void
 */
VOID getpixel(UINTN x,UINTN y, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
	gGraphicsOutput->Blt (gGraphicsOutput,color,EfiBltVideoToBltBuffer,x,y,0,0,1,1,0);
}

VOID Line(UINTN x1, UINTN y1, UINTN x2, UINTN y2, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
	/*ʹ��Bresenham�㷨*/
	INTN d,dx,dy,dx2,dy2,dxy;
	INTN xinc,yinc;

	
	/* ����׼�� */
	dx = (INTN)((x2 > x1) ? (x2 - x1) : (x1 - x2));
	dx2 = dx << 1;
	dy = (INTN)((y2 > y1) ? (y2 - y1) : (y1 - y2));
	dy2 = dy << 1;
	xinc =(INTN) (x2 > x1) ? 1 : ((x2 == x1) ? 0 : (-1));
	yinc = (INTN)(y2 > y1) ? 1 : ((y2 == y1) ? 0 : (-1));
	putpixel(x1, y1, color);
	if(dx>=dy){
		d=dy2-dx;
		dxy=dy2-dx2;
		while(dx--){
			if(d<=0)d+=dy2;
			else{
				d+=dxy;
				y1+=yinc;
			}
			// if(((x1+xinc)<=maxX)&&(y1<=maxY))			/* �Գ�����Ļ��Χ�ĵ㲻����ʾ */
				putpixel(x1+=xinc,y1,color);
		}
	}
	else{
		d=dx2-dy;
		dxy=dx2-dy2;
		while(dy--){
			if(d<=0)d+=dx2;
			else{
				d+=dxy;
				x1+=xinc;
			}
			// if((x1<=maxX)&&((y1+yinc)<=maxY))			/* �Գ�����Ļ��Χ�ĵ㲻����ʾ */
				putpixel(x1,y1+=yinc,color);
		}
	}
}
//Function Name:  HLine
//Input: x1,x2,y,color (x1<x2)
//Output: Void
//Description: Draw horizontal line
VOID HLine(UINTN x1,UINTN x2,UINTN y,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  UINTN minx,maxx;
  minx=(x1<x2)?x1:x2;
  maxx=(x1>x2)?x1:x2;

  gGraphicsOutput->Blt (gGraphicsOutput,color,EfiBltVideoFill,0,0,minx,y,(maxx-minx+1),1,0);  
}
//Function Name:  VLine
//Input: x,y1,y2,color(y1<y2)
//Output: Void
//Description: Draw vertical line
VOID VLine(UINTN x,UINTN y1,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  UINTN miny,maxy;
  miny=(y1<y2)?y1:y2;
  maxy=(y1>y2)?y1:y2;
  
  gGraphicsOutput->Blt (gGraphicsOutput,color,EfiBltVideoFill,0,0,x,miny,1,(maxy-miny+1),0);  
}
//Function Name:  rectblock
//Input: x1,y1,x2,y2,color(y1<y2)
//Output: Void
//Description: Draw block rect
VOID  rectblock(UINTN x1,UINTN y1,UINTN x2,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  UINTN minx,miny,maxx,maxy;
  
  minx=(x1<x2)?x1:x2;
  miny=(y1<y2)?y1:y2;
  maxx=(x1>x2)?x1:x2;
  maxy=(y1>y2)?y1:y2;
  
  gGraphicsOutput->Blt (gGraphicsOutput,color,EfiBltVideoFill,0,0,minx,miny,(maxx-minx+1),(maxy-miny+1),0);   
}

//another function
//Function Name:  rectangle
//Input: x1,y1,x2,y2,color_number: Pointer to ColorTable(y1<y2)
//Output: Void
//Description: Draw Rectangle
VOID  rectangle(UINTN x1,UINTN y1,UINTN x2,UINTN y2,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
//   UINTN left,top,right,bottom,i;
// //  EFI_GRAPHICS_OUTPUT_BLT_PIXEL color;

//   /* Location */
//   left=(x1<x2)?x1:x2;
//   right=(x1>x2)?x1:x2;
//   top=(y1<y2)?y1:y2;
//   bottom=(y1>y2)?y1:y2;
//  right=(right<maxX)?right:maxX;
//  bottom=(bottom<maxY)?bottom:maxY;       /* Cut off the graphic which out of screen */
//  MemCpy(&color,&(gColorTable[color_number]),sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
    
//   for(i=left;i<right;i++)putpixel(i,top,color);
//   for(i=top;i<bottom;i++)putpixel(right,i,color);
//   for(i=right;i>left;i--)putpixel(i,bottom,color);
//   for(i=bottom;i>top;i--)putpixel(left,i,color);
	UINTN minx,miny,maxx,maxy;
  
	minx=(x1<x2)?x1:x2;
	miny=(y1<y2)?y1:y2;
	maxx=(x1>x2)?x1:x2;
	maxy=(y1>y2)?y1:y2;

	HLine(minx,maxx,miny,color);
	VLine(minx,miny,maxy,color);
	HLine(minx,maxx,maxy,color);
	VLine(maxx,miny,maxy,color);

}
//Function Name:  circle
//Input: centerx,centery,radius,color
//Output: Void
//Description: Draw Rectangle
VOID  circle(UINTN centerx,UINTN centery,UINTN radius,EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  INTN x,y,d;
 
  if(!radius)return;
  y=(INTN)(radius);
  d=3-((INTN)(radius)<<1);
  
  
  /*see foxdisk3.01 by luobing*/
  putpixel(centerx,centery+y,color);        /*0,-1*/
  putpixel(centerx,centery-y,color);        /*0,1*/
  putpixel(centerx+y,centery,color);
  putpixel(centerx-y,centery,color);
  
  for(x=1;x<y;x++)
  {
    putpixel(centerx+x,centery+y,color);
    putpixel(centerx+x,centery-y,color);
    putpixel(centerx-x,centery-y,color);
    putpixel(centerx-x,centery+y,color);
    putpixel(centerx+y,centery+x,color);
    putpixel(centerx+y,centery-x,color);
    putpixel(centerx-y,centery-x,color);
    putpixel(centerx-y,centery+x,color);
    if(d<0)d+=((x<<2)+6);           
    else d+=(((x-(y--))<<2)+10);        
  }
  /* x=y */
  putpixel(centerx+y,centery+y,color);
  putpixel(centerx+y,centery-y,color);
  putpixel(centerx-y,centery-y,color);
  putpixel(centerx-y,centery+y,color);
}   

VOID getRectImage(IN UINTN x1,IN UINTN y1,UINTN Width,UINTN Height ,IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer){
	gGraphicsOutput->Blt(gGraphicsOutput,ImageBuffer,EfiBltVideoToBltBuffer,x1,y1,0,0,Width,Height,0);
}

VOID putRectImage(IN UINTN x1,IN UINTN y1,UINTN Height,UINTN Width ,IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer){
	gGraphicsOutput->Blt(gGraphicsOutput,ImageBuffer,EfiBltBufferToVideo,x1,y1,0,0,Width,Height,0);
}
//Description: x1,y1����Ļ����ʾͼ�����Ͻǵ���ʼλ�ã� Width,Height: ͼ����Ĵ�С��
//             distanceX,distanceY:����ͼ�����Ͻǵ�λ�õ����ꣻpartWidth,partHeight:��Ҫ��ʾ����ͼ��Ĵ�С
//             ImageBuffer: ͼ�����ص�����
VOID putPartRectImage(IN UINTN x1,IN UINTN y1,IN UINTN Width,IN UINTN Height,IN UINTN distanceX,IN UINTN distanceY,IN UINTN partWidth,IN UINTN partHeight,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer){
	UINTN realPartWidth,realPartHeight;
	if(distanceX>Width-1)
		return;
	if(distanceY>Height-1)//����ͼ��Χ����ʾ
		return;
	if((distanceX+partWidth)>(Width-1))//������ʵ��ʾͼ���С
		realPartWidth = Width-1 - distanceX +1;
	else
		realPartWidth = partWidth;
	if((distanceY+partHeight)>(Height-1))
		realPartHeight = Height-1-distanceY+1;
	else
		realPartHeight=partHeight;
	//BltBuffer��ĳ���Ӿ��α�ʹ�ã�զDelta����ͼ�ο�����������ֽ���
	gGraphicsOutput->Blt(gGraphicsOutput,ImageBuffer,EfiBltBufferToVideo,distanceX,distanceY,x1+distanceX,y1+distanceY,realPartWidth,realPartHeight,Width* sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
}
//��ͼ�񻺳�����ָ��λ�õ���л�ͼ��imgX,imgY�������ͼ�����Ͻ�λ�õ�����
VOID putImagePixel(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,IN UINTN imgX,IN UINTN imgY,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer){
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL color;
	if(imgX>Width-1)
		return;
	if(imgY>Height-1)
		return;
	color = ImageBuffer[imgY * Width + imgX];
	putpixel(x + imgX, y + imgY, &color);
}

//Description:ͼ���ڻ��ߺ���
VOID putImageLine(IN UINTN x,IN UINTN y,IN UINTN Width,IN UINTN Height,
			IN UINTN imageX1,IN UINTN imageY1, IN UINTN imageX2,IN UINTN imageY2,
			UINTN maxX, UINTN maxY,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBuffer)
{
		/*ʹ��Bresenham�㷨*/
	INTN d,dx,dy,dx2,dy2,dxy;
	INTN xinc,yinc;
	INTN x1, x2, y1, y2;

	x1 = (INTN)(imageX1);
	y1 = (INTN)(imageY1);
	x2 = (INTN)(imageX2);
	y2 = (INTN)(imageY2);

	/* ����׼�� */
	dx = (INTN)((x2 > x1) ? (x2 - x1) : (x1 - x2));
	dx2 = dx << 1;
	dy = (INTN)((y2 > y1) ? (y2 - y1) : (y1 - y2));
	dy2 = dy << 1;
	xinc =(INTN) (x2 > x1) ? 1 : ((x2 == x1) ? 0 : (-1));
	yinc = (INTN)(y2 > y1) ? 1 : ((y2 == y1) ? 0 : (-1));
	putImagePixel(x, y, Width,Height,x1,y1,ImageBuffer);
	if(dx>=dy){
		d=dy2-dx;
		dxy=dy2-dx2;
		while(dx--){
			if(d<=0)d+=dy2;
			else{
				d+=dxy;
				y1+=yinc;
			}
			if(((x1+xinc)<=(INTN)(maxX))&&(y1<=(INTN)maxY))			/* �Գ�����Ļ��Χ�ĵ㲻����ʾ */
				putImagePixel(x, y, Width,Height,x1+=xinc,y1,ImageBuffer);
		}
	}
	else{
		d=dx2-dy;
		dxy=dx2-dy2;
		while(dy--){
			if(d<=0)d+=dx2;
			else{
				d+=dxy;
				x1+=xinc;
			}
			if((x1<=(INTN)maxX)&&((y1+yinc)<=(INTN)maxY))			/* �Գ�����Ļ��Χ�ĵ㲻����ʾ */
				putImagePixel(x, y, Width,Height,x1,y1+=yinc,ImageBuffer);
		}
	}
}
//Function Name: SetBKG
//Input: color
//Output: None
//Description: set background
//VOID SetBKG(EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
//{
//  rectblock(0,0,SY_SCREEN_WIDTH-1,SY_SCREEN_HEIGHT-1,color);
//}
//===========================================Graphic function end======================================

//================Debug function================
//Name: DisplayModeInformation
//Input:
//Output:
EFI_STATUS DisplayModeInformation(void)
{
	Print(L"Print Current Mode Message:\n");
	Print(L"::MaxMode=0x%02x\n",gGraphicsOutput->Mode->MaxMode);
	Print(L"::Current Mode=0x%02x\n",gGraphicsOutput->Mode->Mode);
	Print(L"::SizeOfInfo=0x%x\n",gGraphicsOutput->Mode->SizeOfInfo);
	Print(L"::FrameBufferBase=0x%lx\n",gGraphicsOutput->Mode->FrameBufferBase);
	Print(L"::FrameBufferSize=%x\n",gGraphicsOutput->Mode->FrameBufferSize);
	Print(L"::Info->Version=%x\n",gGraphicsOutput->Mode->Info->Version);
	Print(L"::Info->HorizontalResolution=%x\n",gGraphicsOutput->Mode->Info->HorizontalResolution);
	Print(L"::Info->VerticalResolution=%x\n",gGraphicsOutput->Mode->Info->VerticalResolution);
	Print(L"::Info->PixelsPerScanLine=%x\n",gGraphicsOutput->Mode->Info->PixelsPerScanLine);
	
	return EFI_SUCCESS;
}
//Name: DisplaySpecifyModeMessage(void)
//Input:
//Output:
EFI_STATUS DisplaySpecifyModeMessage(UINT32 ModeNumber)
{
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   *Info;
	UINTN                                  SizeOfInfo;
	EFI_STATUS                         Status;

	Status=gGraphicsOutput->QueryMode(gGraphicsOutput,ModeNumber,&SizeOfInfo,&Info);
	if(EFI_ERROR(Status))
	{
		Print(L"QueryMode(): %r\n",Status);
	}
	else
	{
		Print(L"Print Mode %d Message:\n",ModeNumber);
		Print(L"::Info->Version=%d\n",Info->Version);
		Print(L"::Info->HorizontalResolution=%d\n",Info->HorizontalResolution);
		Print(L"::Info->VerticalResolution=%d\n",Info->VerticalResolution);
		Print(L"::Info->PixelsPerScanLine=%d\n",Info->PixelsPerScanLine);
		Print(L"::Info->PixelFormat= %d\n",Info->PixelFormat);
		Print(L"::Info->PixelInformation:\n ");
		Print(L"0x%x  ",Info->PixelInformation.RedMask);
		Print(L"0x%x  ",Info->PixelInformation.GreenMask);
		Print(L"0x%x  ",Info->PixelInformation.BlueMask);
		Print(L"0x%x  ",Info->PixelInformation.ReservedMask);
		Print(L"\n ");
		gBS->FreePool(Info);
	}
		
	return Status;
}
//Name: DisplayTextModeInformation(void)
//Input:
//Output:
EFI_STATUS DisplayTextModeInformation(void)
{
	Print(L"Print SimPleTextMode Message:\n");
	Print(L"::MaxMode=%d\n",gST->ConOut->Mode->MaxMode);
	Print(L"::Current Mode=%d\n",gST->ConOut->Mode->Mode);
	Print(L"::Attribute=%d\n",gST->ConOut->Mode->Attribute);
	Print(L"::CursorColumn=%d\n",gST->ConOut->Mode->CursorColumn);
	Print(L"::CursorRow=%d\n",gST->ConOut->Mode->CursorRow);
	Print(L"::CursorVisible=%d\n",gST->ConOut->Mode->CursorVisible);
	
	return EFI_SUCCESS;	
}

//Name: DisplaySpecifyTextModeMessage(void)
//Input:
//Output:
EFI_STATUS DisplaySpecifyTextModeMessage(UINTN ModeNumber)
{
	EFI_STATUS                         Status;
	UINTN Columns;
	UINTN Rows;
	
	Status=gST->ConOut->QueryMode(gST->ConOut,ModeNumber,&Columns,&Rows);
	Print(L"Print Text Mode %d Message:\n",ModeNumber);
	Print(L"::Column=%d\n",Columns);
	Print(L"::Row=%d\n",Rows);
	
	return Status;
}

//����ֱ���
VOID showInfo(UINT32 MOdeIndex){
	//��ʾģʽ��Ϣ�ṹ��
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info=NULL;
	UINTN infoSize;//ģʽ��Ϣ��С
	//ʹ��QueryMode��ѯ
	gGraphicsOutput->QueryMode(gGraphicsOutput,MOdeIndex,&infoSize,&info);
	Print(L"Horizontal:%x\n",info->HorizontalResolution);
	Print(L"Vertical:%x\n",info->VerticalResolution);
}
