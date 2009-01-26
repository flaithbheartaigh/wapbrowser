/*
============================================================================
 Name        : SearchResultView.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSearchResultView implementation
============================================================================
*/

#include "SearchResultView.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "TitleBar.h"
#include "PopUpMenu.h"
#include "ListBox.h"
#include "TypeDefine.h"
#include "ScreenLayout.h"
#include "SearchListBox.h"
#include "ContentInfoDialog.h"
#include "HandleInSearch.h"

#include "InputDialog.h"
#include "HandleInfo.h"
#include "HandleGrade.h"

#include "HttpManager.h"
#include <EscapeUtils.h>
#include <utf.h>
#include "Dialog.h"
#include "HttpObserver.h"
#include "StaticCommonTools.h"

#include "FileExecuter.h"
#include "DownLoadEngine.h"

#include "GradeDialog.h"

#include "CoCoPreDefine.h"
#include "BitmapFactory.h"

//定义执行命令用的枚举常量
enum TEventPopCommand
{
	ESearchResultPopInfo,
	ESearchResultPopGrade,
	ESearchResultPopBack,
	ESearchResultPopLastPage,
	ESearchResultPopHomePage,
	ESearchResultPopPrePage,
	ESearchResultPopNextPage,
	ESearchResultPopOk,
	ESearchResultPopGoto,
	ESearchResultPopDownLoad,
	ESearchResultMainWindow,
	ESearchResultPopNone
};
enum TSearchResultDialogCommand
{
	ESRDialogContentInfoWait=100,
	ESRDialogContentListWait,
	ESRDialogContentGradeWait,
	ESRDialogShowInfo,
	ESRDialogAddDownLoadInfo,
	ESRDialogDownLoad,
	ESRDialogContentInfo
};

_LIT(KADVISEURL,"http://59.36.96.182/interface/a?t=1&n=%S&id=%S");

