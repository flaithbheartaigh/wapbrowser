
#include <eikappui.h>
#include "BrowserBookWindow.h"
#include "BookEngine.h"
#include "PopUpMenu.h"
#include "Dialog.h"
#include "TitleBar.h"
#include "MainEngine.h"
#include "Graphic.h"
#include "TypeDefine.h"
#include "ScrollBar.h"
#include "ScreenLayout.h"
#include "ControlFactory.h"
#include "CoCoPreDefine.h"

#define MaxCharCount 40	

enum TEventPopCommand
{
	EPopFullScreen,
	EPopFirstPage,
	EPopUpPage,
	EPopNextPage,
	EPopLastPage,
	EPopSetMark,
	EPopReturnMark,
};

CBrowserBookWindow::CBrowserBookWindow(CWindow* aParent,CMainEngine& aMainEngine)
:CWindow(EOpenFileWindow_Book,aParent,aMainEngine),
iMainEngine(aMainEngine),
iCurrentLine(0),
iBlooder(0),
iLastLoadCount(0),
iLastPos(0),
iCurrentPos(0),
iHasMore(ETrue),
iHasPrev(EFalse),
iFullScreenMode(EFalse),
iRawUnicodeBuf(NULL),
iBookEngine(NULL)
{

}

CBrowserBookWindow::~CBrowserBookWindow()
{
	if(iBookEngine)
	{
		DELETE(iBookEngine);
		iBookEngine=NULL;
	}

	if(iCurrentArrayBuf.Count() > 0)
		iCurrentArrayBuf.ResetAndDestroy();

	DELETE( iRawUnicodeBuf );  //不是当前显示的

	iLastPosArray.Reset();
}

CBrowserBookWindow* CBrowserBookWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CBrowserBookWindow* self = new (ELeave)CBrowserBookWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL( );
	return self;
}

CBrowserBookWindow* CBrowserBookWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CBrowserBookWindow* self=CBrowserBookWindow::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CBrowserBookWindow::ConstructL()
{
	iFileName.Zero();
	iCurrentArrayBuf.Reset();
	iLastPosArray.Reset();

	ChangeButton();

	((CControlFactory&)iMainEngine.ControlFactory()).SetParentWindow(this);

	iTitleBar=iMainEngine.ControlFactory().CreateDefaultTitleBarL(_L(""));

	iShowControlText=EFalse;

	//不允许弹出0*#界面
	iCanPopUpWindow=EFalse;
}

void CBrowserBookWindow::DoActivateL()
{
	//CTitleBar* titleBar = iMainEngine.ControlFactory().CreateDefaultTitleBarL(*iTitle);
}

void CBrowserBookWindow::DoDeactivate()
{
	//DeleteAllControl();
}

void CBrowserBookWindow::DoDraw(CGraphic& aGraphic) const
{
	TInt nMaxLinePerPage=0;
	TInt nMaxLinePerPageOfFullScreen=0;

	TRect nCacScreen=iMainEngine.ScreenLayout().WhiteBackgroundRect();
	nMaxLinePerPage=(nCacScreen.iBr.iY-nCacScreen.iTl.iY-5)/iMainEngine.ScreenLayout().LineHeight();

	TRect nFullScreen=iMainEngine.ScreenLayout().ScreenSize() ;	
	nMaxLinePerPageOfFullScreen=(nFullScreen.iBr.iY-nFullScreen.iTl.iY-5)/iMainEngine.ScreenLayout().LineHeight();

	if ( iFullScreenMode )
	{
		DrawBackground(aGraphic);

		TSize nScreen=iMainEngine.ScreenLayout().ScreenSize();

		TPoint point( 0, 0 );

		aGraphic.SetPenColor(TRgb(255,255,255));
		aGraphic.SetPenStyle(CFbsBitGc::ESolidPen);

		for(TInt i = 0; 
			i < nMaxLinePerPageOfFullScreen && i < iCurrentArrayBuf.Count() &&
			( iCurrentLine <  nMaxLinePerPageOfFullScreen || 
			iCurrentLine < (iCurrentArrayBuf.Count() - nMaxLinePerPageOfFullScreen + 1)); 
		i++)
		{
			aGraphic.DrawText(TPoint(iMainEngine.ScreenLayout().GetClientRect().iTl.iX +3  , 5 + i * iMainEngine.ScreenLayout().LineHeight()) + point,*iCurrentArrayBuf[i + iCurrentLine]);
		}
		aGraphic.SetPenStyle(CFbsBitGc::ENullPen);
	}
	else
	{
		TPoint point(iMainEngine.ScreenLayout().WhiteBackgroundRect().iTl.iX,iMainEngine.ScreenLayout().WhiteBackgroundRect().iTl.iY);

		aGraphic.SetPenColor(TRgb(255,255,255));
		aGraphic.SetPenStyle(CFbsBitGc::ESolidPen);

		const CFont* pFont = aGraphic.GetFont();
		TParse parseFileName;
		parseFileName.Set(iFileName,NULL,NULL);
		TPtrC ptr = parseFileName.Name();

		iTitleBar->SetTextL(ptr);


		for(TInt i = 0; 
			i < nMaxLinePerPage && i < iCurrentArrayBuf.Count() &&
			( iCurrentLine <  nMaxLinePerPage ||
			iCurrentLine < (iCurrentArrayBuf.Count() - nMaxLinePerPage + 1)); 
		i++)
		{
			//	10 pixels right to the left edge of text frame
			//	height = 19, space between lines = 5
			aGraphic.DrawText(TPoint(3, 5 + i * iMainEngine.ScreenLayout().LineHeight()) + point,*iCurrentArrayBuf[i + iCurrentLine]);
		}
		aGraphic.SetPenStyle(CFbsBitGc::ENullPen);

	}

	ASSERT(iScrollbar);
	
	iScrollbar->Draw(aGraphic);
}

