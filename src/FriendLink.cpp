/*
============================================================================
 Name        : FriendLink.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CFriendLink implementation

 待改：EnterModel(...)
============================================================================
*/

#include "FriendLink.h"
#include "HandleSynchronization.h"
#include "CoCoPreDefine.h"

enum TEventCommand
{
	ENULL,
	EStart,
	Edownload,
	EFileGrade,
	EcontentDetial,
	EMainSearch,
	EUpPage,
	EDownPage,
	EEndpage,
	EStartPage,
	EGoPage,
	EOK,
	EUpData
};


CFriendLink::CFriendLink(CWindow* aParent,CMainEngine& aMainEngine):CWindow(EFriendLinkView,aParent,aMainEngine)
,iSyschr(aMainEngine.GetSynchronContent())
{
	this->ilistbox = NULL;
	this->iTitlebar = NULL;
	this->naviPane = NULL;
	this->iDialog = NULL;
	this->iThirdflags = FALSE;
	this->iTitlenumber = 0;
	this->ifriendflags = 0;
	// No implementation required
}


CFriendLink::~CFriendLink()
{
	iSyschr.SetObserver(NULL);
}

CFriendLink* CFriendLink::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CFriendLink* self = new (ELeave)CFriendLink(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CFriendLink* CFriendLink::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CFriendLink* self=CFriendLink::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CFriendLink::ConstructL()
{
	iMainEngine.ControlFactory().SetParentWindow(this);
	ChangeButton();//add by hesanyuan
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
TBool CFriendLink::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = EFalse;
	keyResult = HandleIconKeyL(aKeyCode);
	if(!keyResult)
	{
		keyResult = ETrue;
		switch(aKeyCode)
		{
		case EKeyDevice0:	//左软键,弹出菜单
			this->InitMenu();
			break;
		case EKeyDevice1:	//右软键
			{
				if(iFLinkType==1)
				{
					iFLinkType=0;
					this->ChangListItem(this->naviPane->CurIndex() + 1);
				}
				else
					GoBackToParentWindow();
				break;
			}
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
void CFriendLink::DoDraw(CGraphic& aGraphic)const
{
	
}
void CFriendLink::DoDeactivate()
{

}
void CFriendLink::DoActivateL()
{
	
}
void CFriendLink::CreateTileAndList()
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
void CFriendLink::ChangTitle()
{
	if(this->naviPane)
	{
		TInt allno = iMainEngine.GetSynchronContent().GetCount(EContentHotDownload,-1,-1); 
		this->iTitlenumber = allno;
		TBuf<10>files;
		files.Append(_L("hesan>"));
		files.AppendNum(allno);	
		iMainEngine.WriteLog16(files);		
		this->naviPane->AppendItem(iMainEngine.GetSynchronContent().GetValue(EContentHotDownload,-1,-1));
		this->naviPane->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_FriendLink));
	}
}
TBool CFriendLink::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}
void CFriendLink::DoSizeChanged()
{

}
void CFriendLink::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = this->iMainEngine.GetTextRes();
	SetLeftButtonL(textRes[ETurkeyTextRes_Option]);
	SetRightButtonL(textRes[ETurkeyTextRes_Back]);
}
void CFriendLink::AddListITemL()
{
	if (this->ilistbox)
	{
		this->ilistbox->AppendL(CNormalListBoxItem::NewL(_L("")));
		
		TInt another = iMainEngine.GetSynchronContent().GetCount(EContentHotDownload,0,-1);
		if (another == 0)//没有三级项。
		{
			this->iThirdflags = TRUE;
			return;
		}
		this->ilistbox->Reset();
		for (TInt temp1=0; temp1<another; temp1++)
		{
			ilistbox->AppendL(CNormalListBoxItem::NewL(iMainEngine.GetSynchronContent().GetValue(EContentHotDownload,temp1,-1)));
			TBuf<11>fileqs;
			fileqs.Append(_L("hesan1>"));
			fileqs.AppendNum(temp1);		
			iMainEngine.WriteLog16(fileqs);
		}
	}
}
TBool CFriendLink::HandleIconKeyL(TInt aKeyCode)
{
	return EFalse;

}
void CFriendLink::InitMenu()
{
	

	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Ok),EOK);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Res_UpDate),EUpData);
	return;
	
	//CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);//CreatePopUpMenu(*this);

	//popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Search_SDownLoad),Edownload);
	//popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_FileGrade),EFileGrade);
	//popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_ContentDetail),EcontentDetial);
	////////////////////////////////////////////////////////////////////////////
	////
	//popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MainSearch),EMainSearch);
	//popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_UpPage),EUpPage);
	//popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownPage),EDownPage);
	//popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_EndPage),EEndpage);
	//popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_StartPage),EStartPage);
	//popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Search_GoPage),EGoPage);
	////popUpMenu->InitSubMenu();
	
}
TBool CFriendLink::UpData()
{
	if (iSyschr.IsUpdate(EContentHotDownload,-1,-1))
	{	
		this->InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_SystemUpdating));
		iSyschr.SendSecondRequest(EContentHotDownload);
		//iSyschr.SendSecondRequest(EContentFriendLink);
		iMainEngine.WriteLog16(_L("EContentHotDownload"));
		return TRUE;

	}
	else if(iSyschr.IsUpdate(EContentFriendLink,-1,-1))
	{
		this->InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_SystemUpdating));
		iSyschr.SendSecondRequest(EContentFriendLink);
		iMainEngine.WriteLog16(_L("EContentFriendLink"));
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
TBool CFriendLink::FriendUpData()
{

	if (iSyschr.IsUpdate(EContentFriendLink,-1,-1))
	{	
		this->InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_SystemUpdating));
		iSyschr.SendSecondRequest(EContentFriendLink);
		//iSyschr.SendSecondRequest(EContentFriendLink);
		iMainEngine.WriteLog16(_L("EContentHotDownload"));
		this->ifriendflags = 1;
		return TRUE;
	}
	else
	{
		//RemoveAndDeleteControl(iDialog);
		//_LIT(text,"no need to update");等改
		//iDialog=iMainEngine.ControlFactory().CreateTipDialog(text,*this,12);//这里会有内存泄漏吧. 
		this->ifriendflags = 2;
		iMainEngine.WriteLog16(_L("hesanyuan"));
		return FALSE;
	}
}
void CFriendLink::InitWaitDialog(const TDesC& aText)
{
	iDialog=iMainEngine.ControlFactory().CreateWaitDialog(aText,*this,1,this);
}
void CFriendLink::EnterModel(TInt atype)
{
//	atype += 1;

	TBuf<30>temptitle;
	temptitle.Zero();
	switch(atype)
	{
	case 0:
		{
			temptitle.Append(iMainEngine.GetDesById(ECoCoTextRes_MainView_Three));
			temptitle.Append(_L("->"));
			CNormalListBoxItem& item = (CNormalListBoxItem&)ilistbox->CurItem();

			temptitle.Append(item.GetText());
			CNewDownload* newdowload = CNewDownload::NewL(this,iMainEngine);
			newdowload->titlebuf.Zero();
			newdowload->titlebuf.Copy(temptitle);
			newdowload->iFirstID = EContentHotDownload;
			TInt tempsecondid = ilistbox->CurItemIndex();			
			newdowload->iSecondID = iMainEngine.GetSynchronContent().GetItemID(EContentHotDownload,tempsecondid,-1);
			newdowload->iThirdID = 0;
			newdowload->ConstructLater();
			newdowload->SetViewType(EContentHotDownload);
			this->SetChildWindow(newdowload);
		}
		break;
	case 1:
		{
			if(iFLinkType==0)
			{
				iPIndex=ilistbox->CurItemIndex();
				this->ChangeFriendLinkList(iPIndex);
			}
			else
			{
				TFileName downLoadUrl;
				downLoadUrl.Zero();
				downLoadUrl.Append(_L("http://59.36.96.182/interface/f?n="));
				downLoadUrl.Append(iMainEngine.GetIMEI());
				downLoadUrl.Append(_L("&id="));
				TInt firstid = EContentFriendLink;
				TInt secondid = iPIndex;
				TInt thirdid =this->ilistbox->CurItemIndex();
				TInt Urlid = iMainEngine.GetSynchronContent().GetItemID(firstid,secondid,thirdid);

				downLoadUrl.AppendNum(Urlid);
				downLoadUrl.Append(_L("&t="));
				downLoadUrl.AppendNum(1);
				this->iMainEngine.LaunchWapBrowswerL(downLoadUrl);
			}
		}
	    break;
	default:
	    break;
	}
}
void CFriendLink::NaviPaneIndexChanged(TInt aNewIndex)
{
	iFLinkType=0;
	this->ChangListItem(aNewIndex + 1);
}
void CFriendLink::DoPopUpMenuCommand(TInt aCommand)
{
	switch(aCommand)
	{
	case EStart:
		{
			this->GoBackToParentWindow();
		}
		break;
	case Edownload:
		{
			break;
		}
	case EFileGrade:
		{
			break;
		}
	case EcontentDetial:
		{
			break;
		}

	case EMainSearch:
		{
			break;
		}
	case EUpPage:
		{
			break;
		}
	case EDownPage:
		{
			break;
		}
	case EEndpage:
		{
			break;
		}
	case EStartPage:
		{
			break;
		}
	case EGoPage:
		{
			break;
		}
	case EOK:
		if ((naviPane)&&(ilistbox))
		{
			TInt type = this->naviPane->CurIndex();
			this->EnterModel(type);//add by hesanyuan
		}
		break;
	case EUpData://用于手动更新
		{
			//this->UpData();
			this->InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_SystemUpdating));
			if (this->naviPane->CurIndex()==0)
			{	
				iSyschr.SendSecondRequest(EContentHotDownload);
			}
			else if(this->naviPane->CurIndex()==1)
			{
				iSyschr.SendSecondRequest(EContentFriendLink);
			}
			break;
		}
	default:
		break;
	}
}
void CFriendLink::DialogEvent(TInt aCommand)
{
	if(aCommand==1)
	{
		iSyschr.CancelSendRequest();
	}
}
void CFriendLink::HandleResponseEvent(TInt aEvent,TInt aType)
{
	
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
				this->iFLinkType=0;
				RemoveAndDeleteControl(iDialog);
				iDialog = NULL;
				if (this->ifriendflags == 1)
				{
					this->AddFriendList();
					break;
				}
				this->CreateTileAndList();
				break;
			}
		}
	} 
	else
	{

	}
}
void CFriendLink::AddFriendList()
{
	ilistbox->Reset();
	this->iThirdflags = TRUE;
	TInt friendnumber = iMainEngine.GetSynchronContent().GetCount(EContentFriendLink,0,-1);
	TBuf<10>destst;
	destst.Zero();
	destst.AppendNum(friendnumber);
	destst.Append(_L("&&&"));
	iMainEngine.WriteLog16(destst);
	if (friendnumber == 0)
	{
		return;
	}
	for (TInt i=0;i<friendnumber;i++)
	{
		const TDesC& des = iMainEngine.GetSynchronContent().GetValue(EContentFriendLink,i,-1);		
		ilistbox->AppendL(CNormalListBoxItem::NewL(des));
	}
	ilistbox->Layout();
	return;//这里加入友链的项。
}