CSearchResultView::CSearchResultView(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex)
:CWindow(ESearchResultView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
,iIndex(aIndex)
,iInSearch(NULL)
,iShowInfo(NULL)
,iShowGrade(NULL)
,iUrl(NULL)
,iTempBuf(NULL)
,iCurPage(1)
,iAllPage(0)
{
	// No implementation required
}


CSearchResultView::~CSearchResultView()
{
	DELETE(iTitle);
	DELETE(iInSearch);
	DELETE(iShowInfo);
	DELETE(iShowGrade);
	DELETE(iUrl);
	DELETE(iTempBuf);	
}

CSearchResultView* CSearchResultView::NewLC(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex,const TDesC8& aURL,const TDesC& aTitle)
{
	CSearchResultView* self = new (ELeave)CSearchResultView(aParent, aMainEngine,aIndex);
	CleanupStack::PushL(self);
	self->ConstructL(aURL,aTitle);
	return self;
}

CSearchResultView* CSearchResultView::NewL(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex,const TDesC8& aURL,const TDesC& aTitle)
{
	CSearchResultView* self=CSearchResultView::NewLC(aParent, aMainEngine,aIndex,aURL,aTitle);
	CleanupStack::Pop(); // self;
	return self;
}

void CSearchResultView::ConstructL(const TDesC8& aURL,const TDesC& aTitle)
{
	iMainEngine.ControlFactory().SetParentWindow(this);
	ChangeButton();
	iTitle=HBufC::NewL(aTitle.Length());
	iTitle->Des().Copy(aTitle);

	iInSearch=CHandleInSearch::NewL(*this,iMainEngine);
	iInSearch->SetSendUrl(aURL);
}

void CSearchResultView::DialogEvent(TInt aCommand)
{
	switch(aCommand)
	{
		case -98:
			break;
		case ESRDialogContentInfoWait:
			{
				if(iShowInfo)
				{
					iShowInfo->CancelSendRequest();
				}
			}
			break;
		case ESRDialogContentListWait:
			{
				if(iInSearch)
				{
					iInSearch->CancelSendRequest();
					this->iAllPage=iInSearch->GetAllPage();
					this->iCurPage=iInSearch->GetCurPage();
				}
			}
			break;
		case ESRDialogContentGradeWait:
			{
				if(iShowGrade)
				{
					iShowGrade->CancelSendRequest();
				}
			}
			break;
		case ESRDialogAddDownLoadInfo:
			{
				if(iDownLoadState==5||iDownLoadState==2)
				{
					DownLoadFile(1);
				}
			}
			break;
		case ESRDialogDownLoad:
			DownLoadFile(0);
			break;
	}
	iSearchList->SetEdwinVisible(ETrue);
}
void CSearchResultView::ContentInfoEvent(TInt aLeftCommond)
{
	iContentInfo=NULL;
	iShowInfo->CancelSendRequest();
	//DELETE(iShowInfo);

	if(aLeftCommond==ESRDialogContentInfo)
	{
		//this->InitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadAddToList),ESRDialogDownLoad);
		iSearchList->SetEdwinVisible(EFalse);
		DownLoadFile(0);
	}
	iSearchList->SetEdwinVisible(ETrue);
}
void CSearchResultView::InitInfoDialog(const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,TInt aCommand,TBool aBool)
{
	if(iContentInfo)
		RemoveAndDeleteControl(iContentInfo);

	iContentInfo=iMainEngine.ControlFactory().CreateContentInfoDialog(*this,aName,aSize, aType,aRemark,aCommand,aBool,this);
	iSearchList->SetEdwinVisible(EFalse);
}
void CSearchResultView::InitWaitDialog(const TDesC& aText,TInt aCommand)
{
	iDialog=iMainEngine.ControlFactory().CreateWaitDialog(aText,*this,aCommand,this);
	iSearchList->SetEdwinVisible(EFalse);
}
void CSearchResultView::InitDialog(const TDesC& aText,TInt aCommand,TInt aRCommand)
{
	if(aRCommand!=-99)
	{
		iDialog=iMainEngine.ControlFactory().CreateInquireDialog(aText,*this,aCommand,aRCommand,this);
	}
	else
	{
		iDialog=iMainEngine.ControlFactory().CreateTipDialog(aText,*this,aCommand,this);
	}
	iSearchList->SetEdwinVisible(EFalse);
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CSearchResultView::DoActivateL()
{
	if(iTitleBar&&iAllPage>0&&iCurPage>0)
	{
		TBuf<50> titleBuf;
		titleBuf.Zero();
		titleBuf.Append(*iTitle);
		titleBuf.Append(_L("["));
		titleBuf.AppendNum(iCurPage);
		titleBuf.Append(_L("/"));
		titleBuf.AppendNum(iAllPage);
		titleBuf.Append(_L("]"));
		iTitleBar->SetTextL(titleBuf);
	}
	else
	{
		iTitleBar = iMainEngine.ControlFactory().CreateDefaultTitleBarL(*iTitle,this);
	}
	InitListBox();
}

void CSearchResultView::DoDeactivate()
{
	//DeleteAllControl();
}

void CSearchResultView::DoDraw(CGraphic& aGraphic) const
{
	
}

TBool CSearchResultView::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KKeyCodeUp:
	case KKeyCodeDown:
		iSearchList->KeyEventL(aKeyCode);
		break;
	case KKeyCodeLeft:
		if(iCurPage>1)
		{
			iCurPage--;
			StartInSearch(iKeyWord);
		}
		break;
	case KKeyCodeRight:
		if(iCurPage<iAllPage)
		{
			iCurPage++;
			StartInSearch(iKeyWord);
		}
		break;
	case KLeftSoftKey:
		InitPopUpMenu();
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

TBool CSearchResultView::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CSearchResultView::DoSizeChanged()
{

}

void CSearchResultView::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Option));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