TBool CBrowserBookWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;

	switch(aKeyCode)
	{
	case KKeyCodeUp:
		iScrollbar->Up();

		iCurrentLine=iScrollbar->CurIndex();
		break;
	case KKeyCodeDown:
		iScrollbar->Down();

		iCurrentLine=iScrollbar->CurIndex();
		break;
	case KKeyCodeLeft:
		DoPopUpMenuCommand(EPopUpPage);
		break;
	case KKeyCodeRight:
		DoPopUpMenuCommand(EPopNextPage);
		break;
	case KOKKey:	
		if ( !iFullScreenMode )
		{
			SetFullScreenMode( ETrue);
		}
		break;
	case KLeftSoftKey:
		InitPopUpMenu();
		break;
	case KRightSoftKey:
		if ( iFullScreenMode )
		{
			SetFullScreenMode( EFalse );
		}
		else
		{
			//iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_BookMark_IsSet),*this,1);
			SetBookMark();

			GoBackToParentWindow();

			return keyResult;
		}
		break;
	case '2':
		DoPopUpMenuCommand(EPopFirstPage);
		break;
	case '4':
		DoPopUpMenuCommand(EPopUpPage);
		break;
	case '6':
		DoPopUpMenuCommand(EPopNextPage);
		break;
	case '8':
		DoPopUpMenuCommand(EPopLastPage);
		break;
	case '0':
		DoPopUpMenuCommand(EPopSetMark);
		break;
	case '5':
		DoPopUpMenuCommand(EPopReturnMark);
		break;
	default:
		keyResult = EFalse;
		break;
	}

	//将DoDraw中改变成员变量的语句移出
	if ( iFullScreenMode )
	{
		TInt nMaxLinePerPageOfFullScreen=0;

		TRect nFullScreen=iMainEngine.ScreenLayout().ScreenSize() ;	
		nMaxLinePerPageOfFullScreen=(nFullScreen.iBr.iY-nFullScreen.iTl.iY-5)/iMainEngine.ScreenLayout().LineHeight();

		if ( iCurrentLine > (iCurrentArrayBuf.Count() - nMaxLinePerPageOfFullScreen + 1) &&
			(iCurrentArrayBuf.Count() - nMaxLinePerPageOfFullScreen + 1) > 0)
		{
			//???????????????????????????????
			iCurrentLine = iCurrentArrayBuf.Count()-nMaxLinePerPageOfFullScreen;
		}

		iIsNormalWindow=false;
	}
	else
	{
		iIsNormalWindow=true;
	}

	ShowTitleBar();

	//iMainEngine.RequestDraw();

	return keyResult;
}

TBool CBrowserBookWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CBrowserBookWindow::DoSizeChanged()
{

}

void CBrowserBookWindow::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	SetLeftButtonL(textRes[ETurkeyTextRes_Option]);
	SetRightButtonL(textRes[ETurkeyTextRes_Back]);
}

TInt CBrowserBookWindow::SetFullScreenMode( TBool bFullScreen )
{
	iFullScreenMode = bFullScreen;

	//20070626张晖增加
	InitScrollHeight();

	return ETrue;
}

void CBrowserBookWindow::LoadBookSection()
{
	if(NULL == iBookEngine)
	{
		iBookEngine = new(ELeave)CBookEngine();
	}

	if(iFileName.Length() > 0)
	{
		delete( iRawUnicodeBuf );
		iHasMore = iBookEngine->LoadSectionL( iFileName, iRawUnicodeBuf, iLastLoadCount );	
		iHasPrev = EFalse;
		FormatRawBufToLines();;
		iCurrentPos += iLastLoadCount;//生成当前要显示的ArrayBuf
	}
}

