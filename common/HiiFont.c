#include "common.h"
#include "HiiFont.h"

extern EFI_GRAPHICS_OUTPUT_PROTOCOL *gGraphicsOutput;
extern EFI_HII_FONT_PROTOCOL *gHiiFont;
//SimpleFont
extern EFI_WIDE_GLYPH gSimpleFontWideGlyphData[];
extern UINT32 gSimpleFontWideBytes;
extern EFI_NARROW_GLYPH gSimpleFontNarrowGlyphData[];
extern UINT32 gSimpleFontNarrowBytes;
static EFI_GUID gSimpleFontPackageListGuid = {0xf6643673, 0x6006, 0x3c38, {0x5c, 0xcd, 0xda, 0x1a, 0xeb, 0x3b, 0x26, 0xa2}};
extern EFI_NARROW_GLYPH gRussianNarrowGlyphData[];
extern UINT32 nRussianNarrowGlyphSize;
//Font
extern EFI_WIDE_GLYPH gDMFontWideGlyphData[];
extern UINT32 gDMFontBytes;
extern EFI_NARROW_GLYPH gMyNarrowGlyphData[];
extern UINT32 nMyNarrowGlyphSize;
extern EFI_WIDE_GLYPH gMyWideGlyphData[];
extern UINT32 nMyWideGlyphSize;
static EFI_GUID gDMFontPackageListGuid = {0xf6645673, 0x6106, 0x3138, {0x5c, 0xcd, 0xda, 0x1a, 0xab, 0x3b, 0x26, 0xa2}};
#define WfontWidth 16
#define WfontHight 19
CONST UINT16 WideGlyphBytes = WfontHight * WfontWidth / 8;
CONST UINT16 NrGlyphBytes = 19;

