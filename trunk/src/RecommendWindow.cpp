/*
============================================================================
 Name        : RecommendWindow.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CRecommendWindow implementation
============================================================================
*/

#include <rpbkviewresourcefile.h> 
#include <CPbkMultipleEntryFetchDlg.h>
#include <cpbkcontactitem.h> 
#include <cpbkphonenumberselect.h> 
#include <cpbkcontactengine.h> 
#include <CNTDB.H> 
#include <CNTDEF.H> 
#include <eikgted.h>
#include <eikenv.h>

#include "RecommendWindow.h"
#include "MainEngine.h"
#include "EditorsManager.h"
#include "ScreenLayout.h"
#include "ControlFactory.h"
#include "HttpManager.h"
#include "PopUpMenu.h"
#include "MultiText.h"
#include "Dialog.h"
#include "ScrollBar.h"
#include "TypeDefine.h"
#include "cocobmp.h"
#include "CoCoPreDefine.h"
#include "BitmapFactory.h"
#include "TitleBar.h"
enum TEventCmd
{
	ECmdOK,
	ECmdGoBackToParentWindow,
	ECmdGoBackToMainWindow,
	ECmdOpenPhoneBook,
	ECmdWaitingResponse,
	ECmdHideEdwin,
};

CRecommendWindow::CRecommendWindow(CWindow* aParent,CMainEngine& aMainEngine) : CWindow(ERecommendWindow,aParent,aMainEngine)
																			  , iEditorsManager(aMainEngine.EditorsManager())
																			  , iScreenLayout(aMainEngine.ScreenLayout())
{
}


CRecommendWindow::~CRecommendWindow()
{
	#ifndef __SERIES60_3X__
		delete iPbkContactEngine;
		iPbkContactEngine=NULL;
	#endif
}

CRecommendWindow* CRecommendWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CRecommendWindow* self = new (ELeave)CRecommendWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CRecommendWindow* CRecommendWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CRecommendWindow* self=CRecommendWindow::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CRecommendWindow::ConstructL()
{
	ChangeButton();

	#ifndef __SERIES60_3X__
		iPbkContactEngine = CPbkContactEngine::NewL();
	#endif
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CRecommendWindow::DoActivateL()
{
	//InitBitmaps();
	InitEdwin();
	InitMultiText();
	iTitleBar=iMainEngine.ControlFactory().CreateDefaultTitleBarL(iMainEngine.GetDesById(ETurkeyTextRes_Commend),this);
	
	iTimer = CNotifyTimer::NewL(*this);
 	iTimer->Start(100000);
}

void CRecommendWindow::DoDeactivate()
{
	DeleteAllControl();
	ResetEdwin();
	delete iTimer;
	iTimer = NULL;
}

void CRecommendWindow::DoDraw(CGraphic& aGraphic) const
{

}

TBool CRecommendWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case EKeyDevice0:
#ifdef __SERIES60_3X__
		RequestRecommend();
#else
		InitPopUpMenu();
#endif
		break;

	case EKeyDevice1:
		GoBackToParentWindow();
		break;

	case EKeyDevice3:
		RequestRecommend();
		break;

	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}

TBool CRecommendWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CRecommendWindow::DoSizeChanged()
{

}

void CRecommendWindow::ChangeButton()
{
#ifdef __SERIES60_3X__
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
#else
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Option));
#endif
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}
//////////////////////////////////////////////////////////////////////////
//From MPopUpMenuObserver
//////////////////////////////////////////////////////////////////////////
void CRecommendWindow::DoPopUpMenuCommand(TInt aCommand)
{
	switch(aCommand)
	{
	case ECmdOpenPhoneBook:
		{
		#ifndef __SERIES60_3X__
			CheckPhoneNumber();
		#endif
		}
		break;
	case ECmdOK:
		RequestRecommend();
		break;
	case ECmdGoBackToParentWindow:
		GoBackToParentWindow();
		break;
	case ECmdGoBackToMainWindow:
		GoBackToMainWindow();
		break;
	default:
		ASSERT(EFalse);
		break;
	}
}

