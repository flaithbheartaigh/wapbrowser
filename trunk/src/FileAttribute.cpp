/*
============================================================================
 Name        : FileAttribute.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CFileAttribute implementation
============================================================================
*/
#include <coemain.h>

#include "FileAttribute.h"
#include "MainEngine.h"
#include "ScreenLayout.h"
#include "Graphic.h"
#include "TypeDefine.h"
#include "cocobmp.h"

CFileAttribute::CFileAttribute(CMainEngine& aMainEngine,const TDesC& aFileName) : CControl(EFileAttribute)
																				, iMainEngine(aMainEngine)
{
	SetCleanUpObserver(&aMainEngine);
	iFileName.Append(aFileName);
	iHasButton = ETrue;
}


CFileAttribute::~CFileAttribute()
{
	delete iBitmap;
}

CFileAttribute* CFileAttribute::NewLC(CMainEngine& aMainEngine,const TDesC& aFileName)
{
	CFileAttribute* self = new (ELeave)CFileAttribute(aMainEngine,aFileName);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CFileAttribute* CFileAttribute::NewL(CMainEngine& aMainEngine,const TDesC& aFileName)
{
	CFileAttribute* self=CFileAttribute::NewLC(aMainEngine,aFileName);
	CleanupStack::Pop(); // self;
	return self;
}

void CFileAttribute::ConstructL()
{
	InitBitmap();
	GetFileAttributes();
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CFileAttribute::Draw(CGraphic& aGraphic) const
{
	TInt fontHeight = iMainEngine.ScreenLayout().FontHeight();

	_LIT(KM,":");

	TSize screenSize = iMainEngine.ScreenLayout().ScreenSize();

	TPoint point;
	point.iX = (screenSize.iWidth - iBmpSize.iWidth)/2;
	point.iY = (screenSize.iHeight - iBmpSize.iHeight)/2;

	
	aGraphic.BitBlt(point,iBitmap);
	aGraphic.SetPenColor(KBlackRgb);

	TPoint tmpPoint = point;
	tmpPoint.iY += (fontHeight>>1);
	aGraphic.DrawText(tmpPoint,iMainEngine.GetDesById(ETurkeyTextRes_FileAttribute));


	TPoint titlePos;
	TPoint listPos;
	iMainEngine.ScreenLayout().GetSaveAsPos(titlePos,listPos);

	TInt lineHeight = fontHeight;

	TInt offset1;
	TInt offset2;
	iMainEngine.ScreenLayout().GetAttributeOffset(offset1,offset2);
	//TInt secondRowPos = iMainEngine.ScreenLayout().AttributeOffset();

	//TInt maxDrawLength = 
	//#define	KTEXTHEIGHT	(15)
	//file name
	//point = listPos;
	point += listPos;
	tmpPoint = point;
	tmpPoint.iX += offset1;
	aGraphic.DrawText(tmpPoint,iMainEngine.GetDesById(ETurkeyTextRes_FileName));

	TInt nPos;
	nPos=iAttribute.m_szName.Find(_L("."));

	TBuf<200> nFileName;
	TBuf<100> nFileType;

	nFileName.Zero();
	nFileType.Zero();
	if(nPos==KErrNotFound)
	{
		nFileName.Append(iAttribute.m_szName);
	}
	else
	{
		nFileName.Append(iAttribute.m_szName.Mid(0,nPos));
		nFileType.Append(iAttribute.m_szName.Mid(nPos+1));
	}

	//TFileName szName = iAttribute.m_szName.Left(iAttribute.m_szName.Length() - 4);
	TInt nWidth = aGraphic.GetFont()->MeasureText(nFileName);
	if (nWidth > iMaxLengthInPixel)
	{
		int i=1;
		for (; i<=nFileName.Length(); i++)
		{
			nWidth = aGraphic.GetFont()->MeasureText(nFileName.Left(i));
			if (nWidth > iMaxLengthInPixel)
				break;
		}
		nFileName.Delete(0, nFileName.Length());
		nFileName.Append(iAttribute.m_szName.Left(i-1));
	}
	tmpPoint.iX += offset2;
	aGraphic.DrawText(tmpPoint,nFileName);

	//file type
	point.iY += lineHeight;
	tmpPoint = point;
	tmpPoint.iX += offset1;
	aGraphic.DrawText(tmpPoint,iMainEngine.GetDesById(ETurkeyTextRes_FileType));
	tmpPoint.iX += offset2;
	aGraphic.DrawText(tmpPoint,nFileType);
	//date
	point.iY += lineHeight;
	tmpPoint = point;
	tmpPoint.iX += offset1;
	aGraphic.DrawText(tmpPoint,iMainEngine.GetDesById(ETurkeyTextRes_FileDate));
	tmpPoint.iX += offset2;
	aGraphic.DrawText(tmpPoint,iAttribute.m_szDate);
	//time
	point.iY += lineHeight;
	tmpPoint = point;
	tmpPoint.iX += offset1;
	aGraphic.DrawText(tmpPoint,iMainEngine.GetDesById(ETurkeyTextRes_FileTime));
	tmpPoint.iX += offset2;
	aGraphic.DrawText(tmpPoint,iAttribute.m_szTime);	
	//size
	point.iY += lineHeight;
	tmpPoint = point;
	tmpPoint.iX += offset1;
	aGraphic.DrawText(tmpPoint,iMainEngine.GetDesById(ETurkeyTextRes_FileSize));
	TFileName szFileSize;
	szFileSize.AppendNum(iAttribute.m_nSize);
	szFileSize.Append(_L("KB"));
	tmpPoint.iX += offset2;
	aGraphic.DrawText(tmpPoint,szFileSize);

}

TBool CFileAttribute::KeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	if(EKeyDevice0 == aKeyCode || EKeyDevice3 == aKeyCode)
	{
		CleanupSelf();
	}
	return keyResult;
}

TBool CFileAttribute::HandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CFileAttribute::SizeChanged(const TRect& aScreenRect)
{

}

const TDesC& CFileAttribute::LeftButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Ok);
}