//============================== Hii Function begin==============================================
//ugly codes...
//必须先注册汉字simplefont库，才能显示汉字
EFI_STATUS TestStrPackage(void)
{
    EFI_STATUS Status = 0;
    EFI_GUID mStrPackageGuid = {0xedd31def, 0xf262, 0xc24e, 0xa2, 0xe4, 0xde, 0xf7, 0xde, 0xcd, 0xcd, 0xee};
    EFI_HANDLE HiiHandle = HiiAddPackages(&mStrPackageGuid, gImageHandle, commonStrings, NULL);
    Status = TestLanguage(HiiHandle);
    Status = TestString(HiiHandle);
    return Status;
}
//ugly codes...
EFI_STATUS TestLanguage(EFI_HANDLE HiiHandle)
{
    EFI_STATUS Status = 0;
    CHAR8 buf[256];
    CHAR16 buf16[256];
    UINTN LangSize = 256;
    UINTN i = 0;

    Status = gHiiString->GetLanguages(
        gHiiString,
        HiiHandle,
        buf,
        &LangSize);
    for (i = 0; i < LangSize; i++)
    {
        buf16[i] = buf[i];
    }
    buf16[LangSize] = 0;
    Print(L"Support Language: %s\n", buf16);
    return Status;
}
//ugly codes...
EFI_STATUS TestString(EFI_HANDLE HiiHandle)
{
    EFI_STATUS Status = 0;
    //CHAR8* BestLanguage = "en-US";
    //CHAR8 *BestLanguage = "zh-Hans";
    CHAR8 *BestLanguage = "ru-RU";
    EFI_STRING TempString = NULL;
    UINTN StringSize = 0;
    Status = gHiiString->GetString(
        gHiiString,
        BestLanguage,
        HiiHandle,
        STRING_TOKEN(STR_HELLOWORLD),
        TempString,
        &StringSize,
        NULL);
    //gBS->AllocatePool(EfiBootServicesData,0x100,(void**)&buffer);
    if (Status == EFI_BUFFER_TOO_SMALL)
    {
        Status = gBS->AllocatePool(EfiBootServicesData, StringSize, (void **)&TempString);
        if (EFI_ERROR(Status))
            return EFI_BUFFER_TOO_SMALL;
        Status = gHiiString->GetString(
            gHiiString,
            BestLanguage,
            HiiHandle,
            STRING_TOKEN(STR_HELLOWORLD),
            TempString,
            &StringSize,
            NULL);
        Print(L"%s\n", TempString);
        gBS->FreePool(TempString);
    }
    else
    {
        Print(L"GetString %r\n", Status);
    }
    return 0;
}
//----------------------------------- Hii SimpleFont -----------------------
/***
  构建SimpleFont资源包，并向Hii数据库注册

  @param[in]  NarrowGlyph   窄字符字模信息
  @param[in]  nNarrow       窄字符数组字节长
  @param[in]  WideGlyph     宽字符字模信息 
  @param[in]  nWide         宽字符数组字节长
  @retval  EFI_SUCCESS         执行成功
  @retval  Other               发生错误
***/
EFI_STATUS CreateSimpleFontPkg(EFI_NARROW_GLYPH *NarrowGlyph, UINT32 nNarrow, EFI_WIDE_GLYPH *WideGlyph, UINT32 nWide)
{
    // EFI_STATUS Status;
    EFI_HII_SIMPLE_FONT_PACKAGE_HDR *simpleFont;
    UINT8 *Package;
    UINT8 *Location = NULL;
    UINT32 packageLen = sizeof(EFI_HII_SIMPLE_FONT_PACKAGE_HDR) + nNarrow + nWide + 4;
    Package = (UINT8 *)AllocateZeroPool(packageLen);
    WriteUnaligned32((UINT32 *)Package, packageLen);
    simpleFont = (EFI_HII_SIMPLE_FONT_PACKAGE_HDR *)(Package + 4);
    simpleFont->Header.Length = (UINT32)(packageLen - 4);
    simpleFont->Header.Type = EFI_HII_PACKAGE_SIMPLE_FONTS;
    simpleFont->NumberOfNarrowGlyphs = (UINT16)(nNarrow / sizeof(EFI_NARROW_GLYPH));
    simpleFont->NumberOfWideGlyphs = (UINT16)(nWide / sizeof(EFI_WIDE_GLYPH));
    Location = (UINT8 *)(&simpleFont->NumberOfWideGlyphs + 1); //放到此处赋值，需要对齐
    CopyMem(Location, NarrowGlyph, nNarrow);
    Location += nNarrow;
    CopyMem(Location, WideGlyph, nWide);
    //
    // Add this simplified font package to a package list then install it.
    //
    EFI_HII_HANDLE simpleFontHiiHandle = HiiAddPackages(
        &gSimpleFontPackageListGuid,
        NULL,
        Package,
        NULL);
    if (simpleFontHiiHandle == NULL)
    {
        return EFI_NOT_FOUND; //查看了EfiError.h,选择这个返回值
    }
    FreePool(Package);
    return EFI_SUCCESS;
}
/***
  注册SimpleFont点阵库

  @param[in/out]  void
  @retval  EFI_SUCCESS         执行成功
  @retval  Other               发生错误
***/
EFI_STATUS LoadSimpleFont(void)
{
    EFI_STATUS Status = 0;
    EFI_HII_HANDLE *handles = 0; //for simpleFont of Hii
    handles = HiiGetHiiHandles(&gSimpleFontPackageListGuid);
    if (handles == 0)
    {
        //Status = CreateSimpleFontPkg(gSimpleFontNarrowGlyphData, gSimpleFontNarrowBytes, gSimpleFontWideGlyphData, gSimpleFontWideBytes);
        Status = CreateSimpleFontPkg(gRussianNarrowGlyphData, nRussianNarrowGlyphSize, gSimpleFontWideGlyphData, gSimpleFontWideBytes);
        Print(L"LoadSimpleFont:CreateSimpleFontPkg= %r\n", Status);
        //gST->ConOut->OutputString(gST->ConOut,L"execute CreatesimpleFontPkg()  handles==0\n\r");
        //return Status;
    }
    else
    {
        //gST->ConOut->OutputString(gST->ConOut,L"execute CreatesimpleFontPkg()  handles==1\n\r");
        FreePool(handles);
        return EFI_LOAD_ERROR; //refer to EfiError.h
    }

    return Status;
}



