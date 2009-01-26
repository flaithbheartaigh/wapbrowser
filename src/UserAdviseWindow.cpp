/*
============================================================================
 Name        : UserAdvise.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CUserAdviseWindow implementation
============================================================================
*/
#include <eikgted.h>

#include "UserAdviseWindow.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "EditorsManager.h"
#include "HttpManager.h"
#include "ScreenLayout.h"
#include "MultiText.h"
#include "Dialog.h"
#include "ScrollBar.h"
#include "tinyxml.h"
#include "PreDefine.h"
#include "TypeDefine.h"
#include "CoCoPreDefine.h"
#include "BitmapFactory.h"
#include "cocobmp.h"
#include "TitleBar.h"
//_LIT8(KAdviseUri,		"http://59.36.96.159:8080/interface/idea");
_LIT8(KAdviseUri,		"http://59.36.96.182/interface/idea");

enum TEventCmd
{
	ECmdOK,
	ECmdGoBackToParentWindow,
	ECmdGoBackToMainWindow,

	ECmdWaitingResponse,
	ECmdHideEdwin,
	ECmdGoback,
};
//////////////////////////////////////////////////////////////////////////
//CUserAdviseReply
//////////////////////////////////////////////////////////////////////////
CUserAdviseReply::CUserAdviseReply()
{

}

CUserAdviseReply::~CUserAdviseReply()
{
	UtilityTools::DeleteFile(KTmpXmlFile);
	delete iInfo;
}

TInt CUserAdviseReply::ParseXML(const TDesC& aDes)
{
	UtilityTools::DeleteFile(KTmpXmlFile);
	UtilityTools::WriteTestFileL(KTmpXmlFile,aDes);

	TiXmlDocument* doc = new TiXmlDocument(TmpXmlFile);

	if(!doc->LoadFile())
	{
		delete doc;
		return 0;
	}

	TiXmlElement* root = doc->RootElement();


	TiXmlNode* childEle = root->FirstChildElement("resultcode")->FirstChild();

	if(childEle)
	{
		iResult = atoi( childEle->Value() );
	}

	childEle = root->FirstChildElement("info")->FirstChild();

	if(childEle)
	{
		const char* info = childEle->Value();
		iInfo = CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)info));
		if(!iInfo)
		{
			iInfo = TPtrC(KNullDesC).AllocL();
		}
	}
	//测试
	//UtilityTools::DeleteFile(KTmpXmlFile);

	delete doc;	
	return 1;

}
//////////////////////////////////////////////////////////////////////////
//CUserAdviseWindow
//////////////////////////////////////////////////////////////////////////
CUserAdviseWindow::CUserAdviseWindow(CWindow* aParent,CMainEngine& aMainEngine) : CWindow(EUserAdviseWindow,aParent,aMainEngine)
																				, iEditorsManager(aMainEngine.EditorsManager())
{
}


CUserAdviseWindow::~CUserAdviseWindow()
{
}

CUserAdviseWindow* CUserAdviseWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CUserAdviseWindow* self = new (ELeave)CUserAdviseWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CUserAdviseWindow* CUserAdviseWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CUserAdviseWindow* self=CUserAdviseWindow::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CUserAdviseWindow::ConstructL()
{
	ChangeButton();
}
//////////////////////////////////////////////////////////////////////////
//From CWindow 
//////////////////////////////////////////////////////////////////////////
void CUserAdviseWindow::DoActivateL()
{	
	InitByScreenSize();
	InitMultiText();
	InitEditor();
	iTitleBar=iMainEngine.ControlFactory().CreateDefaultTitleBarL(iMainEngine.GetDesById(ECoCoTextRes_MainView_MenuAdvice),this);

	iTimer = CNotifyTimer::NewL(*this);
 	iTimer->Start(100000);
}

void CUserAdviseWindow::DoDeactivate()
{
	DeleteAllControl();
	iEditorsManager.SetCurEdwin(NULL);
	iEditorsManager.RemoveEdwinFromArray(iContentEdit);
	iContentEdit = NULL;
	delete iTimer;
	iTimer = NULL;
}

void CUserAdviseWindow::DoDraw(CGraphic& aGraphic)const
{
	DrawInputType(aGraphic);
}

TBool CUserAdviseWindow::DoKeyEventL(TInt aKeyCode) 
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KLeftSoftKey:
	case KOKKey:
		SendAdviseL();
		break;

	case KRightSoftKey:
		GoBackToParentWindow();
		break;

	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;

}
TBool CUserAdviseWindow::DoHandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}
void CUserAdviseWindow::DoSizeChanged()
{
}

void CUserAdviseWindow::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
}
//////////////////////////////////////////////////////////////////////////
//From MOperationObserver
//////////////////////////////////////////////////////////////////////////
void CUserAdviseWindow::OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType)
{
#ifdef _DEBUG
	UtilityTools::WriteTestFileL(_L("C:\\TempXml.Xml"),aEventData);
#endif
	TBool removeResult = RemoveAndDeleteControl(iDialog);
	ASSERT(removeResult);
	iDialog = NULL;
	if(aEventType==0)
	{
		if(aEventData.Length() > 0)
		{
			CUserAdviseReply reply;
			reply.ParseXML(aEventData);
			if(reply.iResult == 0)			//上传失败,显示失败原因
			{
				if(reply.iInfo)
				{
					iMainEngine.ControlFactory().CreateTipDialog(*reply.iInfo,*this,ECmdHideEdwin,this);
				}
				else
				{
					iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ECoCoTestRes_UnKownError),*this,ECmdHideEdwin,this);
				}
			}
			else
			{
				if(reply.iInfo)
				{
					iMainEngine.ControlFactory().CreateTipDialog(*reply.iInfo,*this,ECmdGoback,this);
				}
				else
				{
					iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_OperationOk),*this,ECmdGoback,this);
				}
			}
		}
		else
		{
			iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ECoCoTestRes_UnKownContentError),*this,ECmdHideEdwin,this);
		}
	}
	else
	{
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ECoCoTestRes_UnKownError),*this,ECmdHideEdwin,this);
	}

	iMainEngine.RequestDraw();
}