//////////////////////////////////////////////////////////////////////////
//From MPopUpMenuObserver
//////////////////////////////////////////////////////////////////////////
void CSearchResultView::DoPopUpMenuCommand(TInt aCommand)
{
	iPopState=aCommand;
	switch(aCommand)
	{
	case ESearchResultPopInfo:
		GetInfo();
		break;
	case ESearchResultPopGrade:
		InitGradeDialog();
		break;
	case ESearchResultMainWindow:
		GoBackToMainWindow();
		break;
	case ESearchResultPopBack:
		GoBackToParentWindow();
		break;
	case ESearchResultPopLastPage:
		iCurPage=iAllPage;
		StartInSearch(iKeyWord);
		break;
	case ESearchResultPopHomePage:
		iCurPage=1;
		StartInSearch(iKeyWord);
		break;
	case ESearchResultPopPrePage:
		if(iCurPage>1)
		{
			iCurPage--;
			StartInSearch(iKeyWord);
		}
		break;
	case ESearchResultPopNextPage:
		if(iCurPage<iAllPage)
		{
			iCurPage++;
			StartInSearch(iKeyWord);
		}
		break;
	case ESearchResultPopOk:
		{
			iCurPage=1;
			iAllPage=0;
			iKeyWord.Zero();
			iSearchList->GetText(iKeyWord);
			GetTextSearch(iKeyWord);
		}
		break;
	case ESearchResultPopDownLoad:
		this->CheckDownLoadFile();
		break;
	case ESearchResultPopGoto:
		InitInputDialog();
		break;
	default:
		ASSERT(EFalse);
		break;
	}
}
void CSearchResultView::GetTextSearch(const TDesC& aText)
{
	if(aText.Length()>0)
	{
		StartInSearch(aText);
	}
	else
	{
		InitDialog(iMainEngine.GetDesById(ETurkeyTextRes_InputNull),ESRDialogShowInfo);
	}
}
//////////////////////////////////////////////////////////////////////////
//From MHandleEventObserver
//////////////////////////////////////////////////////////////////////////
void CSearchResultView::HandleResponseEvent(TInt aEvent,TInt aType)
{
	if(aEvent==EHttpNoError)
	{
		iSearchList->SetEdwinVisible(ETrue);

		if(iDialog)
			RemoveAndDeleteControl(iDialog);

		if(aType==EHttpTempBuf)
		{
			if(iShowInfo->IsImage()&&iContentInfo)
			{
				RemoveAndDeleteControl(iContentInfo);
				iContentInfo=iMainEngine.ControlFactory().CreateContentInfoDialog(*this,iShowInfo->GetName(),iShowInfo->GetSize(),iShowInfo->GetType(),iShowInfo->GetRemark(),iShowInfo->GetImagePath(),ESRDialogContentInfo,iShowInfo->IsImage(),this);
				iSearchList->SetEdwinVisible(EFalse);
			}
			else
			{
				iPopState=ESearchResultPopNone;
				InitInfoDialog(iShowInfo->GetName(),iShowInfo->GetSize(),iShowInfo->GetType(),iShowInfo->GetRemark(),ESRDialogContentInfo,iShowInfo->IsImage());
			}
		}
		else if(aType==EHttpContentInfoImage)
		{
			if(iContentInfo)
			{
				RemoveAndDeleteControl(iContentInfo);
				iContentInfo=iMainEngine.ControlFactory().CreateContentInfoDialog(*this,iShowInfo->GetName(),iShowInfo->GetSize(),iShowInfo->GetType(),iShowInfo->GetRemark(),iShowInfo->GetImagePath(),ESRDialogContentInfo,iShowInfo->IsImage(),this);
				iSearchList->SetEdwinVisible(EFalse);
			}
		}
		else if(aType==EHttpContentInfo)
		{
			iPopState=ESearchResultPopNone;
			InitInfoDialog(iShowInfo->GetName(),iShowInfo->GetSize(),iShowInfo->GetType(),iShowInfo->GetRemark(),ESRDialogContentInfo,iShowInfo->IsImage());
		}
		else if(aType==EHttpGrade)
		{
			iPopState=ESearchResultPopNone;
			InitDialog(iShowGrade->GetInfo(),ESRDialogShowInfo);
		}
		else if(aType==EHttpInSearch)
		{
			iSearchList->InitListBox();
		
			if(iInSearch->GetCount()>0)
			{
				iAllPage=iInSearch->GetAllPage();
				iCurPage=iInSearch->GetCurPage();
				TBuf<50> titleBuf;
				titleBuf.Zero();
				titleBuf.Append(*iTitle);
				titleBuf.Append(_L("["));
				titleBuf.AppendNum(iCurPage);
				titleBuf.Append(_L("/"));
				titleBuf.AppendNum(iAllPage);
				titleBuf.Append(_L("]"));
				iTitleBar->SetTextL(titleBuf);

				//性感混血MM,jpg,16,2,0,153519,0
				TBuf<100> oneBuf;
				TBuf<50> twoBuf;
				TBuf<50> threeBuf;
				TBuf<10> typeBuf;

				for(TInt i=0;i<iInSearch->GetCount();i++)
				{
					oneBuf.Zero();
					twoBuf.Zero();
					threeBuf.Zero();
					typeBuf.Zero();

					//性感混血MM,jpg,16,2,0,153519,0

					HBufC* buf=HBufC::NewLC(iInSearch->GetOneItem(i).Length());
					buf->Des().Copy(iInSearch->GetOneItem(i));

					::ParseBuf(*buf,_L(","),0,oneBuf);

					twoBuf.Append(iMainEngine.GetDesById(ETurkeyTextRes_FileType));
					::ParseBuf(*buf,_L(","),1,twoBuf);
					twoBuf.Append(_L("  "));
					twoBuf.Append(iMainEngine.GetDesById(ETurkeyTextRes_FileSize));
					::ParseBuf(*buf,_L(","),2,twoBuf);
					twoBuf.Append(_L("K"));

					threeBuf.Append(iMainEngine.GetDesById(ETurkeyTextRes_FilePeople));
					::ParseBuf(*buf,_L(","),3,threeBuf);
					threeBuf.Append(_L("  "));
					threeBuf.Append(iMainEngine.GetDesById(ETurkeyTextRes_FileGrade));
					::ParseBuf(*buf,_L(","),4,threeBuf);

					::ParseBuf(*buf,_L(","),1,typeBuf);

					const CFbsBitmap* icon = NULL;
					const CFbsBitmap* iconMask = NULL;
					icon = iMainEngine.GetBitmapFactory().GetFileTypeIcon(::CheckFileType(typeBuf));
					iconMask = iMainEngine.GetBitmapFactory().GetFileTypeIconMask(::CheckFileType(typeBuf));

					iSearchList->AppendListItem(oneBuf,twoBuf,threeBuf,icon,iconMask);
					CleanupStack::PopAndDestroy(buf);
				}
				iSearchList->ListBoxDraw();
			}
			else
			{
				this->InitDialog(iMainEngine.GetDesById(ETurkeyTextRes_ListNull),ESRDialogShowInfo);
				iTitleBar->SetTextL(*iTitle);
			}
		}
	}
	else
	{
		iSearchList->SetEdwinVisible(ETrue);

		if(aType==EHttpContentInfo)
		{
			if(iDialog)
				RemoveAndDeleteControl(iDialog);

			iPopState=ESearchResultPopNone;
			InitDialog(iShowInfo->GetInfo(),ESRDialogShowInfo);
		}
		else if(aType==EHttpGrade)
		{
			if(iDialog)
				RemoveAndDeleteControl(iDialog);

			iPopState=ESearchResultPopNone;
			InitDialog(iShowGrade->GetInfo(),ESRDialogShowInfo);
		}
		else if(aType==EHttpInSearch)
		{
			if(iDialog)
				RemoveAndDeleteControl(iDialog);

			InitDialog(iInSearch->GetInfo(),ESRDialogShowInfo);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CSearchResultView::InitPopUpMenu()
{

	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);

	if(iSearchList->GetEdwinVisible())
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Ok),ESearchResultPopOk);
	}
	else if(iSearchList->GetListCount()>0)
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownloadURL),ESearchResultPopDownLoad);
	}
	if(iAllPage>0)
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_ContentDetail),ESearchResultPopInfo);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_FileGrade),ESearchResultPopGrade);
	}
	if(iCurPage>1)
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_StartPage),ESearchResultPopHomePage);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_UpPage),ESearchResultPopPrePage);
	}
	if(iCurPage<iAllPage)
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownPage),ESearchResultPopNextPage);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_EndPage),ESearchResultPopLastPage);
	}
	if(iAllPage>0)
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_GoToPage),ESearchResultPopGoto);
	}
	popUpMenu->AppendItem(iMainEngine.GetDesById(ECoCoTextRes_PopUpMenu_GoBackMainView),ESearchResultMainWindow);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Back),ESearchResultPopBack);
}

