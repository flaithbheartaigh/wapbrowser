/*
============================================================================
 Name        : SutraDownload.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSutraDownload implementation
============================================================================
*/

#include "SutraDownload.h"
#include "GradeDialog.h"
#include "HandleGrade.h"
//#include "Control.h"
#include "HandleInfo.h"
#include "SystemInfoDialog.h"
#include "StaticCommonTools.h"
#include "DownLoadEngine.h"
#include "InputDialog.h"
#include "WindowFactory.h"
#include "CacheEngine.h"
#include "HandleSynchronization.h"

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
	EPopNone,
	EOK,
	EUpData,
	EAll,
	EPicture,
	EMusic,
	EVido,
	EGame,
	ETool,
	EBook,
	ETopic
};
CSutraDownload::CSutraDownload(CWindow* aParent,CMainEngine& aMainEngine):CWindow(ESutraDownloadView,aParent,aMainEngine)
,iSyschr(aMainEngine.GetSynchronContent())
{
	iDesArray = NULL;//用于存放所有的值.
	iShowGrade = NULL;
	iDialog = NULL;
	iShowInfo = NULL;
	iInputDialog = NULL;
	iMenuArray = NULL;
	iArrayNo = 0;
	iCurpage = 0;
	iAllpage = 0;//add
	iJumpPage = 1;
	iJumpFlage = 0;//add
	iContentInfo = NULL;
	iCacheEngine = NULL;
	iGradeFlage = FALSE;//add
	itempdes = NULL;//add
	iSearchid = 5;//add

	// No implementation required
}

CSutraDownload::~CSutraDownload()
{
	if (iShowGrade)
	{
		delete iShowGrade;
		iShowGrade = NULL;
	}
	if (iShowInfo)
	{
		delete iShowInfo;
		iShowInfo = NULL;
	}
	if (iDesArray)
	{
		delete iDesArray;
		iDesArray = NULL;
	}
	if (iMenuArray)
	{
		delete iMenuArray;
		iMenuArray = NULL;
	}
	if(iCacheEngine)
	{
		delete iCacheEngine;
		iCacheEngine = NULL;
	}

	iSyschr.SetObserver(NULL);
}

CSutraDownload* CSutraDownload::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CSutraDownload* self = new (ELeave)CSutraDownload(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSutraDownload* CSutraDownload::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CSutraDownload* self=CSutraDownload::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CSutraDownload::ConstructL()
{
	iMainEngine.WriteLog16(_L("CSutraDownload::ConstructL"));
	ChangeButton();
	iMainEngine.ControlFactory().SetParentWindow(this);
	iSyschr.SetObserver(this);
	iDesArray = new (ELeave) CDesCArrayFlat(10);
	if (this->UpData())
	{
		return;
	}
	this->CreateTileAndList();
	iMainEngine.WriteLog16(_L("CSutraDownload::ConstructL End"));
}

TBool CSutraDownload::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = EFalse;
	keyResult = HandleIconKeyL(aKeyCode);
	if(!keyResult)
	{
		keyResult = ETrue;
		switch(aKeyCode)
		{
		case EKeyDevice0:	//左软键,弹出菜单
			InitMenu();
			break;

		case EKeyDevice1:	//右软键,隐藏或退出程序
			//	PrepareExit();
			GoBackToParentWindow();
			//HideApplication();
			break;

		case EKeyDevice3:	//OK键,进入模块		
			{
				TInt type = 0;
				EnterModel(type);
			}
			break;
		case EKeyRightArrow:
			{
				if (iDesArray->Count()>0)
				{
					iflalgs = TRUE;//add
					Changpage(iflalgs);
				}				
				break;
			}
		case EKeyLeftArrow:
			{
				if (iDesArray->Count()>0)
				{
					iflalgs = FALSE;
					Changpage(iflalgs);//add
				}				
				break;
			}
		default:
			keyResult = EFalse;
			break;
		}
	}
	return keyResult;
}
void CSutraDownload::DoDraw(CGraphic& aGraphic)const
{

}
void CSutraDownload::DoDeactivate()
{
	//iTitlebar = NULL;
	//ilistbox=NULL;
	//DeleteAllControl();
}

void CSutraDownload::DoActivateL()
{

	//	naviPane->SetObserver(this);
}