const TDesC& CFileAttribute::RightButton() const
{
	return KNullDesC;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CFileAttribute::InitBitmap()
{
	ASSERT(NULL == iBitmap);
	//iBitmap = iMainEngine.GetBitmapById(EMbmCocobmpAttributes);
	iBitmap = iMainEngine.CreateBmpById(EMbmCocobmpAttributes);
	iBmpSize = iMainEngine.ScreenLayout().ScaleByDefaultRatio(iBitmap);

	TInt offset1 = 0;
	TInt offset2 = 0;
	iMainEngine.ScreenLayout().GetAttributeOffset(offset1,offset2);

	iMaxLengthInPixel = iBmpSize.iWidth - (offset1 + offset2);
}

TInt CFileAttribute::GetFileAttributes()
{

	RFs& fs = CCoeEnv::Static()->FsSession();
	TParse parse;
	parse.Set(iFileName,NULL,NULL);

	const TDesC& filePath = parse.DriveAndPath();
	const TDesC& nameAndExt = parse.NameAndExt();
	RFile	file;
	TFileName path;
	path.Append(filePath);
	path.Append(nameAndExt);
	if (KErrNone != file.Open(fs, path, EFileRead )) {
		return 1;
	}

	TInt nSize = 0;
	file.Size(nSize);
	nSize /= 1024;
	iAttribute.m_nSize = nSize;
	iAttribute.m_szName.Append(nameAndExt);
	iAttribute.m_szPath.Append(filePath);
	TTime oTime;
	file.Modified(oTime);		
	TDateTime dt=oTime.DateTime();
	iAttribute.m_szDate.AppendNum(dt.Year());
	iAttribute.m_szDate.Append(_L("/"));
	iAttribute.m_szDate.AppendNum(dt.Month());
	iAttribute.m_szDate.Append(_L("/"));
	iAttribute.m_szDate.AppendNum(dt.Day());

	if(dt.Hour()>0)
	{
		if(dt.Hour()<10)
		iAttribute.m_szTime.Append(_L("0"));
		iAttribute.m_szTime.AppendNum(dt.Hour());
		iAttribute.m_szTime.Append(_L(":"));
	}
	if(dt.Minute()>0)
	{
		if(dt.Minute()<10)
			iAttribute.m_szTime.Append(_L("0"));
		iAttribute.m_szTime.AppendNum(dt.Minute());
		iAttribute.m_szTime.Append(_L(":"));
	}
	if(dt.Second()<10)
		iAttribute.m_szTime.Append(_L("0"));
	iAttribute.m_szTime.AppendNum(dt.Second());

	file.Close();

	return 0;
}