void CRecommendWindow::PopUpDestried()
{
	iEditorsManager.ShowAllEdwins();
}
//////////////////////////////////////////////////////////////////////////
//From MOperationObserver
//////////////////////////////////////////////////////////////////////////
void CRecommendWindow::OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType)
{
	//User::InfoPrint(aEventData);
	TBool removeResult = RemoveAndDeleteControl(iDialog);
	//ASSERT(removeResult);
	iDialog = NULL;
	if(aEventType==0)
	{
		if(aEventData.Length() > 2)
		{
			iMainEngine.ControlFactory().CreateTipDialog(aEventData.Mid(2),*this,ECmdHideEdwin,this);
			//iMainEngine.ControlFactory().CreateTipDialog(_L("Successfully!"),*this,ECmdHideEdwin);
		}
		else
		{
			iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ECoCoTestRes_UnKownContentError),*this,ECmdHideEdwin,this);
		}
	}
	else
	{
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ECoCoTestRes_UnKownContentError),*this,ECmdHideEdwin,this);
	}

	iMainEngine.RequestDraw();
}
//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CRecommendWindow::DialogEvent(TInt aCommand)
{
	switch(aCommand)
	{
	case ECmdWaitingResponse:
		iMainEngine.GetHttpManager()->CancelTransaction(this,0);
		iEditorsManager.ShowAllEdwins();
		break;

	case ECmdHideEdwin:
		iEditorsManager.ShowAllEdwins();
		break;

	default:
		ASSERT(EFalse);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CRecommendWindow::InitPopUpMenu()
{
	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Ok),ECmdOK);
	#ifndef __SERIES60_3X__
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Commend_OpenPhoneBook),ECmdOpenPhoneBook);
	#endif
	//popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Back),ECmdGoBackToParentWindow);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Back),ECmdGoBackToMainWindow);
	iEditorsManager.HideAllEdwins();
}

void CRecommendWindow::InitMultiText()
{
	CMultiText* multiText = iMainEngine.ControlFactory().CreateDefaultMutliText();
	multiText->AppendL(iMainEngine.GetDesById(ETurkeyTextRes_Commend_Help1));
	multiText->AppendL(iMainEngine.GetDesById(ETurkeyTextRes_Commend_Help2));
	multiText->AppendL(iMainEngine.GetDesById(ETurkeyTextRes_Commend_Help3));
	multiText->Scrollbar()->SetShow(EFalse);
}

void CRecommendWindow::InitEdwin()
{
	ASSERT(NULL == iContentEdit);

	TInt fontHeight = iScreenLayout.FontHeight();
	TInt lineHeight = fontHeight+fontHeight/2;

	TInt maxLine = iScreenLayout.LineNum();

	TRect titleRect = iScreenLayout.GetNaviPaneRect();
	TRect backgroundRect = iScreenLayout.WhiteBackgroundRect();

	TSize screenSzie = iScreenLayout.ScreenSize();

	//TRect edwinRect(backgroundRect.iTl+TPoint(0,(fontHeight+fontHeight/5)*(maxLine-2)),TSize(backgroundRect.Width(),(fontHeight+fontHeight/2)*3));
	//edwinRect.iBr.iX -= iScreenLayout.ScrollBarWidth();
	TRect edwinRect(backgroundRect.iTl+TPoint(0,(fontHeight+fontHeight/5)*(maxLine-2)),TSize(backgroundRect.Width(),GetEdwinHeight()));

	iContentEdit = iEditorsManager.CreateGlbEdwin(edwinRect,100);
	iContentEdit->SetAknEditorCurrentInputMode(EAknEditorNumericInputMode);
	iEditorsManager.SetCurEdwin(iContentEdit);

}

void CRecommendWindow::ResetEdwin()
{
	iEditorsManager.SetCurEdwin(NULL);
	iEditorsManager.RemoveEdwinFromArray(iContentEdit);
	iContentEdit = NULL;
}

void CRecommendWindow::DrawInputType(CGraphic& aGraphic) const
{

}

void CRecommendWindow::RequestRecommend()
{
	HBufC* mobile = HBufC::NewLC(500);
	HBufC8* url = HBufC8::NewLC(500);
	TPtr ptr = mobile->Des();
	iContentEdit->GetText(ptr);
	if(mobile->Des().Length() == 0)
	{
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_Commend_InputMobile),*this,ECmdHideEdwin);
		iEditorsManager.HideAllEdwins();
		CleanupStack::PopAndDestroy(2);
		return;
	}
	if(!Check(*mobile))
	{
		iEditorsManager.HideAllEdwins();
		CleanupStack::PopAndDestroy(2);
		return;
	}