void CSutraDownload::CreateTileAndList()
{
	iMainEngine.WriteLog16(_L("CSutraDownload::CreateTileAndList"));
	if (!iTitlebar)
	{
		if(iMainEngine.GetSynchronContent().GetValue(EContentBestDownload,-1,-1).Length()==0)
		{
			iTitlebar = iMainEngine.ControlFactory().CreateDefaultTitleBarL(iMainEngine.GetDesById(ECoCoTextRes_MainView_One),this);
		}
		else
		{
			iTitlebar = iMainEngine.ControlFactory().CreateDefaultTitleBarL(iMainEngine.GetSynchronContent().GetValue(EContentBestDownload,-1,-1),this);
		}

		ASSERT(NULL == ilistbox);
		ilistbox = iMainEngine.ControlFactory().CreateDefaultListBoxL();
//		_LIT(name,"StraDownload");
		TInt itemp = 0;
		ilistbox->AppendL(CNormalListBoxItem::NewL(_L("")));
		ilistbox->Reset();//add by hesanyuan
		ASSERT(NULL == iCacheEngine);
		TInt firstid = EContentBestDownload;
		TInt secondid = 0;
		TInt thirdid = -1;
		TInt sendid = iMainEngine.GetSynchronContent().GetItemID(firstid,secondid,thirdid);
		iSearchid = sendid;//add

		iCacheEngine = iMainEngine.CreateCacheEngineL(*this,EContentBestDownload,sendid,0);
		iCacheEngine->RequestPageFileName(1);
	}
	iMainEngine.WriteLog16(_L("CSutraDownload::CreateTileAndList End"));
}

TBool CSutraDownload::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}
void CSutraDownload::DoSizeChanged()
{

}
void CSutraDownload::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	SetLeftButtonL(textRes[ETurkeyTextRes_Option]);
	SetRightButtonL(textRes[ETurkeyTextRes_Back]);
}