void CSearchResultView::InitListBox()
{
	if(!iSearchList)
	{
		iSearchList = iMainEngine.ControlFactory().CreateSearchListBox(*this);
		iSearchList->InitListBox();
		iSearchList->ListBoxDraw();
	}
}
void CSearchResultView::SearchListResponseEvent(TInt aEvent,const TDesC& aText)
{
	if(aEvent==0)
	{
		iCurPage=1;
		iAllPage=0;
		iKeyWord.Zero();
		iKeyWord.Append(aText);
		GetTextSearch(iKeyWord);
	}
	else if(aEvent==999)
	{
		this->CheckDownLoadFile();
	}
}

void CSearchResultView::StartInSearch(const TDesC& aText)
{
	InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting),ESRDialogContentListWait);
	iInSearch->SendRequest(GetUrlEncode(aText),iIndex,iCurPage);
}

const TDesC8& CSearchResultView::GetUrlEncode(const TDesC& aText)
{
	if(iTempBuf)
	{
		delete iTempBuf;
		iTempBuf=NULL;
	}

	TBuf8<20> tBuf;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(tBuf,aText);
	iTempBuf=EscapeUtils::EscapeEncodeL(tBuf,EscapeUtils::EEscapeUrlEncoded);
	return *iTempBuf;
}