/*
	if(mobile->Des().Right(1).Compare(_L(";"))!=0)
	{
		mobile->Des().Append(_L(";"));
	}
	TInt er = HandelMobile(*mobile);

	if(er==0)
	{	
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_Commend_InputMobile),*this,ECmdHideEdwin);
		iEditorsManager.HideAllEdwins();
		CleanupStack::PopAndDestroy(2);
		return;
	}
*/

/*
	if(er > 5)
	{
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_Commend_CommendFive),*this,ECmdHideEdwin);
		iEditorsManager.HideAllEdwins();
		CleanupStack::PopAndDestroy(2);
		return;
	}
*/

	url->Des().Zero();
	url->Des().Append(_L8("http://59.36.96.182/3gserver/send.jsp?m="));
	url->Des().Append(*mobile);
	url->Des().Append(_L8("&p="));
	url->Des().AppendNum(7);	//待修改
	url->Des().Append(_L8("&im="));
	url->Des().Append(iMainEngine.GetIMEI());

	iMainEngine.GetHttpManager()->SendGetRequest(*this,*url);

	iDialog = iMainEngine.ControlFactory().CreateWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting),*this,ECmdWaitingResponse,this);
	iEditorsManager.HideAllEdwins();

	CleanupStack::PopAndDestroy(2);
}

TInt CRecommendWindow::HandelMobile(const TDesC& aDes)
{
	for(TInt i=0;i<aDes.Length();i++)
	{
		if(aDes.Mid(i,1).Compare(_L(";"))==0||aDes.Mid(i,1).Compare(_L("+"))==0)
			continue;

		TLex temp(aDes.Mid(i,1));
		TInt b;
		TInt iVal=temp.Val( b );
		TInt iLgt=temp.Remainder().Length();
		if((( iVal== KErrNone ) && ( iLgt== 0 )))
		{
			continue;
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

void CRecommendWindow::InitBitmaps()
{

}

/*
enum TCheckResult
{
	EOK,
	EExceedFive,
	EInvalid,
};
*/

TInt CRecommendWindow::Check(const TDesC& aDes,TInt aDepth)
{
	//return ETrue;

	TBool result = EFalse;
	if(aDepth == 5)		//递归深度达到5,说明超过了5个号码
	{
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_Commend_CommendFive),*this,ECmdHideEdwin);
	}
	else
	{
		TInt pos = aDes.Find(_L(";"));

		if(KErrNotFound == pos)	//没有找到分号,后面没有号码了
		{
			result = IsValidPhoneNumber(aDes);
			if(!result)
			{
				iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_Commend_InputMobile),*this,ECmdHideEdwin);
			}
		}
		else					//找到分号,后面可能还有号码
		{
			result = IsValidPhoneNumber(aDes.Left(pos));
			if(result)
			{
				if(aDes.Length() == pos + 1)	//待调试
				{
					result = IsValidPhoneNumber(aDes);
				}
				else
				{
					result = Check(aDes.Mid(pos + 1),aDepth + 1);
				}
			}
			else
			{
				iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_Commend_InputMobile),*this,ECmdHideEdwin);
			}
		}
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////
//暂时不使用
//////////////////////////////////////////////////////////////////////////
TBool CRecommendWindow::IsValidPhoneNumber(const TDesC& aDes) const
{
	return ETrue;

	HBufC* tmpBuf = aDes.AllocLC();
	//tmpBuf->Des().Trim();
	TBool result = EFalse;
	if(tmpBuf->Length() == 0)
	{
		result = ETrue;	//删掉无效空格以后,字符串长度为零,认为其是一个有效号码,以便判断
	}
	else
	{
		TLex lex(*tmpBuf);
		TInt64 phoneNumber = 0;
		TInt err = lex.Val(phoneNumber);

		if(KErrNone != err)
		{
			result = EFalse;
		}
		else
		{
// #ifdef __SERIES60_3X__
// 			if(phoneNumber.High() < (10000000000>>32) || phoneNumber.High() > (20000000000>>32))
// #else
// 			if(phoneNumber.High() < (10000000000>>32) || phoneNumber.High() > (20000000000>>32))
// #endif
/*
			if(phoneNumber.High() < (10000000000>>32) || phoneNumber.High() > (20000000000>>32))
			{
				result = EFalse;
			}
			else
			{
				result = ETrue;
			}
*/

		}
	}
	CleanupStack::PopAndDestroy();
	return result;
}