TBool CSutraDownload::HandleIconKeyL(TInt aKeyCode)
{
	return EFalse;

}
void CSutraDownload::InitMenu()
{
	if (iDesArray->Count()<=0)
	{
		CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Res_UpDate),EUpData);
		return;
	}
	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);//CreatePopUpMenu(*this);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Search_SDownLoad),Edownload);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_FileGrade),EFileGrade);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_ContentDetail),EcontentDetial);
	//////////////////////////////////////////////////////////////////////////
	//
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MainSearch),EMainSearch);
	//submenu
	CSubMenuItem* subItem = new (ELeave) CSubMenuItem(iMainEngine.GetDesById(ECoCoTestRes_LookWithContent));

	TInt allno = iMainEngine.GetSynchronContent().GetCount(EContentBestDownload,0,-1); 
	iMenuArray = new TInt[allno];
	iArrayNo = allno;
	for (TInt temp=0;temp<allno;temp++)
	{	
		iMenuArray[temp] = temp+14;
		subItem->Append(iMainEngine.GetSynchronContent().GetValue(EContentBestDownload,temp,-1),iMenuArray[temp]);
	}
	//subItem->Append(iMainEngine.GetDesById(ETurkeyTextRes_Topic),ETopic);
	popUpMenu->AppendSubMenu(subItem);

	if (iCurpage == 1)
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownPage),EDownPage);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_EndPage),EEndpage);
	}
	else if (iCurpage == iAllpage)
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_StartPage),EStartPage);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_UpPage),EUpPage);
		
	}
	else 
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_StartPage),EStartPage);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_UpPage),EUpPage);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownPage),EDownPage);		
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_EndPage),EEndpage);		
	}
	
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Search_GoPage),EGoPage);

}
void CSutraDownload::EnterModel(TInt atype)
{
	if (iDesArray->Count()>0)
	{
		this->CheckDowndtype();//add
	}
	//上面是新加的内容
}
void CSutraDownload::DoPopUpMenuCommand(TInt aCommand)
{
	if (aCommand<14)
	{
		switch(aCommand)
		{

		case Edownload://下载
			{
				this->CheckDowndtype();
				//iDialog=iMainEngine.ControlFactory().CreateTipDialog(_L("is down?"),*this,3);
				break;
			}
		case EFileGrade://评分
			{
				InitGradeDialog();
				break;
			}
		case EcontentDetial://信息详情
			{
				GetInfo();
				break;
			}
		case EMainSearch://搜索
			{
				Search();
				break;
			}
		case EUpPage://上一页
			{
				if (iDesArray->Count()>0)
				{	
					iflalgs = FALSE;//add
					Changpage(iflalgs);
				}		
				//iCacheEngine->RequestPageFileName(iCurpage);
				break;
			}
		case EDownPage://下一页
			{
				if (iDesArray->Count() > 0)
				{
					iflalgs = TRUE;//add
					Changpage(iflalgs);
				}
				//iCacheEngine->RequestPageFileName(iCurpage);
				break;
			}
		case EEndpage://尾页
			{
				this->Goto(this->iAllpage);
				iJumpFlage = 1;
				iJumpPage = iAllpage;//add
				break;
			}
		case EStartPage://首页
			{
				this->Goto(1);
				iJumpFlage = 1;
				iJumpPage = 1;//add
				break;
			}
		case EGoPage://转至
			{
				//这里填入我们写的值，就可以了
				InputDialog();			
				break;
			}	
		case EOK:
			{
				break;

			}
		case EUpData:
			{//可能需要改动.
				if (iCacheEngine)
				{
					iCurpage = iCurpage <= 0 ? 1 : iCurpage;
					iCacheEngine->RequestPageFileName(iCurpage);//add by hesanyuan
				}
				else
				{
					//UpData();//add
					this->InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_SystemUpdating));
					iSyschr.SendSecondRequest(EContentBestDownload);
				}
				break;
			}
		default:
			break;
		}
	}
	else
	{
		for (TInt temp=0; temp<iArrayNo; temp++)
		{
			if (iMenuArray[temp]==aCommand)
			{
				TInt firstid = EContentBestDownload;
				TInt secondid = aCommand-14;
				TInt thirdid = -1;
				TInt sendid = iMainEngine.GetSynchronContent().GetItemID(firstid,secondid,thirdid);
				if (this->iCacheEngine)
				{
					delete iCacheEngine;
					iCacheEngine = NULL;
				}
				iSearchid = sendid;//add

				iCacheEngine = iMainEngine.CreateCacheEngineL(*this,EContentBestDownload,sendid,0);
				iCacheEngine->RequestPageFileName(1);
				return;
			}

		}

	}
	
}
//
//返回１表示为下载类广告，０表示非下载类广告，２表示下载类文件。
//
TInt CSutraDownload::AdvertisMent()
{//广告。
    TInt index = ilistbox->CurItemIndex();
	HBufC* iBuf = NULL;
	const TPtrC& ptr = (*iDesArray)[index];//原来他是返回一个TPtrC的值。
	iBuf=HBufC::NewL(ptr.Length());
	iBuf->Des().Append(ptr);

	_LIT(KNAMEFLAGE,",");	
	TBuf<40>tempbuf;//名称
	TBuf<10>bitbuf;//大小
	TBuf<10>typebuf;//类型
	TBuf<10>hot;//人气
	TBuf<10>grade;//评分
	TBuf<10>downid;//应用码.
	tempbuf.Zero();
	bitbuf.Zero();
	typebuf.Zero();
	hot.Zero();
	grade.Zero();
	downid.Zero();
	ParseBuf(*iBuf,KNAMEFLAGE,0,tempbuf);
	ParseBuf(*iBuf,KNAMEFLAGE,2,bitbuf);
	ParseBuf(*iBuf,KNAMEFLAGE,1,typebuf);
	ParseBuf(*iBuf,KNAMEFLAGE,3,hot);
	ParseBuf(*iBuf,KNAMEFLAGE,4,grade);
	ParseBuf(*iBuf,KNAMEFLAGE,5,downid);

	hot.TrimRight();
	_LIT(adflage,"A");
	TInt jude = hot.Compare(adflage);
	TInt judegrade = grade.Compare(adflage);
	if (jude == 0)
	{
		//加入到下载里
		TFileName downLoadUrl;
		downLoadUrl.Zero();
		downLoadUrl.Append(_L("http://59.36.96.182/interface/a?t=1&n="));
		downLoadUrl.Append(iMainEngine.GetIMEI());
		downLoadUrl.Append(_L("&id="));
		downLoadUrl.Append(downid);
		
		TInt ifielsize = ::ConvertTDesCToTInt(bitbuf);
		iDownType = AddDownLoadList(*this,13,downLoadUrl,tempbuf,ifielsize,0,0,1);
		//AddDownLoadList(*this,);//在这里传参数就可以了.
		delete iBuf;
		iBuf = NULL;
		return 0;
	}	
	else if (judegrade == 0)
	{//调用打开网页
		//TBuf<3>addrs;
		TFileName downLoadUrl;
		downLoadUrl.Zero();
		downLoadUrl.Append(_L("http://59.36.96.182/interface/a?t=1&n="));
		downLoadUrl.Append(iMainEngine.GetIMEI());
		downLoadUrl.Append(_L("&id="));
		downLoadUrl.Append(downid);
		iMainEngine.LaunchWapBrowswerL(downLoadUrl);
		
		delete iBuf;
		iBuf = NULL;
		return 1;
	}
	else
	{
		delete iBuf;
		iBuf = NULL;

		return 2;
	}
	
}

