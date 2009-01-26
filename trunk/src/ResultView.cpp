/*
============================================================================
 Name        : ResultView.cpp
 Author      : hsany
 Version     :
 Copyright   : Your copyright notice
 Description : CResultView implementation
============================================================================
*/

#include "ResultView.h"
#include "TypeDefine.h" 
#include "CoCoPreDefine.h"

#include "HandleSynchronization.h"
//HandleSynchronization
#include "NewDownload.h"

enum TEventCommand
{
	ENULL,	
	EOK,
	EFriend,
	EUpData
};
CResultView::CResultView(CWindow* aParent,CMainEngine& aMainEngine):CWindow(EResultWindow,aParent,aMainEngine)
,iSyschr(aMainEngine.GetSynchronContent())
{
	this->naviPane = NULL;
	this->iDialog = NULL;
	this->ilistbox = NULL;
	this->panitem = -1;
	iTitlenumber = 0;
	//this->iSyschr = iMainEngine.GetSynchronContent();
	// No implementation required
}

CResultView::~CResultView()
{
	//if (this->ilistbox)
	//{
	//	delete this->ilistbox;
	//	this->ilistbox = NULL;
	//}
	iSyschr.SetObserver(NULL);
}

CResultView* CResultView::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CResultView* self = new (ELeave)CResultView(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CResultView* CResultView::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CResultView* self=CResultView::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CResultView::ConstructL()
{
	ChangeButton();
	//
	iMainEngine.ControlFactory().SetParentWindow(this);
	iSyschr.SetObserver(this);
	if (this->UpData())
	{
		return;
	}
	else
	{
		this->CreateTileAndList();
	}
}
void CResultView::DoActivateL()
{


}
void CResultView::CreateTileAndList()
{
	naviPane = iMainEngine.ControlFactory().CreateDefaultNaviPane();
	//naviPane->AppendItem(_L("myload"));	
	naviPane->SetObserver(this);
	if (!this->ilistbox)
	{
		this->ilistbox = iMainEngine.ControlFactory().CreateDefaultListBoxL();
	}
	//this->UpData();
	this->ChangTitle();
	this->AddListITemL();
}
void CResultView::DoDeactivate()
{
	TInt temp = 0;
}
void CResultView::DoSizeChanged()
{
	
}
void CResultView::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = this->iMainEngine.GetTextRes();
	SetLeftButtonL(textRes[ETurkeyTextRes_Option]);
	SetRightButtonL(textRes[ETurkeyTextRes_Back]);

}

void CResultView::ChangTitle()
{
	/*if (this->UpData())
	{
		return;
	}*/
	if(this->naviPane)
	{	
		
		TInt allno = iMainEngine.GetSynchronContent().GetCount(EContentTypeDownload,0,-1); 
		iTitlenumber = allno;
		TBuf<10>files;
		files.Append(_L("hesan>"));
		files.AppendNum(allno);		
		iMainEngine.WriteLog16(files);
		for (TInt temp=0; temp<allno; temp++)
		{
			this->naviPane->AppendItem(iMainEngine.GetSynchronContent().GetValue(EContentTypeDownload,temp,-1));
			//ilistbox->AppendL(CNormalListBoxItem::NewL(iMainEngine.GetSynchronContent().GetValue(EContentTypeDownload,temp,-1)));
			TBuf<11>fileqs;
			fileqs.Append(_L("hesan>"));
			fileqs.AppendNum(temp);		
			iMainEngine.WriteLog16(fileqs);
		}
//		this->naviPane->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_FriendLink));

		//ilistbox->AppendL(CNormalListBoxItem::NewL(iMainEngine.GetSynchronContent().GetValue(EContentTypeDownload,temp,-1)));
		
	}
}