//----------------------------------- Hii Font -----------------------
/***
  构建Font资源包，并向Hii数据库注册
  @param[in]  FontName      字体名字
  @param[in]  FontWidth     字体宽度
  @param[in]  FontHeight    字体高度
  @param[in]  NarrowGlyph   窄字符字模信息
  @param[in]  NrSizeInBytes 窄字符数组字节长
  @param[in]  NrStart       窄字符起始字符
  @param[in]  NrCharNum     窄字符数目
  @param[in]  WideGlyph     宽字符字模信息 
  @param[in]  SizeInBytes   宽字符数组字节长
  @param[in]  Start         宽字符起始字符
  @param[in]  CharNum       宽字符数目
  @retval  EFI_SUCCESS         执行成功
  @retval  Other               发生错误
  注意： Font资源包中，不存在窄字符和宽字符的概念，本函数主要是借用了SimpleFont的字模信息，
        构建了Font资源包而已。
***/
EFI_STATUS CreateMyFontPkg(CHAR16 *FontName, UINT16 FontWidth, UINT16 FontHeight,
                           EFI_NARROW_GLYPH *NarrowGlyph, UINT32 NrSizeInBytes, CHAR16 NrStart, CHAR16 NrCharNum,
                           EFI_WIDE_GLYPH *WideGlyph, UINT32 SizeInBytes, CHAR16 Start, CHAR16 CharNum)
{
    EFI_HII_FONT_PACKAGE_HDR *FontPkgHeader; //Font 专用的数据结构
    UINT32 PackageLength;                    //包长度
    UINT8 *Package;                          //Font包
    UINTN BlockLength = 0;                   //块大小
    UINT8 *pCurrent = 0;                     //当前指针
    CHAR16 NextChar = 0;                     //下一个字符
    EFI_GLYPH_GIBT_END_BLOCK *FontEndBlock; //Font包终止块
    EFI_HII_GLYPH_INFO Cell;                  //Font点阵信息
    //    EFI_HII_GLYPH_INFO Cell = {FontWidth, FontHeight, 10, 10, (INT16)FontWidth};  //2019-6-7 12:02:37 luobing: build error

    UINT16 FontNameLen = (UINT16)StrLen(FontName) * 2 + 2;  //包名称大小
    //设置点阵信息
    Cell.Width = FontWidth;
    Cell.Height = FontHeight;
    Cell.OffsetX = 0;
    Cell.OffsetY = 0;
    Cell.AdvanceX = (INT16)FontWidth;
    //包长度等于Font包头结构大小加上包名大小加上各块结构大小加上字符数据总大小
    PackageLength = 4 + sizeof(EFI_HII_FONT_PACKAGE_HDR) + (FontNameLen /*Max Length of Font Name*/) +
                    sizeof(EFI_HII_GIBT_SKIP2_BLOCK) +
                    sizeof(EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK) - 1 + CharNum * WideGlyphBytes +
                    sizeof(EFI_HII_GIBT_SKIP2_BLOCK) +
                    sizeof(EFI_HII_GIBT_GLYPHS_BLOCK) - 1 + NrCharNum * NrGlyphBytes +
                    sizeof(EFI_GLYPH_GIBT_END_BLOCK);
    //根据包长度为包分配相应的空间
    Package = (UINT8 *)AllocateZeroPool(PackageLength);
    //ASSERT (Package != NULL);

    // 设置Font包头
    WriteUnaligned32((UINT32 *)Package, PackageLength);                     //将可能未对齐的32位值写入内存。该函数保证写操作不会产生对齐错误。
    FontPkgHeader = (EFI_HII_FONT_PACKAGE_HDR *)(Package + 4);              //将空间分配给包头，除了为包本身分配空间之外，我们还需要为包大小添加4个字节，以附加所有数据。
    FontPkgHeader->Header.Length = (UINT32)(PackageLength - 4);             //包长度
    FontPkgHeader->Header.Type = EFI_HII_PACKAGE_FONTS;                     //设置类型
    FontPkgHeader->HdrSize = sizeof(EFI_HII_FONT_PACKAGE_HDR) + FontNameLen;    //Font包头加字体名称的大小作为包头大小
    FontPkgHeader->GlyphBlockOffset = sizeof(EFI_HII_FONT_PACKAGE_HDR) + FontNameLen;   //偏移量，偏移量后指向第一个UNICODE字符
    FontPkgHeader->Cell = Cell;
    FontPkgHeader->FontStyle = EFI_HII_FONT_STYLE_NORMAL;                   //字体风格
    CopyMem((FontPkgHeader->FontFamily), FontName, FontNameLen);            //字体名称

    pCurrent = (UINT8 *)(Package + 4 + FontPkgHeader->GlyphBlockOffset);    //令指针指向字符

    //CHAR 0...255
    BlockLength = FillNarrowGLYPH(pCurrent, NarrowGlyph, NrSizeInBytes, 1, NrStart, NrCharNum);         //填充窄字符并返回块长度
    pCurrent += BlockLength;                    //移动指针
    NextChar = NrStart + NrCharNum;             //下一个字符起始
    // EFI_HII_GIBT_GLYPHS_DEFAULT
    BlockLength = FillWideGLYPH(pCurrent, WideGlyph, SizeInBytes, NextChar, Start, CharNum);            //填充宽字符
    pCurrent += BlockLength;
    // END BLOCK
    FontEndBlock = (EFI_GLYPH_GIBT_END_BLOCK *)(pCurrent);

    FontEndBlock->Header.BlockType = (UINT8)EFI_HII_GIBT_END;

    //
    // Add this simplified font package to a package list then install it.
    //
    {
        EFI_HANDLE gFontHiiHandle = HiiAddPackages(
            &gDMFontPackageListGuid,
            NULL,
            Package,
            NULL);
        (void)gFontHiiHandle;
        //ASSERT (gFontHiiHandle != NULL);
    }
    FreePool(Package);
    return EFI_SUCCESS;
}
/***
  注册Font点阵库

  @param[in/out]  void
  @retval  EFI_SUCCESS         执行成功
  @retval  Other               发生错误
***/
EFI_STATUS LoadFont(void)
{
    EFI_STATUS Status = 0;
    EFI_HII_HANDLE *handles1 = 0; //for Font of Hii
    handles1 = HiiGetHiiHandles(&gDMFontPackageListGuid);
    if (handles1 == 0)
    {
        //extern CHAR16* FontName ;
        CHAR16 *FontName = (CHAR16 *)L"LUOBING"; //随便取个名字
        Status = CreateMyFontPkg(FontName, 16, 19,
                                 gMyNarrowGlyphData, nMyNarrowGlyphSize, 32, (CHAR16)(nMyNarrowGlyphSize / sizeof(EFI_NARROW_GLYPH)),
                                 gMyWideGlyphData, nMyWideGlyphSize, 0x4e00, (CHAR16)(nMyWideGlyphSize / sizeof(EFI_WIDE_GLYPH)));
        Print(L"LoadFont:CreateMyFontPkg= %r\n", Status);
    }
    else
    {
        //gST->ConOut->OutputString(gST->ConOut,L"execute CreateMyFontPkg()  handles1==1\n\r");
        FreePool(handles1);
        return EFI_LOAD_ERROR; //refer to EfiError.h
    }

    return Status;
}