TBool CBrowserBookWindow::FormatRawBufToLines()
{
	if( NULL == iRawUnicodeBuf )
		return 0;

	TBuf<MaxCharCount>  aLineBuf;

	TUint16*	nTotalBuf = (TUint16*)iRawUnicodeBuf->Ptr();
	TUint		ch = 0x20;//空格
	TInt		nRawLength  = iRawUnicodeBuf->Length();

	TInt nMaxPixelPerLine=0;
	TRect nScreen=iMainEngine.ScreenLayout().WhiteBackgroundRect ();
	nMaxPixelPerLine=nScreen.iBr.iX - nScreen.iTl.iX-3;  

	TBuf<100> testOly;
	testOly.Append( _L(",") );
	testOly.ZeroTerminate();
	TInt aTestSize =iMainEngine.ScreenLayout().Font()->TextWidthInPixels( testOly );

	for( TInt i = 0; i < (nRawLength - 1); )
	{
		for( TInt k = 0; k < aLineBuf.MaxLength(); k++,i++ )
		{
			if( i == 0 && nTotalBuf[i] == 0xfeff )//调过用Txt生成的unicode标致
			{
				k--;
				continue;
			}
			else if( nTotalBuf[i] == 0x0d &&  nTotalBuf[i+1] == 0x0a )
			{
				i+=2;
				break;//会将i++跳过 所以i+2
			}
			else if((nTotalBuf[i]  >= 0 && nTotalBuf[i] < 32 ) || (nTotalBuf[i] >= 127 && nTotalBuf[i] < 160))
			{

				aLineBuf.Append(ch);		  		
			}
			else
			{
				aLineBuf.Append(nTotalBuf[i]);	
			}

			//	It will at least hold 13 characters
			//	So it doesn't need to check width from the beginning
			//	The API is really time consuming
			if ( iMainEngine.ScreenLayout().Font()->TextWidthInPixels( aLineBuf ) > nMaxPixelPerLine)
			{
				//aLineBuf[ aLineBuf.Length() - 1 ] = 0;
				aLineBuf.SetLength( aLineBuf.Length() - 1 );
				break;
			}
		}
		HBufC* ptr = HBufC::NewL(aLineBuf.Length()); 
		*ptr = aLineBuf;

		iCurrentArrayBuf.Append(ptr); 	
		aLineBuf.Delete(0,aLineBuf.Length());
	}

	//20070626张晖增加
	InitScrollHeight();

	return ETrue;
}

void CBrowserBookWindow::SetFileName(const TDesC& aDes)
{
	if(iFileName.Length() > 0)
		iFileName.Delete(0,iFileName.Length());

	iFileName.Copy(aDes);
	if(NULL == iBookEngine)
	{
		iBookEngine = new(ELeave)CBookEngine();	
	}
	iBookEngine->SetPath(iFileName);

	LoadBookSection();
}

void CBrowserBookWindow::SetFileName(const TDesC& aDes,const TInt aLine)
{
	//20070629张晖增加
	SetFileName(aDes);

	iCurrentLine=aLine;

	if(iScrollbar)
	{
		iScrollbar->SetIndex (iCurrentLine);
	}

}

void CBrowserBookWindow::InitPopUpMenu()
{
	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);

	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_FullScreen),EPopFullScreen);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Book_FirstPage),EPopFirstPage);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Book_UpPage),EPopUpPage);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Book_NextPage),EPopNextPage);

	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Book_LastPage),EPopLastPage);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_BookMark_SetMark),EPopSetMark);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_BookMark_ReturnMark),EPopReturnMark);
}