void CResultView::DoPopUpMenuCommand(TInt aCommand)
{
	switch(aCommand)
	{
	case EOK:
		{
			if ((naviPane)&&(ilistbox))
			{
				TInt type = this->naviPane->CurIndex();
				this->EnterModel(type);
			}
			
			//这里是菜单的“确定”
		}
		break;
	case EFriend:
		{
			//“友情链接”
		}
		break;
	case EUpData:
		{
			//this->UpData();
			this->InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_SystemUpdating));
			iSyschr.SendSecondRequest(EContentTypeDownload);
		/*	_LIT(waite,"please wait");
			this->InitWaitDialog(waite);*/
			
			//if (iSyschr.IsUpdate(EContentTypeDownload,1,-1))
			//{
			//	iSyschr.SendSecondRequest(EContentTypeDownload);
			//	iMainEngine.WriteLog16(_L("EContentTypeDownload"));

			//}//iMainEngine.GetSynchronContent().GetValue()
			//iMainEngine.WriteLog16(_L("hesanyuan"));
			//更新。
		}
		break;
	case 4:
		{
		}
		break;
	default:
		break;
	}
}
void CResultView::DialogEvent(TInt aCommand)
{
	switch(aCommand)
	{
	case 1:
		iSyschr.CancelSendRequest();
		break;
	case 2:
		break;

	case 3:
		break;
	case 10://这里是点确定
		{
			this->UpData();
			break;
		}
		
	case 11://这里是取消.
		{
			GoBackToParentWindow();
			break;
		}
		

	}
}
void CResultView::InitWaitDialog(const TDesC& aText)
{
	iDialog=iMainEngine.ControlFactory().CreateWaitDialog(aText,*this,1,this);
}
TBool CResultView::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = EFalse;
	keyResult = HandleIconKeyL(aKeyCode);
	if(!keyResult)
	{
		keyResult = ETrue;
		switch(aKeyCode)
		{
		case EKeyDevice0:	//左软键,弹出菜单
			this->InitMenu(this->panitem);
			break;

		case EKeyDevice1:	//右软键,隐藏或退出程序
			//	PrepareExit();
			GoBackToParentWindow();
			//HideApplication();
			break;

		case EKeyDevice3:	//OK键,进入模块		
			{
				TInt type = this->naviPane->CurIndex();
				this->EnterModel(type);
			}
			break;
		default:
			keyResult = EFalse;
			break;
		}
	}
	return keyResult;
}
TBool CResultView::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}
void CResultView::DoDraw(CGraphic& aGraphic) const
{

}
void CResultView::AddListITemL()
{
	if (this->ilistbox)
	{
		_LIT(name,"1");
		this->ilistbox->AppendL(CNormalListBoxItem::NewL(name));
		this->ilistbox->Reset();
		TInt another = iMainEngine.GetSynchronContent().GetCount(EContentTypeDownload,0,0);
		for (TInt temp1=0; temp1<another; temp1++)
		{
			ilistbox->AppendL(CNormalListBoxItem::NewL(iMainEngine.GetSynchronContent().GetValue(EContentTypeDownload,0,temp1)));
			TBuf<11>fileqs;
			fileqs.Append(_L("hesan1>"));
			fileqs.AppendNum(temp1);		
			iMainEngine.WriteLog16(fileqs);
		}
	}
}
TBool CResultView::HandleIconKeyL(TInt aKeyCode)
{
	TInt iCurIndex = 0;
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case EKeyUpArrow:
		//iCurIndex -= iIconNumPerLine;
		break;

	case EKeyDownArrow:
		//iCurIndex += iIconNumPerLine;
		break;

	case EKeyLeftArrow:
		if(iCurIndex > 0)
		{
			iCurIndex--;
		}
		else
		{
			iCurIndex = 0;
		}
		break;

	case EKeyRightArrow:		
		break;

	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}
void CResultView::EnterModel(TInt aType)
{
	temptitle.Zero();
	const TDesC& des = iMainEngine.GetSynchronContent().GetValue(EContentTypeDownload,aType,-1);
	temptitle.Append(des);
	temptitle.Append(_L("->"));
	CNormalListBoxItem& item = (CNormalListBoxItem&)ilistbox->CurItem();

	temptitle.Append(item.GetText());	
	CNewDownload* newdowload = CNewDownload::NewL(this,iMainEngine);
	newdowload->titlebuf.Zero();
	newdowload->titlebuf.Copy(temptitle);
	TInt temp = 0;
	newdowload->iFirstID = EContentTypeDownload;
	newdowload->iSecondID = iMainEngine.GetSynchronContent().GetItemID(EContentTypeDownload,aType,-1);
	TInt listindex = this->ilistbox->CurItemIndex();
	newdowload->iThirdID = iMainEngine.GetSynchronContent().GetItemID(EContentTypeDownload,aType,listindex);
	TBuf<10>sendid;//delete later;
	sendid.Zero();
	sendid.AppendNum(newdowload->iSecondID);
	sendid.Append(_L("->"));
	sendid.AppendNum(newdowload->iThirdID);
	iMainEngine.WriteLog16(sendid);
	newdowload->ConstructLater();
	newdowload->SetViewType(EContentTypeDownload);
	this->SetChildWindow(newdowload);
	
}
void CResultView::InitMenu(TInt atype)
{
	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);//CreatePopUpMenu(*this);