void CSutraDownload::Search()
{
	TBuf<20>tempBuf;
	tempBuf.Append(iMainEngine.GetDesById(ECoCoTextRes_MainView_One));
	tempBuf.Append(_L("->"));
		tempBuf.Append(iMainEngine.GetDesById(ETurkeyTextRes_MainSearch));//add by hesanyuan
	//CNormalListBoxItem& item = (CNormalListBoxItem&)ilistbox->CurItem();
	//tempBuf.Append(item.GetText());
	TInt index = ilistbox->CurItemIndex();
	HBufC* iBuf = NULL;
	const TPtrC& ptr = (*iDesArray)[index];//原来他是返回一个TPtrC的值。
	iBuf=HBufC::NewL(ptr.Length());
	iBuf->Des().Append(ptr);
	
	_LIT(temppage,"%d");
	TBuf<4>pageId;
	pageId.Zero();
	pageId.Format(temppage,iCurpage);
	TBuf8<200> urlBuf;
	urlBuf.Append(_L8("http://59.36.96.182/coco/psearch?p=1&username="));
	urlBuf.Append(iMainEngine.GetIMEI());
	urlBuf.Append(_L8("&bid="));						//id
	urlBuf.AppendNum(iSearchid);					//add
	urlBuf.Append(_L8("&keyword="));					//关键字.
	//urlBuf.Append(iMainEngine.GetDesById(ETurkeyTextRes_All));

	SetChildWindow(iMainEngine.WindowFactory().CreateSearchResultView(ESearchResultView,this,ilistbox->CurItemIndex(),urlBuf,tempBuf));

	delete iBuf;
	iBuf = NULL;
}
void CSutraDownload::GradeEvent(TInt aEvent,TInt aGrade)
{
	if(aEvent == 0)
	{
		iGrade = aGrade;
		SendMyGrade();
		//这里写处理确定的事件。
	}
}
void CSutraDownload::HandleResponseEvent(TInt aEvent,TInt aType)
{
	if(iDialog)
		RemoveAndDeleteControl(iDialog);
	if (aEvent == EHttpNoError)
	{
		switch(aType)
		{
		case EHttpGrade:
			{
				iPopState=EPopNone;//不知道这个值有什么用。
				InitDialog(iShowGrade->GetInfo());
				_LIT(KGRADEFLAG,"0");//看是否返回0
				const TDesC& tempdes1 = iShowGrade->GetGrade();
				if (tempdes1.Compare(KGRADEFLAG)!=0)//不为零则重新获取。
				{
					iGradeFlage = TRUE;
				}//add
			}
			break;
		case EHttpContentInfoImage:
			{
				if(iContentInfo)
				{
					RemoveAndDeleteControl(iContentInfo);
					iContentInfo=iMainEngine.ControlFactory().CreateContentInfoDialog(*this,iShowInfo->GetName(),iShowInfo->GetSize(),iShowInfo->GetType(),iShowInfo->GetRemark(),iShowInfo->GetImagePath(),12,iShowInfo->IsImage(),this);
				
				}
				break;
			}
		case EHttpContentInfo:
			{			
				InitInfoDialog(iShowInfo->GetName(),iShowInfo->GetSize(),iShowInfo->GetType(),iShowInfo->GetRemark(),12,iShowInfo->IsImage());
				break;
			}
		case EHttpTempBuf:
			{
				if(iShowInfo->IsImage())
				{
					if(iContentInfo)
						RemoveAndDeleteControl(iContentInfo);

					iContentInfo=iMainEngine.ControlFactory().CreateContentInfoDialog(*this,iShowInfo->GetName(),iShowInfo->GetSize(),iShowInfo->GetType(),iShowInfo->GetRemark(),iShowInfo->GetImagePath(),12,iShowInfo->IsImage(),this);
					break;
				}
				else
				{		
					InitInfoDialog(iShowInfo->GetName(),iShowInfo->GetSize(),iShowInfo->GetType(),iShowInfo->GetRemark(),12,iShowInfo->IsImage());
					break;
				}
				break;
			}
		case EHttpSecondContentUpdate:
			{
				this->CreateTileAndList();
				break;
			}
		}		
	}

}
void CSutraDownload::InfoResponseEvent(TInt aEvent)
{
	//这里也可以处理信息详情。
}
void CSutraDownload::DialogEvent(TInt aCommand)
{
	switch(aCommand)
	{
		case -98:
		break;
		case 3:
			DownLoadFile(0);
		break;
		case 1:
			{
				//取消缓存联网
				iSyschr.CancelSendRequest();
			}
			break;
		case 13:
			{
				if ((iDownType == 2)||(iDownType == 5))
				{
					DownLoadFile(1);
				}
			}
		break;
		case 7:
		{
			if (iCurpage == 1)
			{
				iCacheEngine->RequestPageFileName(1);//add
			}
			else
			{
				if (iJumpFlage == 1)
				{
					iCacheEngine->RequestPageFileName(iJumpPage);//add
					iJumpFlage = 0;
				}
				else
				{
					TInt temppage = iCurpage;

					if (iflalgs == FALSE)
					{
						temppage -= 1;//上一页
						if (temppage<=0)
						{
							temppage = 1;
						}
					}
					else
					{
						temppage += 1;
						if (temppage>=iAllpage)
						{
							temppage = iAllpage;
						}
					}
					iCacheEngine->RequestPageFileName(temppage);//add
				}
			}
		}
		break;
		case 8:
			if(0 == iCurpage)		//如果请求的页面为第一页
			{
				GoBackToParentWindow();
			}
		break;
	case 14:
		{
			if (iDialog)
			{
				RemoveAndDeleteControl(iDialog);
				iDialog = NULL;
			}
			iShowGrade->CancelSendRequest();
			//这里是取消评分。
			break;
		}
	case 15:
		{
			if (iDialog)
			{
				RemoveAndDeleteControl(iDialog);
				iDialog = NULL;
			}
			iShowInfo->CancelSendRequest();
			break;
			//这里是取消信息详情。
		}//add
	}
}
void CSutraDownload::Goto(TInt iCurpage)
{
	iCacheEngine->RequestPageFileName(iCurpage);
	//这里跳转
	//this->DrawRightInfo();
}