void CSearchResultView::GetInfo()
{
	InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting),ESRDialogContentInfoWait);
	if(!iShowInfo)
	{
		iShowInfo=CHandleInfo::NewL(*this,iMainEngine);
	}
	//性感混血MM,jpg,16,2,0,153519,0
	TInt index=iSearchList->GetSelectedIndex();

	TBuf<15> code;
	::ParseBuf(iInSearch->GetOneItem(index),_L(","),5,code);
	iShowInfo->SendRequest(code);
}
void CSearchResultView::SentGrade()
{
	InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting),ESRDialogContentGradeWait);
	if(!iShowGrade)
	{
		iShowGrade=CHandleGrade::NewL(*this,iMainEngine);
	}
	
	//性感混血MM,jpg,16,2,0,153519,0
	TInt index=iSearchList->GetSelectedIndex();

	TBuf<15> code;
	::ParseBuf(iInSearch->GetOneItem(index),_L(","),5,code);

	//iShowInfo->SendRequest(code);//delete by hesanyuan
	iShowGrade->SendRequest(code,iGrade);
} 
void CSearchResultView::InitInputDialog()
{
	iInputDialog=iMainEngine.ControlFactory().CreateInputNumberDialog(*this,iMainEngine.GetDesById(ETurkeyTextRes_GoToPage),_L(""),1,this);
	iSearchList->SetEdwinVisible(EFalse);
}
void CSearchResultView::InputResponseEvent(TInt aEvent,const TDesC& aText)
{
	if(aEvent==1)
	{
		TInt page=0;
		if(::CheckIsTInt(aText)==KErrNone)
		{
			page=::ConvertTDesCToTInt(aText);
			if(page<iAllPage)
			{
				if(page==0)
					iCurPage=1;
				else
					iCurPage=page;
			}
			else
			{
				iCurPage=iAllPage;
			}
			StartInSearch(iKeyWord);
		}
	}
	iSearchList->SetEdwinVisible(ETrue);
}