//	popUpMenu->AppendItem(_L("EResultStart"),EStart);
	switch(atype)
	{	
	case 1:
		{
			popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Ok),EOK);
			popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Res_UpDate),EUpData);
			break;
		}
		
	case 8:
		{
			popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Ok),EOK);
			popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_FriendLink),EFriend);
			break;
		}
	    
	default:
		{
			popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Ok),EOK);
			popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Res_UpDate),EUpData);
			break;
		}
	    break;
	}
	
	return;
}
TBool CResultView::UpData()
{
	
	if (iSyschr.IsUpdate(EContentTypeDownload,-1,-1))
	{		
		this->InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_SystemUpdating));
		iSyschr.SendSecondRequest(EContentTypeDownload);
		iMainEngine.WriteLog16(_L("EContentTypeDownload"));
		return TRUE;

	}
	else
	{
		//RemoveAndDeleteControl(iDialog);
		//_LIT(text,"no need to update");等改
		//iDialog=iMainEngine.ControlFactory().CreateTipDialog(text,*this,12);//这里会有内存泄漏吧. 
		iMainEngine.WriteLog16(_L("hesanyuan"));
		return FALSE;
	}

}
void CResultView::NaviPaneIndexChanged(TInt aNewIndex)
{
	TInt t=0;
	this->ChangListItem(aNewIndex + 1);
	this->panitem = aNewIndex;
	//InitListBox(aNewIndex+1);
}

void CResultView::ChangListItem(TInt aItem)
{
	TInt o = 0;
	if(!this->ilistbox)
	{
		ilistbox = iMainEngine.ControlFactory().CreateDefaultListBoxL();
	}
	else
	{
		ilistbox->Reset();
	}
	TInt tem = aItem - 1;
	
	_LIT(name2,"2");
	ilistbox->AppendL(CNormalListBoxItem::NewL(name2));
	
	this->ilistbox->Reset();
	iMainEngine.WriteLog16(_L("changlist:2"));
	
	TInt another = iMainEngine.GetSynchronContent().GetCount(EContentTypeDownload,tem,0);
	for (TInt temp1=0; temp1<another; temp1++)
	{		
		const TDesC& des = iMainEngine.GetSynchronContent().GetValue(EContentTypeDownload,tem,temp1);		
		ilistbox->AppendL(CNormalListBoxItem::NewL(des));
		
	}
	TBuf<10>destst;
	destst.Zero();
	destst.AppendNum(another);
	iMainEngine.WriteLog16(destst);
	ilistbox->Layout();
}

//这里是处理返回同步的信息。HandleResponseEvent
void CResultView::HandleResponseEvent(TInt aEvent,TInt aType)
{
	RemoveAndDeleteControl(iDialog);
	iMainEngine.WriteLog16(_L("HandleResponseEvent"));
	if(aEvent==EHttpNoError)
	{
		switch(aType)
		{
		case EHttpLogin:
			//HandleLoginState();
			break;
		case EHttpFirstContentUpdate:
		//	HandleSystemInfo();
			break;
		case EHttpSystemInfo:
			break;
		//	HandleSystemInfo();
		case EHttpSecondContentUpdate:
			{	
				this->CreateTileAndList();
				
			}
			break;
		}
	}
	else
	{
		iDialog = iMainEngine.ControlFactory().CreateInquireDialog(iMainEngine.GetDesById(ETurkeyTextRes_Search_NetErrorReturn),*this,10,11,this);
		iMainEngine.WriteLog16(_L("UpdateElse"));
		//iStatueFlag=ELoginNone;
	}
}