//字符点阵数据较小的字符
EFI_STATUS FillNarrowGLYPH(UINT8 *p, EFI_NARROW_GLYPH *NarrowGlyph, UINT32 SizeInBytes, CHAR16 Next, CHAR16 NrStart, CHAR16 NrCharNum)
{
    UINT8 *BitmapData = NULL;
    UINTN Length = 0;
    EFI_HII_GLYPH_INFO Cell = {8, 19, 0, 0, (INT16)8};   //Font结构体
    EFI_HII_GIBT_GLYPHS_BLOCK *GlyphsBlock; //2019-6-7 11:59:05 luobing ammend  点阵块

    // SKIP  
    if (Next != NrStart)
    {
        EFI_HII_GIBT_SKIP2_BLOCK *FontSkip2Block = (EFI_HII_GIBT_SKIP2_BLOCK *)p;                   //设置EFI_HII_GIBT_SKIP2_BLOCK 因为是连续存储所以直接将p强转为块
        FontSkip2Block->Header.BlockType = (UINT8)EFI_HII_SIBT_SKIP2;
        FontSkip2Block->SkipCount = NrStart - Next;

        p = (UINT8 *)(FontSkip2Block + 1);                          //p继续指向块后
        Length += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);                 //长度加上块长
    }

    //设置多字符的点阵块
    //    EFI_HII_GIBT_GLYPHS_BLOCK * GlyphsBlock = (EFI_HII_GIBT_GLYPHS_BLOCK *)(p);
    GlyphsBlock = (EFI_HII_GIBT_GLYPHS_BLOCK *)(p);
    GlyphsBlock->Header.BlockType = (UINT8)EFI_HII_GIBT_GLYPHS;
    GlyphsBlock->Cell = Cell;
    GlyphsBlock->Count = NrCharNum;
    BitmapData = GlyphsBlock->BitmapData;
    {                                           //填充数据
        UINT16 i = 0, j = 0;
        for (i = 0; i < NrCharNum; i++)         //字符个数
        {
            for (j = 0; j < 19; j++)            //字符位图 每个字符GlyphCol1数目与字符高度相同 所以需填充19个 因为是8*19/8所以与19相同
            {
                BitmapData[j] = NarrowGlyph[i].GlyphCol1[j];
            }
            BitmapData += 19;                   //下一个字符
        }
    }
    Length += (sizeof(EFI_HII_GIBT_GLYPHS_BLOCK) + 19 * NrCharNum - 1);     //长度加上块长与字符总长
    return (EFI_STATUS)Length;
}
//大多与窄字符相似
EFI_STATUS FillWideGLYPH(UINT8 *p, EFI_WIDE_GLYPH *WideGlyph, UINT32 SizeInBytes, CHAR16 Next, CHAR16 Start, CHAR16 CharNum)
{
    UINT8 *BitmapData = NULL;
    UINTN Length = 0;
    EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK *GlyphsDefaultBlock;

    // SKIP
    if (Next != Start)
    {
        EFI_HII_GIBT_SKIP2_BLOCK *FontSkip2Block = (EFI_HII_GIBT_SKIP2_BLOCK *)p;
        FontSkip2Block->Header.BlockType = (UINT8)EFI_HII_SIBT_SKIP2;
        FontSkip2Block->SkipCount = Start - Next;

        p = (UINT8 *)(FontSkip2Block + 1);
        Length += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);
    }

    //设置默认字体的多字符点阵块
    //    EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK* GlyphsDefaultBlock = (EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK*)(p);
    GlyphsDefaultBlock = (EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK *)(p);
    GlyphsDefaultBlock->Header.BlockType = (UINT8)EFI_HII_GIBT_GLYPHS_DEFAULT;
    GlyphsDefaultBlock->Count = CharNum;
    BitmapData = GlyphsDefaultBlock->BitmapData;
    {
        UINT16 i = 0, j = 0;
        for (i = 0; i < CharNum; i++)
        {
            for (j = 0; j < WideGlyphBytes; j++)            //WideGlyphBytes=16*19/8
            {
                BitmapData[j] = WideGlyph[i].GlyphCol1[(j % 2) * 19 + j / 2];
            }
            BitmapData += WideGlyphBytes;
        }
    }
    Length += (sizeof(EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK) + WideGlyphBytes * CharNum - 1);
    return (EFI_STATUS)Length;
}

