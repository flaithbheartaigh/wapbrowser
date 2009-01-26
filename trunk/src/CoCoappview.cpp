/*
============================================================================
 Name        : CoCoView.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Application view
============================================================================
*/

// INCLUDE FILES
#include <coemain.h>
#include "CoCoAppView.h"

#include "MainEngine.h"
#include "MainWindow.h"
#include "Graphic.h"
#include "EditorsManager.h"
#include "PreDefine.h"
#include "TypeDefine.h"

#include <aknenv.h>
#include <avkon.hrh>


const TInt KRedrawInterval = 100000;
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCoCoAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCoCoAppView* CCoCoAppView::NewL( const TRect& aRect )
    {
    CCoCoAppView* self = CCoCoAppView::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCoCoAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCoCoAppView* CCoCoAppView::NewLC( const TRect& aRect )
    {
    CCoCoAppView* self = new ( ELeave ) CCoCoAppView;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CCoCoAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCoCoAppView::ConstructL( const TRect& /*aRect*/ )
    {
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
	//SetRect( aRect );
	ApplicationStartL();

	SetExtentToWholeScreen();
	
    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CCoCoAppView::CCoCoAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCoCoAppView::CCoCoAppView()
    {
    // No implementation required
    }


// -----------------------------------------------------------------------------
// CCoCoAppView::~CCoCoAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CCoCoAppView::~CCoCoAppView()
    {
	ApplicationEnd();
    // No implementation required
    }


// -----------------------------------------------------------------------------
// CCoCoAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CCoCoAppView::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();

	CGraphic& graphic = iMainEngine->Graphic();
	const CFbsBitmap* offScreenBitmap = graphic.GetOffScreenBitmap();
	gc.BitBlt(TPoint(0,0),offScreenBitmap);
  	}

// -----------------------------------------------------------------------------
// CCoCoAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CCoCoAppView::SizeChanged()
    {  
	//事件发生时需要调用SetExtentToWholeScreen(),将导致无穷嵌套递归
	//所以不能直接调用界面的SizeChanged(),而在定时器里调用
	iSizeChanged = ETrue;
    }

TKeyResponse CCoCoAppView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	//iMainEngine->WriteLog16(_L("CCoCoAppView::OfferKeyEventL"));
	ASSERT(iCurWindow);

	TKeyResponse keyResponse = EKeyWasNotConsumed;

	CEditorsManager& editorsManager = iMainEngine->EditorsManager();
	if(aKeyEvent.iCode != '*' || aKeyEvent.iCode != '#')
	{
		keyResponse = editorsManager.OfferKeyEventL(aKeyEvent,aType);
	}

	if(keyResponse == EKeyWasNotConsumed && aType == EEventKey)
	{
		ASSERT(iCurWindow);
		//iMainEngine->WriteLog16(_L("CCoCoAppView::OfferKeyEventL iCurWindow->KeyEventL pre"));
		TBool result = iCurWindow->KeyEventL(aKeyEvent.iCode);
		//iMainEngine->WriteLog16(_L("CCoCoAppView::OfferKeyEventL iCurWindow->KeyEventL End"));
		keyResponse = result ? EKeyWasConsumed : EKeyWasNotConsumed;
	}
	if(keyResponse)
	{
		RequestDraw();
	}
	//iMainEngine->WriteLog16(_L("CCoCoAppView::OfferKeyEventL End"));
	return keyResponse;
}

TInt CCoCoAppView::CountComponentControls() const
{
	CEditorsManager& editorsManager = iMainEngine->EditorsManager();
	return editorsManager.CountComponentControls();
}

CCoeControl* CCoCoAppView::ComponentControl(TInt aIndex) const
{
	CEditorsManager& editorsManager = iMainEngine->EditorsManager();
	return editorsManager.ComponentControl(aIndex);
}
//////////////////////////////////////////////////////////////////////////
//From MTimerNotifier
//////////////////////////////////////////////////////////////////////////
TBool CCoCoAppView::DoPeriodTask()
{
	//iMainEngine->WriteLog16(_L("CCoCoAppView::DoPeriodTask"));
	ASSERT(NULL != iMainEngine);
	if(iSizeChanged)
	{
		SetExtentToWholeScreen();
		iMainEngine->SetScreenRect(Rect());
		iMainWindow->SizeChanged();
		iIsDraw = ETrue;
		iSizeChanged = EFalse;
// 		TBuf<10> buf;
// 		buf.AppendNum(Rect().Width());
// 		buf.Append(_L(":"));
// 		buf.AppendNum(Rect().Height());
// 
// 		//User::InfoPrint(buf);

	}
	if(iIsDraw)
	{
		////UtilityTools::InfoPrint(iDrawCount++);
		DrawWindow();
		iIsDraw = EFalse;
	}
	//iMainEngine->WriteLog16(_L("CCoCoAppView::DoPeriodTask End"));
	return ETrue;
}
//////////////////////////////////////////////////////////////////////////
//public
//////////////////////////////////////////////////////////////////////////
void CCoCoAppView::RequestDraw()
{
	iIsDraw = ETrue;
}

void CCoCoAppView::SetCurWindow(CWindow* aWindow)
{
	ASSERT(aWindow);	//必须有一个界面作为当前界面,
	if(iCurWindow)		//除了第一次调用,iCurWindow总是应该有值的
	{
		iCurWindow->Deactivate();
	}
	iCurWindow = aWindow;
	iCurWindow->ActivateL();
	//iCurWindow->SizeChanged();
	RequestDraw();
}

CWindow* CCoCoAppView::CurWindow() const
{
	ASSERT(iCurWindow);
	return iCurWindow;
}

TInt CCoCoAppView::GetEditorInputMethod()
{
	MAknEditingStateIndicator * ei = CAknEnv::Static()->EditingStateIndicator() ;
	if (!ei)   return 0;

	CAknIndicatorContainer *ic = ei->IndicatorContainer();        
	if (!ic)   return 0;

	if(ic->IndicatorState(S_uid(EAknNaviPaneEditorIndicatorNumberCase)))
	{
		//to do ... if input method =  "123";
		return 1;
	}
	else if(ic->IndicatorState(S_uid(EAknNaviPaneEditorIndicatorTextCase)))
	{
		//to do ... if input method =  "Abc";
		return 2;
	}
	else if(ic->IndicatorState(S_uid(EAknNaviPaneEditorIndicatorLowerCase)))
	{
		//to do ... if input method = "abc";
		return 3;
	}
	else if(ic->IndicatorState(S_uid(EAknNaviPaneEditorIndicatorUpperCase)))
	{
		//to do ... if input method = "ABC";
		return 4;
	}
	else if(ic->IndicatorState(S_uid(EAknNaviPaneEditorIndicatorT9) ))
	{
		//to do ... if input method = T9 (即拼音和笔画)
		return 5;
	}
	return 0;
}

//下面两个函数似乎无效
void CCoCoAppView::SetClippingRect(const TRect& aRect)
{
	ActivateGc();
	CWindowGc& gc = SystemGc();
	gc.SetClippingRect(aRect);
	DeactivateGc();
}

void CCoCoAppView::SetClippingRegion(const TRegion& aRegion)
{
	ActivateGc();
	CWindowGc& gc = SystemGc();
	gc.SetClippingRegion(aRegion);
	DeactivateGc();
}

void CCoCoAppView::CancelClipping()
{
	ActivateGc();
	CWindowGc& gc = SystemGc();
	gc.CancelClippingRect();
	gc.CancelClippingRegion();
	DeactivateGc();
}

//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CCoCoAppView::ApplicationStartL()
{
	UtilityTools::WriteLogsL(_L("CCoCoAppView::ApplicationStartL 0"));

	iMainEngine = CMainEngine::NewL(*this);
	UtilityTools::WriteLogsL(_L("CCoCoAppView::ApplicationStartL 1"));
	iMainWindow = CMainWindow::NewL(*iMainEngine,*this);
	UtilityTools::WriteLogsL(_L("CCoCoAppView::ApplicationStartL 2"));

	iTimer = CNotifyTimer::NewL(*this);
	iTimer->Start(KRedrawInterval);

	UtilityTools::WriteLogsL(_L("CCoCoAppView::ApplicationStartL 3"));
	iIsDraw = ETrue;
}

void CCoCoAppView::ApplicationEnd()
{
	delete iMainWindow;
	iMainWindow = NULL;
	delete iMainEngine;
	iMainEngine = NULL;
	delete iTimer;
	iTimer = NULL;
}

void CCoCoAppView::DrawWindow() const
{
	ASSERT(iCurWindow);
	CGraphic& graphic = iMainEngine->Graphic();
	iCurWindow->Draw(graphic);
	DrawNow();
}
// End of File