TInt CSearchResultView::GetDownLoadInfo(TDes& aUrl,TDes& aFileName)
{

	TInt fileSize=0;
	TInt index=iSearchList->GetSelectedIndex();

	HBufC* buf=HBufC::NewLC(iInSearch->GetOneItem(index).Length());

	buf->Des().Copy(iInSearch->GetOneItem(index));

	TInt tPo=buf->Find(_L(","));
	if(tPo<0)
		return fileSize;

	//性感混血MM,jpg,16,2,0,153519,0
	TBuf<50> bufTemp;

	bufTemp.Zero();
	::ParseBuf(*buf,_L(","),0,bufTemp);
	aFileName.Append(bufTemp);

	bufTemp.Zero();
	::ParseBuf(*buf,_L(","),1,bufTemp);
	aFileName.Append(_L("."));
	aFileName.Append(bufTemp);


	bufTemp.Zero();
	::ParseBuf(*buf,_L(","),2,bufTemp);
	fileSize=::ConvertTDesCToTInt(bufTemp);

	aUrl.Append(_L("http://59.36.96.182/coco/d?flag=1&username="));
	aUrl.Append(iMainEngine.GetIMEI());
	aUrl.Append(_L("&fileid="));

	bufTemp.Zero();
	::ParseBuf(*buf,_L(","),5,bufTemp);
	aUrl.Append(bufTemp);

	CleanupStack::PopAndDestroy(buf);
	return fileSize;
}

void CSearchResultView::DownLoadFile(TInt aType)
{
	
	TFileName downLoadUrl;
	TFileName fileName;

	TInt fileSize=GetDownLoadInfo(downLoadUrl,fileName);

	iDownLoadState=	AddDownLoadList(*this,ESRDialogAddDownLoadInfo,downLoadUrl,fileName,fileSize,0,aType,1);
}

void CSearchResultView::GradeEvent(TInt aEvent,TInt aGrade)
{
	if(aEvent==0)
	{
		iGrade=aGrade;
		SentGrade();
	}
	iSearchList->SetEdwinVisible(ETrue);
}

void CSearchResultView::InitGradeDialog()
{
	CGradeDialog* gradeDialog=iMainEngine.ControlFactory().CreateGradeDialog(*this);
	iSearchList->SetEdwinVisible(EFalse);
}
void CSearchResultView::CheckDownLoadFile()
{
	TBuf<50> bufTemp;
	TInt index=iSearchList->GetSelectedIndex();
	bufTemp.Zero();

	//判断是会否打开wap广告
	::ParseBuf(iInSearch->GetOneItem(index),_L(","),3,bufTemp);
	if(bufTemp.Compare(_L("A"))==0)
	{
		bufTemp.Zero();
		::ParseBuf(iInSearch->GetOneItem(index),_L(","),5,bufTemp);
		TBuf<300> url;
		url.Format(KADVISEURL,&iMainEngine.GetIMEI(),&bufTemp);
		iMainEngine.LaunchWapBrowswerL(url);
		return;
	}

	bufTemp.Zero();
	//判断是否下载广告
	::ParseBuf(iInSearch->GetOneItem(index),_L(","),4,bufTemp);
	if(bufTemp.Compare(_L("A"))==0)
	{
		bufTemp.Zero();
		::ParseBuf(iInSearch->GetOneItem(index),_L(","),5,bufTemp);
		TBuf<300> url;
		url.Format(KADVISEURL,&iMainEngine.GetIMEI(),&bufTemp);

		TBuf<50> fileName;
		::ParseBuf(iInSearch->GetOneItem(index),_L(","),0,fileName);
		TBuf<50> fileSize;
		::ParseBuf(iInSearch->GetOneItem(index),_L(","),2,fileSize);

		iDownLoadState=	AddDownLoadList(*this,ESRDialogAddDownLoadInfo,url,fileName,::ConvertTDesCToTInt(fileSize),0,1,1);
		return;
	}

	bufTemp.Zero();
	::ParseBuf(iInSearch->GetOneItem(index),_L(","),1,bufTemp);
	TInt rValue=::CheckFileType(bufTemp);
	//图片、工具、游戏
	if(rValue==EFileTypeImage||rValue==EFileTypeAppOrGame)
	{
		this->GetInfo();
	}
	else
	{
		DownLoadFile(0);
		//this->InitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadAddToList),ESRDialogDownLoad);
		iSearchList->SetEdwinVisible(EFalse);
	}
}