// EFI_STATUS FillGLYPH(UINT8 *p,
//                      UINT8 *Glyph, UINT32 SizeInBytes, CHAR16 Next, CHAR16 Start, CHAR16 CharNum)
// {
//     UINT8 *BitmapData = NULL;
//     UINTN Length = 0;
//     EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK *GlyphsDefaultBlock; //2019-6-7 11:59:45 luobing ammend
//     // SKIP
//     if (Next != Start)
//     {
//         EFI_HII_GIBT_SKIP2_BLOCK *FontSkip2Block = (EFI_HII_GIBT_SKIP2_BLOCK *)p;
//         FontSkip2Block->Header.BlockType = (UINT8)EFI_HII_SIBT_SKIP2;
//         FontSkip2Block->SkipCount = Start - Next;

//         p = (UINT8 *)(FontSkip2Block + 1);
//         Length += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);
//     }

//     //
//     //    EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK* GlyphsDefaultBlock = (EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK*)(p);
//     GlyphsDefaultBlock = (EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK *)(p);
//     GlyphsDefaultBlock->Header.BlockType = (UINT8)EFI_HII_GIBT_GLYPHS_DEFAULT;
//     GlyphsDefaultBlock->Count = CharNum;
//     BitmapData = GlyphsDefaultBlock->BitmapData;
//     CopyMem(BitmapData, Glyph, SizeInBytes);
//     Length += (sizeof(EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK) + SizeInBytes - 1);
//     return (EFI_STATUS)Length;
// }