void CSutraDownload::DrawRightInfo()
{
	TBuf<20>page;
	page.Zero();
	_LIT(tigle,"/");
	page.AppendNum(iCurpage);
	page.Append(tigle);
	page.AppendNum(iAllpage);
	this->iTitlebar->SetRightInfoL(page);
}

void CSutraDownload::Changpage(TBool aflalgs)
{
	TInt temppage = iCurpage;

	if (aflalgs == FALSE)
	{
		temppage -= 1;//上一页
		if (temppage<=0)
		{
			temppage = 1;
		}
	}
	else
	{
		temppage += 1;
		if (temppage>=iAllpage)
		{
			temppage = iAllpage;
		}

	}
	iCacheEngine->RequestPageFileName(temppage);
}
void CSutraDownload::Changdrawpage(TBool aflalgs)
{
	if (aflalgs == FALSE)
	{
		iCurpage -= 1;//上一页
		if (iCurpage<=0)
		{
			iCurpage = 1;
		}
	}
	else
	{
		iCurpage += 1;
		if (iCurpage>=iAllpage)
		{
			iCurpage = iAllpage;
		}

	}
}
void CSutraDownload::CheckDowndtype()
{
	TInt index = this->ilistbox->CurItemIndex();
	HBufC* iBuf = NULL;
	const TPtrC& ptr = (*iDesArray)[index];//原来他是返回一个TPtrC的值。
	iBuf=HBufC::NewL(ptr.Length());
	iBuf->Des().Append(ptr);

	_LIT(KNAMEFLAGE,",");	
	TBuf<20>filetype;
	filetype.Zero();
	ParseBuf(*iBuf,KNAMEFLAGE,1,filetype);
	filetype.TrimRight();
	TInt rValue=::CheckFileType(filetype);
	if(rValue==EFileTypeImage||rValue==EFileTypeAppOrGame)
	{
		this->GetInfo();
	}
	else
	{
		//iDialog=iMainEngine.ControlFactory().CreateTipDialog(_L("is down?"),*this,3);//delete by hesanyuan
        //iDialog = iMainEngine.ControlFactory().CreateInquireDialog(iMainEngine.GetDesById(ETurkeyTextRes_MainDown),*this,3,4);//add by hesanyuan
		DownLoadFile(0);
	}
	//iShowInfo->SendRequest(downid);

	delete iBuf;
	iBuf = NULL;
}

void CSutraDownload::InitGradeDialog()
{
	CGradeDialog* gradeDialog=iMainEngine.ControlFactory().CreateGradeDialog(*this);//评分控件
}

