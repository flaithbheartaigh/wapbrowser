/*
============================================================================
 Name        : MultiText.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CMultiText implementation
============================================================================
*/
#include <coemain.h>
#include <s32file.h>

#include "MultiText.h"
#include "ScrollBar.h"
#include "UtilityTools.h"
#include "Graphic.h"
#include "TypeDefine.h"
#include "MControlObserver.h"

CMultiText::CMultiText() : CControl(EMultiText)
{
	iScrollWidth = 3;
	iScrollHeight = 20;
	iTextColor = KRgbWhite;
}


CMultiText::~CMultiText()
{
	delete iScrollbar;
	delete iDesArray;
}

CMultiText* CMultiText::NewLC()
{
	CMultiText* self = new (ELeave)CMultiText();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CMultiText* CMultiText::NewL()
{
	CMultiText* self=CMultiText::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CMultiText::ConstructL()
{
	iBool=EFalse;
	iLastIndex=0;
}
//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CMultiText::Draw(CGraphic& gc) const
{
	Assert();
	//gc.UseFont(iFont);
	gc.SetPenColor(iTextColor);
	gc.SetPenStyle(CBitmapContext::ESolidPen);
	gc.SetBrushStyle(CBitmapContext::ENullBrush);
	TPoint point = iStartPoint;
	//point.iY += iFont->HeightInPixels();
	for (TInt i = 0 ; i < iMaxLinePerPage ; i++)
	{
		TInt curIndex = iScrollbar->CurIndex() + i;
		if(curIndex >= iDesArray->Count())
		{
			break;
		}
		const TDesC& aDes = (*iDesArray)[curIndex];
		//绘制(卞涛修改,对最后一行进行判断)
		if(iBool&&curIndex>iLastIndex)
		{
			gc.DrawText_Red(point,aDes);
		}
		else
		{
			gc.DrawText(point,aDes);
		}
		point.iY += iLineHeight;
		point.iY += iRowSpacing;
	}
	iScrollbar->Draw(gc);
}

TBool CMultiText::KeyEventL(TInt aKeyCode)
{
	Assert();
	TBool keyResult = EFalse;
	keyResult = iScrollbar->KeyEventL(aKeyCode);

	if(iObserver)
	{
		if(GetEndFlag())
		{
			iObserver->MultiEvent(1);
		}
		else
		{
			iObserver->MultiEvent(0);
		}
	}
	return keyResult;
}

TBool CMultiText::HandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CMultiText::SizeChanged(const TRect& /*aScreenRect*/)
{

}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CMultiText::AppendL(const TDesC& aDes)
{
// 	//WriteLogsL(_L("aDes length = %d"),aDes.Length());
// 	//WriteLogsL(_L("CMultiText::AppendL iLineWidth = %d"),iLineWidth);
	ASSERT(iLineWidth > 0 );
	if(iDesArray == NULL)
	{
		iDesArray = new (ELeave) CDesCArrayFlat(10);
	}
	//根据行宽拆分字符串,放置在iDesArray里
	TInt textPos = 0;
	////WriteLogsL(_L("CMultiText::AppendL 1"));
 	while(textPos < aDes.Length())
	{	
		TInt charNum = iFont->TextCount(aDes.Mid(textPos),iLineWidth);
 		const TDesC& tmpDes = aDes.Mid(textPos,charNum);

/*
 		TInt tmpPos = tmpDes.Find(_L("\r\n"));

		if(tmpPos != KErrNotFound)
		{
			charNum = tmpPos;
		}
*/

		iDesArray->AppendL(aDes.Mid(textPos,charNum));
		textPos += charNum;

/*
		if(tmpPos != KErrNotFound)
		{
			textPos += 2;
		}
*/

	}
//	//WriteLogsL(_L("CMultiText::AppendL 2"));
	InitScrollbarL();
//	//WriteLogsL(_L("CMultiText::AppendL 3"));
}
void CMultiText::AppendEndItem(const TDesC& aText)
{

	iLastIndex=iDesArray->Count()-1;
	AppendL(aText);
	iBool=ETrue;
	//InitScrollbarL();
}
void CMultiText::LoadFromFileL(const TDesC& aFileName)
{
	RFs& fs = CCoeEnv::Static()->FsSession();
	HBufC8* fileContent = UtilityTools::GetFileContentL(fs,aFileName);
	CleanupStack::PushL(fileContent);

	TPtrC ptr((const TUint16*)(fileContent->Ptr()),fileContent->Length()/2);
	//处理FF FE前缀
	const TDesC& des = ptr.Mid(1);

	//有两个或多个回车换行的情况下只会换行一次,暂未处理
	//最后一行没有回车时解析错误,暂未处理
	TInt textPos = 0;
	while(textPos < des.Length())
	{	
		const TDesC& tmpDes = des.Mid(textPos);
		TInt charNum = tmpDes.Find(_L("\r\n"));
		if(charNum == KErrNotFound)
		{
			AppendL(tmpDes);
		}
		else
		{
			AppendL(des.Mid(textPos,charNum));
		}
		textPos += charNum;
		textPos += 2;
	}	
	CleanupStack::PopAndDestroy();		//fileContent
}

//////////////////////////////////////////////////////////////////////////
void CMultiText::SetScrollSize( TInt aScrollWidth,TInt aScrollHeight )
{
	iScrollWidth = aScrollWidth;
	iScrollHeight = aScrollHeight;
}

void CMultiText::SetClientRect(const TRect& aRect)
{
	iClientRect = aRect;
}

void CMultiText::SetMargin(TInt aLeftRight,TInt aTopBottom)
{
	iLeftRightMargin = aLeftRight;
	iTopBottomMargin = aTopBottom;
}

void CMultiText::Layout(const TRect& aRect,const CFont* aFont,TInt aScrollBarWidth,TInt aRowSpace)
{
	ASSERT(aRect.Width() > 0 && aRect.Height() > 0);
	ASSERT(aFont);
	//ASSERT(aFont);										//该函数必须在字体被设置后才可以调用

	//WriteLogsL(_L("CMultiText::Layout"));
	iFont = aFont;
	iClientRect = aRect;
	//计算可以显示行数及列数
	iLineHeight = iFont->HeightInPixels();			//计算行高,参数设置为一组汉字
	//iRowSpacing += iLineHeight/3;						//行高设置为字体高度的1.5 倍
	//iRowSpacing += (iLineHeight>>1);						//行高设置为字体高度的1.5 倍
	iRowSpacing = aRowSpace;

	TInt textVerticalSpace = iClientRect.Height();
	textVerticalSpace -= (iTopBottomMargin<<1);
	textVerticalSpace += iRowSpacing;
	iMaxLinePerPage = textVerticalSpace/(iLineHeight + iRowSpacing);		//该数值应该通过屏幕尺寸与行高计算获得
	iLineWidth = iClientRect.Width();
	iLineWidth -= (iLeftRightMargin<<1);
	iLineWidth -= iScrollWidth;
	iStartPoint = iClientRect.iTl;
	iStartPoint.iX += iLeftRightMargin;
	iStartPoint.iY += iTopBottomMargin;


	//WriteLogsL(_L("CMultiText::Layout pre CScrollbar iLineWidth = %d"),iLineWidth);

	iScrollbar = new (ELeave)CScrollBar;
	TPoint scrollPoint = iClientRect.iTl;
	scrollPoint.iX += iClientRect.Width();
	scrollPoint.iX -= aScrollBarWidth;
	iScrollbar->SetBeginPostion(scrollPoint);
	iScrollbar->SetVerticalSpace(iClientRect.Height());
	iScrollbar->SetScrollWidth(aScrollBarWidth);
	iScrollbar->SetBackgroundColor(KRgb1);
	iScrollbar->SetCursorColor(KRgb2);
	iScrollbar->SetMaxLinePerPage(iMaxLinePerPage);
	iScrollbar->SetTotalLineNum(0);

	iScrollbar->Layout();
	//WriteLogsL(_L("CMultiText::Layout End"));
}

void CMultiText::InitScrollbarL()
{
	//WriteLogsL(_L("CMultiText::InitScrollbarL"));
	ASSERT(iScrollbar);
	if(!iScrollbar)
	{
/*
		if(iObserver)
		{
			iScrollbar = iObserver->CreateScrollbarL();
		}
		else*/

		{
// 			iScrollbar = new (ELeave)CScrollbar;
// 			iScrollbar->DefaultLayout();
// 			LayoutScrollbar();
/*
			iScrollbar = new(ELeave)CScrollbar;

			iScrollbar->SetBeginPostion(point);
			iScrollbar->SetVerticalSpace(rect.Height());
			iScrollbar->SetScrollWidth(1);
			iScrollbar->SetBackgroundColor(KRgb1);
			iScrollbar->SetCursorColor(KRgb2);
			iScrollbar->SetMaxLinePerPage(iScreenLayout->LineNum());
			iScrollbar->SetTotalLineNum(0);

			scrollbar->Layout();
			*/

/*
			iScrollbar = new (ELeave)CScrollbar;
			TPoint scrollPoint = aRect.iTl;
			scrollPoint.iX += aRect.Width();
			scrollPoint.iX -= scrollBarWidth;
			iScrollbar->SetBeginPostion(scrollPoint);
			iScrollbar->SetVerticalSpace(aRect.Height());
			iScrollbar->SetScrollWidth(scrollBarWidth);
			iScrollbar->SetBackgroundColor(KRgb1);
			iScrollbar->SetCursorColor(KRgb2);
			iScrollbar->SetMaxLinePerPage(iMaxLinePerPage);
			iScrollbar->SetTotalLineNum(0);
*/

		}
	}
	iScrollbar->SetMaxLinePerPage(iMaxLinePerPage);
	iScrollbar->SetTotalLineNum(iDesArray->Count());

	iScrollbar->Layout();
	//WriteLogsL(_L("CMultiText::InitScrollbarL End"));
}

void CMultiText::SetTextFont(const CFont* aFont)
{
	iFont = aFont;
}

void CMultiText::SetTextColor(const TRgb& aColor)
{
	iTextColor = aColor;
}

CScrollBar* CMultiText::Scrollbar()const
{
	ASSERT(iScrollbar);
	return iScrollbar;
}

void CMultiText::Assert()const
{
	ASSERT(iFont);
	ASSERT(iScrollbar);
}
TBool CMultiText::GetEndFlag()
{
	TBool returnBool=EFalse;

	if(iBool&&(iScrollbar->CurIndex()+iMaxLinePerPage-1)>iLastIndex)
	{
		returnBool=ETrue;
	}

	return returnBool;
}
void CMultiText::SetObserver(MMultiObserver* aObserver)
{
	iObserver=aObserver;
}