//////////////////////////////////////////////////////////////////////////
//还没有:加上联网失败的处产理
//////////////////////////////////////////////////////////////////////////
void CFriendLink::ChangListItem(TInt aNewIndex)
{

	this->iThirdflags = FALSE;
	TInt o = 0;
	if(!this->ilistbox)
	{
		ilistbox = iMainEngine.ControlFactory().CreateDefaultListBoxL();
		
	}
	else
	{
		ilistbox->Reset();
	}
	TInt tem = aNewIndex;
	
	ilistbox->AppendL(CNormalListBoxItem::NewL(_L("")));
	
	iMainEngine.WriteLog16(_L("changlist:2"));

	//没有第二个二级项.
	this->ifriendflags = 0;
	if (aNewIndex == 2)
	{
		if (this->FriendUpData())
		{
			return;
		}
		this->ilistbox->Reset();
		this->iThirdflags = TRUE;
		TInt friendnumber = iMainEngine.GetSynchronContent().GetCount(EContentFriendLink,0,-1);
		TBuf<10>destst;
		destst.Zero();
		destst.AppendNum(friendnumber);
		destst.Append(_L("&&&"));
		iMainEngine.WriteLog16(destst);
		if (friendnumber == 0)
		{
			return;
		}
		for (TInt i=0;i<friendnumber;i++)
		{
			const TDesC& des = iMainEngine.GetSynchronContent().GetValue(EContentFriendLink,i,-1);		
			ilistbox->AppendL(CNormalListBoxItem::NewL(des));
		}
		return;//这里加入友链的项。
	}
	TBuf<10>temptst;
	temptst.Zero();
	temptst.AppendNum(iTitlenumber);
	iMainEngine.WriteLog16(temptst);

	TInt another = iMainEngine.GetSynchronContent().GetCount(EContentHotDownload,0,-1);
	if (another == 0)//没有三级项///
	{
		this->iThirdflags = TRUE;
		return;
	}
	this->ilistbox->Reset();
	for (TInt temp1=0; temp1<another; temp1++)
	{		
		const TDesC& des = iMainEngine.GetSynchronContent().GetValue(EContentHotDownload,temp1,-1);		
		ilistbox->AppendL(CNormalListBoxItem::NewL(des));

	}

	ilistbox->Layout();
}
void CFriendLink::ChangeFriendLinkList(TInt aType)
{
	TInt num = iMainEngine.GetSynchronContent().GetCount(EContentFriendLink,aType,0);

	if (num>0)
	{
		iFLinkType=1;
		ilistbox->Reset();
		for (TInt i=0;i<num;i++)
		{
			const TDesC& des = iMainEngine.GetSynchronContent().GetValue(EContentFriendLink,aType,i);		
			ilistbox->AppendL(CNormalListBoxItem::NewL(des,this->iMainEngine.GetBitmapFactory().GetFileTypeIcon(EFileTypeLink),this->iMainEngine.GetBitmapFactory().GetFileTypeIconMask(EFileTypeLink)));
		}
	}
}
void CFriendLink::ReadXML()
{
	TiXmlDocument doc("c:\\insearch.xml");
	if(!doc.LoadFile())
	{
		TInt t = 0;
		return;
	}
	TiXmlElement *RootElement=doc.FirstChildElement();//根
	if (!RootElement)
	{
		return;
	}
	TiXmlElement* cpnext = RootElement->FirstChildElement("cp");//值.
	if (!cpnext)
	{
		return;
	}
	
//	const char *elementtxt = next->Value();
	TInt Currentcp = atoi( cpnext->FirstChild()->Value() );
	
	TiXmlElement* apnext = RootElement->FirstChildElement("ap");
	if(!apnext)
	{
		return;
	}
	TInt allap = atoi(apnext->FirstChild()->Value());
	
	TiXmlElement* Lnext = RootElement->FirstChildElement("l");

	CDesCArray* iDesArray = NULL;//用于存放所有的值.
	
	iDesArray = new (ELeave) CDesCArrayFlat(10);
	
	while (Lnext)
	{
		//循环读L里的数据。
		
		iDesArray->AppendL(* CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)Lnext->FirstChild()->Value())));
		Lnext=Lnext->NextSiblingElement();
	//	ilistbox->AppendL(* CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)Lnext->FirstChild()->Value())));
		
	}

	//把底下的代码放到一个函数里，就可以了。下面是用来读取到列表的，上面是用来存放数据的。

	TInt allcount = iDesArray->Count();
	if (this->ilistbox)
	{
		this->ilistbox->Reset();
		for(int i=0;i<allcount;i++)
		{
			HBufC* iBuf = NULL;
			
			const TPtrC& ptr = (*iDesArray)[i];//原来他是返回一个TPtrC的值。

			iBuf=HBufC::NewL(ptr.Length());
		
			iBuf->Des().Append(ptr);
			this->ilistbox->AppendL(CNormalListBoxItem::NewL(*iBuf));
			delete iBuf;
			iBuf = NULL;
		}
	}
	TInt itemp = 0;
	delete iDesArray;
	iDesArray = NULL;

}