void CSutraDownload::SendMyGrade()
{
	//InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting));
	iDialog=iMainEngine.ControlFactory().CreateWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting),*this,14);//add;
	if(!iShowGrade)
	{
		iShowGrade=CHandleGrade::NewL(*this,iMainEngine);
	}

	TInt index = ilistbox->CurItemIndex();
	HBufC* iBuf = NULL;
	const TPtrC& ptr = (*iDesArray)[index];//原来他是返回一个TPtrC的值。
	iBuf=HBufC::NewL(ptr.Length());
	iBuf->Des().Append(ptr);

	_LIT(KNAMEFLAGE,",");
	TBuf<10>downid;
	downid.Zero();
	ParseBuf(*iBuf,KNAMEFLAGE,5,downid);
	iShowGrade->SendRequest(downid,iGrade);
	delete iBuf;
	iBuf = NULL;

}

void CSutraDownload::InitWaitDialog(const TDesC& aText)
{
	iDialog=iMainEngine.ControlFactory().CreateWaitDialog(aText,*this,1,this);
}

void CSutraDownload::InitDialog(const TDesC& aText)
{
	iDialog=iMainEngine.ControlFactory().CreateTipDialog(aText,*this,2,this);//这里会有内存泄漏吧. 
} 

void CSutraDownload::InitInfoDialog(const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,TInt aCommand,TBool aBool)
{
	if(iContentInfo)
		RemoveAndDeleteControl(iContentInfo);

	iContentInfo=iMainEngine.ControlFactory().CreateContentInfoDialog(*this,aName,aSize, aType,aRemark,aCommand,aBool,this);
	
}
//////////////////////////////////////////////////////////////////////////
//From MCacheObserver
//////////////////////////////////////////////////////////////////////////
void CSutraDownload::CacheUpdate(const TDesC& aFileName)
{

	iMainEngine.WriteLog16(_L("CSutraDownload::CacheUpdate"));
	if(ilistbox)
	{
		iMainEngine.WriteLog16(_L("CSutraDownload::CacheUpdate InitListBox"));
// 		RFs
// 		ASSERT(BaflUtils::FileExists(fs,aFileName));
		//ilistbox->Reset();delete by hesanyuan
		//iDesArray->Reset();
		_LIT(name,"StraDownload");
		TInt itemp = 0;
		//ilistbox->AppendL(CNormalListBoxItem::NewL(name));
		//ilistbox->Reset();//add by hesanyuan
		ReadXML(aFileName);	
		TInt allcount = iDesArray->Count();//add
		if (allcount <= 0)
		{//add
			this->InitDialog(iMainEngine.GetDesById(ETurkeyTextRes_ListNull));
			return;
		}
	}
	iMainEngine.WriteLog16(_L("CSutraDownload::CacheUpdate End"));
}

void CSutraDownload::GetInfo()
{
	iDialog=iMainEngine.ControlFactory().CreateWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting),*this,15);//add;
	if(!iShowInfo)
	{
		iShowInfo=CHandleInfo::NewL(*this,iMainEngine);
	}
	TInt index = this->ilistbox->CurItemIndex();
	HBufC* iBuf = NULL;
	const TPtrC& ptr = (*iDesArray)[index];//原来他是返回一个TPtrC的值。
	iBuf=HBufC::NewL(ptr.Length());
	iBuf->Des().Append(ptr);
	
	_LIT(KNAMEFLAGE,",");	
	TBuf<10>downid;
	downid.Zero();
	ParseBuf(*iBuf,KNAMEFLAGE,5,downid);
	downid.TrimRight();
	iShowInfo->SendRequest(downid);
	
	delete iBuf;
	iBuf = NULL;
//	iShowInfo->SendRequest(_L("153801"));
}
void CSutraDownload::InputDialog()
{
	iInputDialog=iMainEngine.ControlFactory().CreateInputNumberDialog(*this,iMainEngine.GetDesById(ETurkeyTextRes_GoToPage),_L(""),1,this);
}