void CBrowserBookWindow::DoPopUpMenuCommand(TInt aCommand)
{
	TInt nMaxLinePerPage=0;
	TInt nMaxLinePerPageOfFullScreen=0;

	TRect nCacScreen=iMainEngine.ScreenLayout().GetClientRect ();
	nMaxLinePerPage=(nCacScreen.iBr.iY-nCacScreen.iTl.iY-5)/iMainEngine.ScreenLayout().LineHeight();

	TRect nFullScreen=iMainEngine.ScreenLayout().ScreenSize() ;	
	nMaxLinePerPageOfFullScreen=(nFullScreen.iBr.iY-nFullScreen.iTl.iY-5)/iMainEngine.ScreenLayout().LineHeight();

	switch(aCommand)
	{
	case EPopFullScreen:
		{
			//全屏显示
			SetFullScreenMode( ETrue );
		}
		break;
	case EPopFirstPage:
		{
			//首页
			iCurrentLine = 0;

			iScrollbar->Home ();
		}
		break;
	case EPopUpPage:
		{
			//上一页
			if ( !iFullScreenMode )
			{
				if(iCurrentArrayBuf.Count()<=nMaxLinePerPage)
					break;

			}
			else
			{
				if(iCurrentArrayBuf.Count()<=nMaxLinePerPageOfFullScreen )
					break;
			}

			iScrollbar->PageUp();

			iCurrentLine=iScrollbar->CurIndex(); 
		}
		break;
	case EPopNextPage:
		{
			//下一页
			if ( !iFullScreenMode && iCurrentArrayBuf.Count() <= nMaxLinePerPage )
			{
				break;
			}

			if ( iFullScreenMode && iCurrentArrayBuf.Count() <= nMaxLinePerPageOfFullScreen )
			{
				break;
			}

			iScrollbar->PageDown ();

			iCurrentLine=iScrollbar->CurIndex(); 
		}
		break;
	case EPopLastPage:
		{
			//尾页
			if ( !iFullScreenMode && iCurrentArrayBuf.Count() <= nMaxLinePerPage )
			{
				break;
			}

			if ( iFullScreenMode && iCurrentArrayBuf.Count() <= nMaxLinePerPageOfFullScreen )
			{
				break;
			}

			iScrollbar->End ();

			iCurrentLine=iScrollbar->CurIndex(); 
		}
		break;
	case EPopSetMark:
		{
			//设置书签
			TInt i=SetBookMark();
			if(i!=KErrNone)
			{
				iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_BookMark_SetFail),*this,0);
			}
			else
			{
				iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_BookMark_SetSuccess),*this,0);
			}
		}
		break;
	case EPopReturnMark:
		{
			//返回书签
			TInt i=GetBookMark();

			if(i!=-1)
			{
				iCurrentLine=i;

				iScrollbar->SetIndex (iCurrentLine);
			}
		}
		break;
	default:
		ASSERT(EFalse);
		break;
	}
}

void CBrowserBookWindow::InitScrollHeight()
{
	TInt tmpCurIndex = iScrollbar ? iScrollbar->CurIndex() : 0;
	TRect rect;
	TPoint point;
	TInt maxLinePerPage = 0;
	if (iFullScreenMode)
	{
		rect = iMainEngine.ScreenLayout().ScreenSize() ;	
		maxLinePerPage=(rect.iBr.iY-rect.iTl.iY-5)/iMainEngine.ScreenLayout().LineHeight();
	}
	else
	{
		rect =iMainEngine.ScreenLayout().WhiteBackgroundRect();
		maxLinePerPage=(rect.iBr.iY-rect.iTl.iY-5)/iMainEngine.ScreenLayout().LineHeight();

		point = rect.iTl;
		point.iX =  iMainEngine.ScreenLayout().ScreenSize().iWidth- iMainEngine.ScreenLayout().ScrollBarWidth();
	}
	delete iScrollbar;
	iScrollbar = NULL;
	iScrollbar = iMainEngine.ControlFactory().CreateScrollBar(rect);

	if (!iFullScreenMode)
	{
		iScrollbar->SetBeginPostion(point);
	}

	iScrollbar->SetMaxLinePerPage(maxLinePerPage);
	iScrollbar->SetTotalLineNum(iCurrentArrayBuf.Count());
	iScrollbar->Layout();
	
	iScrollbar->SetIndex(tmpCurIndex);
}

TInt CBrowserBookWindow::SetBookMark()
{
	TParse parseFileName;
	parseFileName.Set(iFileName,NULL,NULL);
	TPtrC ptr = parseFileName.Name();

	TInt i = iMainEngine.DBEngine().GetBookMarksByKeyL (_L("BookName"),ptr);

	TInt nRet;
	if(i==-1)
	{
		nRet=iMainEngine.DBEngine().AddBookMarkWithSql(ptr, iCurrentLine);
	}
	else
	{
		nRet=iMainEngine.DBEngine().UpdateBookMarkLine (ptr,iCurrentLine);
	}

	return nRet;
}

TInt CBrowserBookWindow::GetBookMark()
{
	TParse parseFileName;
	parseFileName.Set(iFileName,NULL,NULL);
	TPtrC ptr = parseFileName.Name();

	TInt i = iMainEngine.DBEngine().GetBookMarksByKeyL (_L("BookName"),ptr);

	return i;
}


void CBrowserBookWindow::DialogEvent(TInt aCommand)
{
	//???????????????????????
	/*if (aCommand == 1)
	{
		SetBookMark();

		GoBackToParentWindow();
	}*/
}

void CBrowserBookWindow::ShowTitleBar()
{
	if(iFullScreenMode)
	{
		//全屏不显示TitleBar
		RemoveControl(iTitleBar);
	}
	else
	{
		AddControl(iTitleBar);
	}
}