TInt CRecommendWindow::GetEdwinHeight() const
{
	TSize screenSize = iScreenLayout.ScreenSize();
	TInt edwinHeight = iScreenLayout.LineHeight()*3;
	if(screenSize.iWidth == 176&& screenSize.iHeight == 208)		//3250
	{
		edwinHeight += 4;
	}
	else if(screenSize.iWidth == 352&& screenSize.iHeight == 416)	//N80
	{
		edwinHeight += 4;
	}
	else if(screenSize.iWidth == 240&& screenSize.iHeight == 320)	//N73
	{
		edwinHeight += 8;
	}
	return edwinHeight;
}
//////////////////////////////////////////////////////////////////////////
//From MTimerNotifier
//////////////////////////////////////////////////////////////////////////
TBool CRecommendWindow::DoPeriodTask()
{
	if(iTitleBar)
	{
		iTitleBar->SetRightBitmap(iMainEngine.GetBitmapFactory().GetInputIconBmp(iMainEngine.GetInputType()-1),iMainEngine.GetBitmapFactory().GetInputIconBmpMask(iMainEngine.GetInputType()-1));
	}
	return ETrue;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef __SERIES60_3X__
void CRecommendWindow::DisplayPhoneNumbersL(MObjectProvider* aMopParent)
{

RPbkViewResourceFile  phonebookResource( *(CEikonEnv::Static())  );

if ( !phonebookResource.IsOpen())
    {
    phonebookResource.OpenL();
    }

// Add searching array to parameters
CContactDatabase::TContactViewFilter filter(CContactDatabase::EPhonable); 
CPbkMultipleEntryFetchDlg::TParams params;
params.iContactView = &iPbkContactEngine->FilteredContactsViewL( filter ); 

// Launch fetching dialog
CPbkMultipleEntryFetchDlg* fetchDlg = CPbkMultipleEntryFetchDlg::NewL( params, *iPbkContactEngine );
//fetchDlg->SetMopParent( aMopParent );  
TInt okPressed = fetchDlg->ExecuteLD();
CleanupStack::PushL( params.iMarkedEntries );

if ( okPressed )
{
	TInt paramCount = params.iMarkedEntries->Count();
	// Get the selected contacts id array
	for ( TInt i = 0; i < paramCount; ++i )
    {
		const TContactItemId cid = ( *params.iMarkedEntries )[i];
	    
		CPbkContactItem* pbkItem = iPbkContactEngine->ReadContactLC( cid );
		TPtrC tempAddress;
	    
		CPbkPhoneNumberSelect* selectDlg = new (ELeave) CPbkPhoneNumberSelect( );
		tempAddress.Set( selectDlg->ExecuteLD(*pbkItem, pbkItem->DefaultSmsField(), ETrue) );

		if((iNum+i)>4)
		{
			CleanupStack::PopAndDestroy(1);  // pbkItem, infoText
			break;
		}
		iPhoneText.Append(*tempAddress.Alloc());
		iPhoneText.Append(_L(";"));

		CleanupStack::PopAndDestroy(1);  // pbkItem, infoText
	}

	iContentEdit->SetTextL(iPhoneText.Alloc());
}

CleanupStack::PopAndDestroy();  // iMarkedEntries

phonebookResource.Close();	   // Close the phonebook view
}
void CRecommendWindow::CheckPhoneNumber()
{
	iPhoneText.Zero();
	iContentEdit->GetText(iPhoneText);
	if(iPhoneText.Length()>0)
	{
		if(iPhoneText.Right(1).Compare(_L(";"))!=0)
		{
			iPhoneText.Append(_L(";"));
		}
	}

	iNum=0;
	for(TInt i=0;i<iPhoneText.Length();i++)
	{
		if(iPhoneText.Mid(i,1).Compare(_L(";"))==0)
			iNum++;
	}

	if(iNum<5)
		DisplayPhoneNumbersL((MObjectProvider*)iMainEngine.Container());	
	else
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_Commend_CommendFive),*this,ECmdHideEdwin);
}
#endif