void CSutraDownload::InputResponseEvent(TInt aEvent,const TDesC& aText)
{
	if(aEvent==1)
	{
		TInt page=0;
		if(::CheckIsTInt(aText)==KErrNone)
		{
			page=::ConvertTDesCToTInt(aText);
			if (page>iAllpage)
			{
				page = iAllpage;
			}
			else if (page<1)
			{
				page = 1;
			}
			
			iCacheEngine->RequestPageFileName(page);
			iJumpFlage = 1;
			iJumpPage = page;//add
		}
	}
}
void CSutraDownload::ShowList()//用来在列表中显示数据.
{
	TInt i = 0;
	TInt allcount = iDesArray->Count();
	if (ilistbox)
	{	
		if (allcount <= 0)
		{
			InitDialog(iMainEngine.GetDesById(ETurkeyTextRes_ListNull));
			return;
		}
		
		_LIT(KNAMEFLAGE,",");	

		for(int i=0;i<allcount;i++)
		{
			HBufC* iBuf = NULL;
			const TPtrC& ptr = (*iDesArray)[i];//原来他是返回一个TPtrC的值。
			iBuf=HBufC::NewL(ptr.Length());
			iBuf->Des().Append(ptr);
			//文件名，		文件后缀名，文件大小，人气	评分	应用码	详情
			//爱你一万年,	gif,		34,			2,	0,		150890,	0		
			TBuf<40>tempbuf;
			TBuf<10>bitbuf;
			TBuf<20>typebuf;
			TBuf<20>hot;//人气
			TBuf<10>grade;//评分
			tempbuf.Zero();
			bitbuf.Zero();
			typebuf.Zero();
			hot.Zero();
			grade.Zero();
			typebuf.Append(iMainEngine.GetDesById(ETurkeyTextRes_FileType));
			bitbuf.Append(iMainEngine.GetDesById(ETurkeyTextRes_FileSize));
			hot.Append(iMainEngine.GetDesById(ETurkeyTextRes_FilePeople));
			grade.Append(iMainEngine.GetDesById(ETurkeyTextRes_FileGrade));
			ParseBuf(*iBuf,KNAMEFLAGE,0,tempbuf);
			ParseBuf(*iBuf,KNAMEFLAGE,2,bitbuf);
			ParseBuf(*iBuf,KNAMEFLAGE,1,typebuf);
			ParseBuf(*iBuf,KNAMEFLAGE,3,hot);
			ParseBuf(*iBuf,KNAMEFLAGE,4,grade);
			
			TBuf<10>mapbuf;
			mapbuf.Zero();
			ParseBuf(*iBuf,KNAMEFLAGE,1,mapbuf);
			mapbuf.TrimRight();
			CBitmapFactory& bitmapFactory = iMainEngine.GetBitmapFactory();
			TInt fileTypeicon = CheckFileType(mapbuf);
			iIconBitmap = bitmapFactory.GetFileTypeIcon(fileTypeicon);
			iIconBitmapMask = bitmapFactory.GetFileTypeIconMask(fileTypeicon);
			bitbuf.Append(_L(" K"));
			typebuf.Append(bitbuf);
			hot.Append(grade);
			ilistbox->AppendL(CCurThreeListBoxItem::NewL(tempbuf,typebuf,hot,iIconBitmap,iIconBitmapMask));
			
			//	ilistbox->AppendL(CCurThreeListBoxItem::NewL(tempbuf,typebuf,bitbuf));

			delete iBuf;
			iBuf = NULL;
		}
		/*
		if (iJumpFlage == 1)
				{
					iCurpage = iJumpPage;
					iJumpFlage = 0;
				}
				else
				{
					Changdrawpage(iflalgs);	
				}			//add*/
		
		this->DrawRightInfo();
	}
}