//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CUserAdviseWindow::DialogEvent(TInt aCommand)
{
	switch(aCommand)
	{
	case ECmdWaitingResponse:
		iMainEngine.GetHttpManager()->CancelTransaction(this,0);
		iEditorsManager.ShowAllEdwins();
		iRequest = EFalse;
		break;

	case ECmdHideEdwin:
		iEditorsManager.ShowAllEdwins();
		break;

	case ECmdGoback:
		GoBackToParentWindow();
		break;

	default:
		ASSERT(EFalse);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CUserAdviseWindow::InitMultiText()
{
	CMultiText* multiText = iMainEngine.ControlFactory().CreateDefaultMutliText();

	multiText->AppendL(iMainEngine.GetDesById(ECoCoTestRes_UserAdviseInfo));
	multiText->Scrollbar()->SetShow(EFalse);
}

void CUserAdviseWindow::InitEditor()
{
	const CScreenLayout& screenLayout = iMainEngine.ScreenLayout();

	TRect rect = screenLayout.WhiteBackgroundRect();
	TInt innerMargin = screenLayout.InnerMargin();
	TRect editorRect = rect;
	editorRect.Shrink(innerMargin,innerMargin);
	editorRect.iTl.iY += iLineHeight;
	editorRect.iBr.iY = editorRect.iTl.iY + iEditorHeight;

	iContentEdit = iEditorsManager.CreateGlbEdwin(editorRect,50);
	iEditorsManager.SetCurEdwin(iContentEdit);
}

HBufC8* CUserAdviseWindow::MakeBodyL(const TDesC8& aMsg,const TDesC8& aUserID)
{
	iMainEngine.WriteLog16(_L("CUserAdviseWindow::MakeBodyL"));
	TInt length = 200 + aMsg.Length() + aUserID.Length();
	HBufC8* buf = HBufC8::NewL(length);
	TPtr8 ptr = buf->Des();
	ptr.Append(KDivision);
	ptr.Append(KCarriageReturn);
	ptr.Append(aMsg);
	ptr.Append(KCarriageReturn);
	ptr.Append(KDivision);
	ptr.Append(KCarriageReturn);
	ptr.Append(aUserID);
	ptr.Append(KCarriageReturn);
	ptr.Append(KDivision);

	iMainEngine.WriteLog16(_L("CUserAdviseWindow::MakeBodyL End"));
	return buf;
}

void CUserAdviseWindow::SendAdviseL()
{
	if(iRequest)return;

	iMainEngine.WriteLog16(_L("CUserAdviseWindow::SendAdviseL"));
	
	HBufC* msgBody = iContentEdit->GetTextInHBufL();
	if(msgBody == NULL)
	{
		//ShowDialog((*iDesArray)[6],CDialog::EDialogTip);
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ECoCoTestRes_UserAdviseTip),*this,ECmdHideEdwin,this);
		iEditorsManager.HideAllEdwins();
		return;
	}
	CleanupStack::PushL(msgBody);
	HBufC8* msgBody8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*msgBody);
	CleanupStack::PushL(msgBody8);
	HBufC8* userName = UtilityTools::ConvertToHBufC8L(iMainEngine.GetIMEI());
	CleanupStack::PushL(userName);

	HBufC8* msg = MakeBodyL(*msgBody8,*userName);
	//iMainEngine.WriteLog8(*msg);
	CleanupStack::PopAndDestroy(3);	//msgBody,msgBody8,userName

	CleanupStack::PushL(msg);

	iMainEngine.GetHttpManager()->SendPostRequest(*this,KAdviseUri,KContentType,*msg);

	iDialog = iMainEngine.ControlFactory().CreateWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting),*this,ECmdWaitingResponse,this);
	iEditorsManager.HideAllEdwins();

	iRequest = ETrue;
	CleanupStack::PopAndDestroy();	//msg

	iMainEngine.WriteLog16(_L("CUserAdviseWindow::SendAdviseL End"));
}

void CUserAdviseWindow::DrawInputType(CGraphic& gc)const
{

}

void CUserAdviseWindow::InitByScreenSize()
{
	const CScreenLayout& screenLayout = iMainEngine.ScreenLayout();
	TSize screenSize = screenLayout.ScreenSize();
	iLineHeight = screenLayout.LineHeight();

	if(screenSize.iWidth == 176&& screenSize.iHeight == 208)		//3250
	{
		iEditorHeight = 88;
	}
	else if(screenSize.iWidth == 352&& screenSize.iHeight == 416)	//N80
	{
		iEditorHeight = 172;
	}
	else if(screenSize.iWidth == 240&& screenSize.iHeight == 320)	//N73
	{
		iEditorHeight = 157;
	}
}
//////////////////////////////////////////////////////////////////////////
//From MTimerNotifier
//////////////////////////////////////////////////////////////////////////
TBool CUserAdviseWindow::DoPeriodTask()
{
	if(iTitleBar)
	{
		iTitleBar->SetRightBitmap(iMainEngine.GetBitmapFactory().GetInputIconBmp(iMainEngine.GetInputType()-1),iMainEngine.GetBitmapFactory().GetInputIconBmpMask(iMainEngine.GetInputType()-1));
	}
	return ETrue;
}