//----------------------------------- 字符串显示 -----------------------
/***
  图形模式下显示字符串
  @param[in]  x         显示位置X坐标
  @param[in]  y         显示位置Y坐标
  @param[in]  String    字符串
  @param[in]  FGColor   显示字符的前景色
  @param[in]  BGColor   显示字符的背景色
  @retval  EFI_SUCCESS         执行成功
  @retval  Other               发生错误
***/

EFI_STATUS putHiiFontStr(UINTN x, UINTN y, EFI_STRING String,       //位置和字符串
                                CHAR16 *fontName,                   //字体名 
                                EFI_GRAPHICS_OUTPUT_BLT_PIXEL *FGColor, //前景色
                                EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BGColor  //背景色
                                )
{
    //gHiiFont 和 gGraphicsOutput 已经加载
    EFI_STATUS Status = 0;
    EFI_IMAGE_OUTPUT gSystemFrameBuffer;
    EFI_IMAGE_OUTPUT *pSystemFrameBuffer = &gSystemFrameBuffer;
    EFI_FONT_DISPLAY_INFO *fontDisplayInfo;

    if (fontName == NULL) //选择使用SimpleFont点阵字库
    {
        // fontDisplayInfo = NULL;
        fontDisplayInfo = (EFI_FONT_DISPLAY_INFO *)AllocateZeroPool(sizeof(EFI_FONT_DISPLAY_INFO) + 2);
        fontDisplayInfo->ForegroundColor = *FGColor;
        fontDisplayInfo->BackgroundColor = *BGColor;
        fontDisplayInfo->FontInfoMask = (EFI_FONT_INFO_ANY_FONT | EFI_FONT_INFO_ANY_SIZE);
        fontDisplayInfo->FontInfo.FontStyle = EFI_HII_FONT_STYLE_ITALIC; //特意设定Font资源包注册时没有使用的，显示时使用SimpleFont
        fontDisplayInfo->FontInfo.FontSize = 19;
        *(fontDisplayInfo->FontInfo.FontName) = 0x00;
    }
    else
    {                               //使用Font点阵字库
            fontDisplayInfo = (EFI_FONT_DISPLAY_INFO *)AllocateZeroPool(sizeof(EFI_FONT_DISPLAY_INFO) + StrLen(fontName) * 2 + 2);
        fontDisplayInfo->ForegroundColor = *FGColor;
        fontDisplayInfo->BackgroundColor = *BGColor;
        fontDisplayInfo->FontInfoMask = (EFI_FONT_INFO_ANY_FONT | EFI_FONT_INFO_ANY_SIZE);
        fontDisplayInfo->FontInfo.FontStyle = EFI_HII_FONT_STYLE_NORMAL;
        fontDisplayInfo->FontInfo.FontSize = 19;
        CopyMem(fontDisplayInfo->FontInfo.FontName, fontName, StrLen(fontName) * 2 + 2);
    }

    gSystemFrameBuffer.Width = (UINT16)gGraphicsOutput->Mode->Info->HorizontalResolution;
    gSystemFrameBuffer.Height = (UINT16)gGraphicsOutput->Mode->Info->VerticalResolution;
    gSystemFrameBuffer.Image.Screen = gGraphicsOutput;

    Status = gHiiFont->StringToImage(
        gHiiFont,
        EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_CLIP |
            EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_OUT_FLAG_CLIP_CLEAN_Y |
            EFI_HII_IGNORE_LINE_BREAK | EFI_HII_DIRECT_TO_SCREEN,
        String,
        fontDisplayInfo,
        &pSystemFrameBuffer,
        (UINTN)x,
        (UINTN)y,
        0,
        0,
        0);
    FreePool(fontDisplayInfo);
    return Status;
}