//void CSutraDownload::ReadXML()
void CSutraDownload::ReadXML(const TDesC& aFileName)
{
	HBufC8* fileName = UtilityTools::ConvertToHBufC8L(aFileName);
	CleanupStack::PushL(fileName);
	//TiXmlDocument* doc = new TiXmlDocument((const char*)(fileName->Des().PtrZ()));

	//TiXmlDocument doc("c:\\insearch.xml");
	TiXmlDocument doc((const char*)(fileName->Des().PtrZ()));
	CleanupStack::PopAndDestroy();
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
	if (!cpnext->FirstChild())
	{
		return;//add;
	}
	iCurpage = atoi( cpnext->FirstChild()->Value() );//得到当前页数.	

	TiXmlElement* apnext = RootElement->FirstChildElement("ap");
	if(!apnext)
 	{
 		return;
 	}
	TiXmlNode* tempnode = apnext->FirstChild();
	if (!tempnode)
	{
		return;
	}
	iAllpage = atoi(apnext->FirstChild()->Value());	//得到总的页数
	 
	iDesArray->Reset();
	ilistbox->AppendL(CNormalListBoxItem::NewL(_L("")));
	ilistbox->Reset();//add by hesanyuan
	this->DrawRightInfo();//add

	TiXmlElement* Lnext = RootElement->FirstChildElement("l");
	while (Lnext)
	{

		//循环读L里的数据。
		TiXmlNode* nodechlild = Lnext->FirstChild();
		if (!nodechlild)
		{
			return;
		}
		iDesArray->AppendL(* CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)Lnext->FirstChild()->Value())));
		Lnext=Lnext->NextSiblingElement();
		//	ilistbox->AppendL(* CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)Lnext->FirstChild()->Value())));

	}
	ShowList();
}
void CSutraDownload::GetDownLoadInfo(TDes& aUrl,TDes& aFileName,TInt iFileSize)
{

	//得到列表里的内容。
	//TInt index=ilistbox->CurItemIndex(); 
	//CNormalListBoxItem& item = (CNormalListBoxItem&)ilistbox->CurItem();
	//HBufC* buf=HBufC::NewL((item.GetText()).Length());//得到当前长度。
	//buf->Des().Copy(item.GetText() );//上面三句我改动过

	TInt index = ilistbox->CurItemIndex();
	HBufC* buf = NULL;
	const TPtrC& ptr = (*iDesArray)[index];//原来他是返回一个TPtrC的值。
	buf=HBufC::NewL(ptr.Length());
	buf->Des().Append(ptr);

	TInt tPo=buf->Find(_L(","));
	if(tPo<0)
		return;

	//aUrl.Append(_L("http://59.36.96.182/3gserver/download?flag=1&username="));
	aUrl.Append(_L("http://59.36.96.182/coco/d?flag=1&username="));
	aUrl.Append(iMainEngine.GetIMEI());
	aUrl.Append(_L("&fileid="));

	aFileName.Append(buf->Left(tPo));
	aFileName.Append(_L("."));//??
	buf->Des().Delete(0,tPo+1);

	tPo=buf->Find(_L(","));
	aFileName.Append(buf->Left(tPo));
	buf->Des().Delete(0,tPo+1);

	tPo=buf->Find(_L(","));
	iFileSize=::ConvertTDesCToTInt(buf->Left(tPo));
	buf->Des().Delete(0,tPo+1);

	tPo=buf->Find(_L(","));
	buf->Des().Delete(0,tPo+1);

	tPo=buf->Find(_L(","));
	buf->Des().Delete(0,tPo+1);

	tPo=buf->Find(_L(","));
	aUrl.Append(buf->Left(tPo));

	iMainEngine.WriteLog16(aUrl);

	buf->Des().Delete(0,tPo+1);
	
	delete buf;
	buf = NULL;
}
void CSutraDownload::DownLoadFile(TInt aDownType)
{

	TInt fileSize=0;
	TFileName downLoadUrl;
	TFileName fileName;
	
	TInt flage = AdvertisMent();
	if (flage == 2)
	{
		GetDownLoadInfo(downLoadUrl,fileName,fileSize);

		iMainEngine.WriteLog16(downLoadUrl);
		iMainEngine.WriteLog16(fileName);

		TBuf<10> tm;
		tm.AppendNum(fileSize);
		iMainEngine.WriteLog16(tm);

		//AddDownLoadList(*this,2,downLoadUrl,fileName,fileSize,0,0,1);//delete by hesanyuan
		iDownType = AddDownLoadList(*this,13,downLoadUrl,fileName,fileSize,0,aDownType,1);
	}
	else
	{
		return;
	}
	
	//iMainEngine.GetDownLoadEngine().AddTaskToDownLoadThread();
}
TBool CSutraDownload::UpData()
{
	iMainEngine.WriteLog16(_L("CSutraDownload::UpData"));
	TBool isUpdate = iSyschr.IsUpdate(EContentBestDownload,-1,-1);
	if (isUpdate)
	{
		this->InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_SystemUpdating));
		iSyschr.SendSecondRequest(EContentBestDownload);
		iMainEngine.WriteLog16(_L("EContentBestDownload"));
	}
	else
	{
		//RemoveAndDeleteControl(iDialog);
		//_LIT(text,"no need to update");等改
		//iDialog=iMainEngine.ControlFactory().CreateTipDialog(text,*this,12);//这里会有内存泄漏吧. 
		iMainEngine.WriteLog16(_L("hesanyuan"));
	}
	iMainEngine.WriteLog16(_L("CSutraDownload::UpData End"));
	return isUpdate;
}
void CSutraDownload::ContentInfoEvent(TInt aLeftCommond)
{
	iContentInfo=NULL;
	this->iShowInfo->CancelSendRequest();
	if(aLeftCommond==12)
	{
		DownLoadFile(0);
	}
}
void CSutraDownload::CacheFailed()
{
	iMainEngine.ControlFactory().CreateInquireDialog(iMainEngine.GetDesById(ECoCoTestRes_NetError_IsRetry),*this,7,8);
}//add
