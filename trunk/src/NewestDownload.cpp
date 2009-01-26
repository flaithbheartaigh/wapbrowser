/*
============================================================================
 Name        : NewestDownload.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice 
 Description : NewestDownload implementation
============================================================================
*/ 

#include "NewestDownload.h"
#include "GradeDialog.h"
#include "HandleGrade.h"
#include "ListBoxItem.h"
#include "StaticCommonTools.h"
#include "HandleSynchronization.h"
#include "CacheEngine.h"
#include "HandleInfo.h"
#include "SystemInfoDialog.h"
#include "ControlFactory.h"
#include "WindowFactory.h"
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
NewestDownload::NewestDownload(CWindow* aParent,CMainEngine& aMainEngine):CWindow(ENewestdownView,aParent,aMainEngine)
,iSyschr(aMainEngine.GetSynchronContent())
{
	iflalgs = FALSE;
	this->iDialog = NULL;
	this->ilistbox = NULL;
	this->iTitlebar = NULL;
	this->iShowGrade = NULL;
	this->iInputDialog = NULL;
	iDesArray = NULL;//用于存放所有的值.
	iInputid = 0;
	this->iCacheEngine = NULL;
	this->iMenuArray = NULL;
	iArrayNo = 0;
	iShowInfo = NULL;
	iJumpPage = 1;//add
	iJumpFlage = 0;
	iCurpage = 0;
	iAllpage = 0;//add
	iContentInfo = NULL;
	iSearchid = 5;//全部.
	iGradeFlage= FALSE;//add
	itempdes = NULL;//add
	// No implementation required
}


NewestDownload::~NewestDownload()
{
	if (this->iShowGrade)
	{
		delete this->iShowGrade;
		this->iShowGrade = NULL;
	}
	if (this->iDesArray)
	{
		delete iDesArray;
		iDesArray = NULL;
	}
	if (iMenuArray)
	{
		delete iMenuArray;
		iMenuArray = NULL;
	}
	if (iCacheEngine)
	{
		delete iCacheEngine;
		iCacheEngine = NULL;
	}
	iSyschr.SetObserver(NULL);
}

NewestDownload* NewestDownload::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	NewestDownload* self = new (ELeave)NewestDownload(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

NewestDownload* NewestDownload::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	NewestDownload* self=NewestDownload::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void NewestDownload::ConstructL()
{
	ChangeButton();
	iMainEngine.ControlFactory().SetParentWindow(this);
	iSyschr.SetObserver(this);

	iDesArray = new (ELeave) CDesCArrayFlat(10);
	if (this->UpData())
	{
		return;
	}
	this->CreateTileAndList();	 
}
TBool NewestDownload::DoKeyEventL(TInt aKeyCode)
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

		case EKeyDevice1:	//右软键,隐藏或退出程序
			{
				GoBackToParentWindow();
                break;
			}
		case EKeyDevice3:	//OK键,进入模块
			{
				TInt type = 0;
				if (iDesArray->Count()>0)
				{
					//return;//add by hesanyan
					this->CheckDowndtype();//add by hesanyan
				}
				
				//iDialog=iMainEngine.ControlFactory().CreateTipDialog(_L("is down?"),*this,3);
			}
			break;
		case EKeyRightArrow:
			{
				if (iDesArray->Count()>0)
				{
					iflalgs = TRUE;
					Changpage(iflalgs);
				}
				break;
			}
		case EKeyLeftArrow:
			{
				if (iDesArray->Count()>0)//add
				{
					iflalgs = FALSE;
					Changpage(iflalgs);
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
void NewestDownload::DoDraw(CGraphic& aGraphic)const
{

}
void NewestDownload::DoDeactivate()
{

}
void NewestDownload::DoActivateL()
{
 
}
void NewestDownload::CreateTileAndList()
{
	if (!this->iTitlebar)
	{
		if(iMainEngine.GetSynchronContent().GetValue(EContentNewDownload,-1,-1).Length()==0)
		{
			iTitlebar = iMainEngine.ControlFactory().CreateDefaultTitleBarL(iMainEngine.GetDesById(ECoCoTextRes_MainView_Two),this);
		}
		else
		{
			iTitlebar = iMainEngine.ControlFactory().CreateDefaultTitleBarL(iMainEngine.GetSynchronContent().GetValue(EContentNewDownload,-1,-1),this);
		}
	}
	//	naviPane->SetObserver(this);
	if (!this->ilistbox)
	{
		this->ilistbox = iMainEngine.ControlFactory().CreateDefaultListBoxL();
		ilistbox->AppendL(CNormalListBoxItem::NewL(_L("")));
		this->ilistbox->Reset();//add by hesanyuan
		ASSERT(NULL == iCacheEngine);
		TInt firstid = EContentNewDownload;
		TInt secondid = 0;
		TInt thirdid = -1;
		TInt sendid = iMainEngine.GetSynchronContent().GetItemID(firstid,secondid,thirdid);
		iSearchid = sendid;
		iCacheEngine = iMainEngine.CreateCacheEngineL(*this,EContentNewDownload,sendid,0);
		iCacheEngine->RequestPageFileName(1);
	}
	//CreateListBox(aRect);
	//this->AddListITemL();
}

TBool NewestDownload::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}
void NewestDownload::DoSizeChanged()
{

}
void NewestDownload::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = this->iMainEngine.GetTextRes();
	SetLeftButtonL(textRes[ETurkeyTextRes_Option]);
	SetRightButtonL(textRes[ETurkeyTextRes_Back]);
}
void NewestDownload::AddListITemL()
{
	//if (this->ilistbox)
	//{
	//	_LIT(name,"StraDownload");
	//	this->ilistbox->AppendL(CNormalListBoxItem::NewL(name));
	//	this->ReadXML();
	//	//this->iInputid = iMainEngine.GetSynchronContent().GetItemID(EContentTypeDownload,aInput,-1);
	//}
}
TBool NewestDownload::HandleIconKeyL(TInt aKeyCode)
{
	return EFalse;

}
void NewestDownload::InitMenu()
{
	if (iDesArray->Count()<=0)
	{
		CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);
	//	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Ok),EOK);//delete by hesanyuan
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

	CSubMenuItem* subItem = new (ELeave) CSubMenuItem(iMainEngine.GetDesById(ECoCoTestRes_LookWithContent));

	TInt allno = iMainEngine.GetSynchronContent().GetCount(EContentNewDownload,0,-1); 
	iMenuArray = new TInt[allno];
	iArrayNo = allno;
	for (TInt temp=0;temp<allno;temp++)
	{
		iMenuArray[temp] = temp+13;
		subItem->Append(iMainEngine.GetSynchronContent().GetValue(EContentNewDownload,temp,-1),iMenuArray[temp]);
	}
	/*subItem->Append(iMainEngine.GetDesById(ETurkeyTextRes_Topic),ETopic);*/
	
	popUpMenu->AppendSubMenu(subItem);

	if (this->iCurpage == 1)
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownPage),EDownPage);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_EndPage),EEndpage);
	}
	else if (this->iCurpage == iAllpage)
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
void NewestDownload::EnterModel(TInt atype)
{
	TBuf<30>temptitle;
	temptitle.Zero();
	ASSERT(EFalse);	//请修改
	//temptitle.Append(ilistbox->GetCurItemDes());
	//this->CreateChildWindow(EnewDownloadView);
	CNewDownload* newdowload = CNewDownload::NewL(this,iMainEngine);
	newdowload->titlebuf.Zero();
	newdowload->titlebuf.Copy(temptitle);
	this->SetChildWindow(newdowload);
}
void NewestDownload::DoPopUpMenuCommand(TInt aCommand)
{
	if (aCommand<13)
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
				CheckDowndtype();
				//iDialog=iMainEngine.ControlFactory().CreateTipDialog(_L("is down?"),*this,3);
				break;
			}
		case EFileGrade://评分
			{
				this->InitGradeDialog();
				break;
			}
		case EcontentDetial:
			{
				this->GetInfo();
				break;
			}

		case EMainSearch:
			{
				this->Search();
				break;
			}
		case EUpPage://上一页
			{
				if (iDesArray->Count()>0)
				{
					iflalgs = FALSE;
					this->Changpage(iflalgs);
				}
				break;
			}
		case EDownPage://下一页
			{
				if (iDesArray->Count()>0)
				{
					iflalgs = TRUE;
					this->Changpage(iflalgs);
				}
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
				this->InputDialog();
				break;
			}
		case EOK://确定.
			{
				break;
			}
		case EUpData://手动更新
			{
				if (iCacheEngine)
				{
					iCurpage = iCurpage <= 0 ? 1 : iCurpage;
					iCacheEngine->RequestPageFileName(iCurpage);//add by hesanyuan
				}
				else
				{
					//UpData();//add
					this->InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_SystemUpdating));
					iSyschr.SendSecondRequest(EContentNewDownload);
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
				TInt firstid = EContentNewDownload;
				TInt secondid = aCommand-13;
				TInt thirdid = -1;
				TInt sendid = iMainEngine.GetSynchronContent().GetItemID(firstid,secondid,thirdid);
				if (this->iCacheEngine)
				{
					delete iCacheEngine;
					iCacheEngine = NULL;
				}
				iSearchid = sendid;
				iCacheEngine = iMainEngine.CreateCacheEngineL(*this,EContentNewDownload,sendid,0);
				iCacheEngine->RequestPageFileName(1);
				return;
			}
			
		}
		
	}
	
}
void NewestDownload::InputDialog()
{
	this->iInputDialog=iMainEngine.ControlFactory().CreateInputNumberDialog(*this,iMainEngine.GetDesById(ETurkeyTextRes_GoToPage),_L(""),1,this);
}
void NewestDownload::DialogEvent(TInt aCommand)
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
			//这里是取消评分。
			iShowGrade->CancelSendRequest();
			break;
		}
	case 15:
		{
			if (iDialog)
			{
				RemoveAndDeleteControl(iDialog);
				iDialog = NULL;
			}
			this->iShowInfo->CancelSendRequest();
			break;
			//这里是取消信息详情。
		}//add
	}
}
void NewestDownload::InputResponseEvent(TInt aEvent,const TDesC& aText)
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
				iCacheEngine->RequestPageFileName(page);//add
			
			iJumpFlage = 1;
			iJumpPage = page;
		}
	}
}
void NewestDownload::Goto(TInt apage)
{
	iCacheEngine->RequestPageFileName(apage);
	//这里跳转
	 //this->DrawRightInfo();
}
void NewestDownload::Changpage(TBool aflalgs)
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
	iCacheEngine->RequestPageFileName(temppage);//add
}
void NewestDownload::Changdrawpage(TBool aflalgs)
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
void NewestDownload::HandleResponseEvent(TInt aEvent,TInt aType)
{
	if (iDialog)
	{
		RemoveAndDeleteControl(iDialog);
	}

	if (aEvent == EHttpNoError)
	{
		switch(aType)
		{
		case EHttpGrade:
			{
				this->InitDialog(iShowGrade->GetInfo());
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
void NewestDownload::GradeEvent(TInt aEvent,TInt aGrade)
{
	if(aEvent == 0)
	{
		this->iGrade = aGrade;
		this->SendMyGrade();
		//这里写处理确定的事件。
	}
}

void NewestDownload::InitDialog(const TDesC& aText)//用来初始化对话框
{
	iDialog=iMainEngine.ControlFactory().CreateTipDialog(aText,*this,2,this);//这里会有内存泄漏吧. 
}

void NewestDownload::InitGradeDialog()//评分界面。
{
	CGradeDialog* gradeDialog=iMainEngine.ControlFactory().CreateGradeDialog(*this,this);//评分控件
}

void NewestDownload::InitWaitDialog(const TDesC& aText)
{
	iDialog=iMainEngine.ControlFactory().CreateWaitDialog(aText,*this,1,this);
}

void NewestDownload::SendMyGrade()
{
	iDialog=iMainEngine.ControlFactory().CreateWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting),*this,14);//add;
	if(!iShowGrade)
	{
		iShowGrade=CHandleGrade::NewL(*this,iMainEngine);
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
	iShowGrade->SendRequest(downid,iGrade);
	delete iBuf;
	iBuf = NULL;
}
void NewestDownload::ShowList()
{
	
	TInt allcount = iDesArray->Count();
	if (this->ilistbox)
	{
		if (allcount <= 0)
		{
			this->InitDialog(iMainEngine.GetDesById(ETurkeyTextRes_ListNull));
			return;
		}
		_LIT(KNAMEFLAGE,",");	

		//this->ilistbox->Reset();//delete by hesanyuan
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
			/*iIconBitmap = bitmapFactory.GetFileTypeIcon(1);
			iIconBitmapMask = bitmapFactory.GetFileTypeIconMask(1);*/
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

			this->ilistbox->AppendL(CCurThreeListBoxItem::NewL(tempbuf,typebuf,hot,iIconBitmap,iIconBitmapMask));
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
		}			*/
//add
		this->DrawRightInfo();//add
	}
}
void NewestDownload::ReadXML(const TDesC& aFileName)
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
		return;//add
	}
	iCurpage = atoi( cpnext->FirstChild()->Value() );//得到当前页数.	

	TiXmlElement* apnext = RootElement->FirstChildElement("ap");
	if(!apnext)
	{
		return;//add
	}
	if (!apnext->FirstChild())
	{
		return;
	}
	iAllpage = atoi(apnext->FirstChild()->Value());	//得到总的页数

	iDesArray->Reset();
	ilistbox->AppendL(CNormalListBoxItem::NewL(_L("")));
	this->ilistbox->Reset();//add by hesanyuan
	this->DrawRightInfo();//add

	TiXmlElement* Lnext = RootElement->FirstChildElement("l");
	while (Lnext)
	{
		//循环读L里的数据。
		TiXmlNode* nodechlild = Lnext->FirstChild();
		if (!nodechlild)
		{
			return;//add by hesanyuan
		}
		iDesArray->AppendL(* CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)Lnext->FirstChild()->Value())));
		Lnext=Lnext->NextSiblingElement();
		//	ilistbox->AppendL(* CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)Lnext->FirstChild()->Value())));

	}
	this->ShowList();


}

void NewestDownload::DownLoadFile(TInt adowntype)
{

	TInt fileSize=0;
	TFileName downLoadUrl;//不知道这样会不会有问题.
	TFileName fileName;

	TInt flage = this->AdvertisMent();
	if (flage == 2)
	{
		GetDownLoadInfo(downLoadUrl,fileName,fileSize);

		iMainEngine.WriteLog16(downLoadUrl);
		iMainEngine.WriteLog16(fileName);

		TBuf<10> tm;
		tm.AppendNum(fileSize);
		iMainEngine.WriteLog16(tm);

	//	this->AddDownLoadList(*this,2,downLoadUrl,fileName,fileSize,0,0,1);//delete by hesanyuan
		iDownType = AddDownLoadList(*this,13,downLoadUrl,fileName,fileSize,0,adowntype,1);//add by hesanyuan
	}
	else
	{
		return;
	}

	//iMainEngine.GetDownLoadEngine().AddTaskToDownLoadThread();
}

void NewestDownload::GetGradid(TInt& aFirstid,TInt& aSecondid,TInt& aInput)
{
	aFirstid = EContentNewDownload;
	aSecondid = iMainEngine.GetSynchronContent().GetItemID(EContentTypeDownload,aInput,-1);
	//TInt listindex = this->ilistbox->CurItemIndex();//要改.
	//aThirdid = iMainEngine.GetSynchronContent().GetItemID(EContentTypeDownload,aType,listindex);
}
void NewestDownload::CacheUpdate(const TDesC& aFileName)
{
	iMainEngine.WriteLog16(_L("CSutraDownload::CacheUpdate"));
	if(ilistbox)
	{
		iMainEngine.WriteLog16(_L("CSutraDownload::CacheUpdate InitListBox"));
		// 		RFs
		// 		ASSERT(BaflUtils::FileExists(fs,aFileName));
	//	ilistbox->Reset();//delete by hesanyuan
		//iDesArray->Reset();
		TInt itemp = 0;
		//ilistbox->AppendL(CNormalListBoxItem::NewL(_L("")));
		//this->ilistbox->Reset();//add by hesanyuan
		ReadXML(aFileName);	
		TInt allcount = iDesArray->Count();//add
		if (allcount <= 0)
		{//add
			this->InitDialog(iMainEngine.GetDesById(ETurkeyTextRes_ListNull));
			return;
		}
		//this->DrawRightInfo();//delete
		itempdes = &aFileName;//add
	}

}
void NewestDownload::GetDownLoadInfo(TDes& aUrl,TDes& aFileName,TInt iFileSize)
{
	TInt index = this->ilistbox->CurItemIndex();
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
TInt NewestDownload::AdvertisMent()
{//广告。
	TInt index = this->ilistbox->CurItemIndex();
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
		iDownType = AddDownLoadList(*this,13,downLoadUrl,tempbuf,ifielsize,0,0,1);//add by hesanyuan
		//this->AddDownLoadList(*this,);//在这里传参数就可以了.
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
		this->iMainEngine.LaunchWapBrowswerL(downLoadUrl);

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
TBool NewestDownload::UpData()
{
	if (iSyschr.IsUpdate(EContentNewDownload,-1,-1))
	{
		this->InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_SystemUpdating));
		iSyschr.SendSecondRequest(EContentNewDownload);
		iMainEngine.WriteLog16(_L("EContentNewDownload"));
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

void NewestDownload::DrawRightInfo()
{
	TBuf<20>page;
	page.Zero();
	_LIT(tigle,"/");
	page.AppendNum(iCurpage);
	page.Append(tigle);
	page.AppendNum(iAllpage);
	this->iTitlebar->SetRightInfoL(page);
}

void NewestDownload::InfoResponseEvent(TInt aEvent)
{
	//这里也可以处理信息详情。
}


void NewestDownload::GetInfo()
{
	//InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting));
	iDialog=iMainEngine.ControlFactory().CreateWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting),*this,15);
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
void NewestDownload::ContentInfoEvent(TInt aLeftCommond)
{
	iContentInfo=NULL;
	this->iShowInfo->CancelSendRequest();
	if(aLeftCommond==12)
	{
		DownLoadFile(0);
	}
}

void NewestDownload::InitInfoDialog(const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,TInt aCommand,TBool aBool)
{
	if(iContentInfo)
		RemoveAndDeleteControl(iContentInfo);
	iContentInfo=iMainEngine.ControlFactory().CreateContentInfoDialog(*this,aName,aSize, aType,aRemark,aCommand,aBool,this);
}

void NewestDownload::Search()
{
	TBuf<20>tempBuf;
	if(iMainEngine.GetSynchronContent().GetValue(EContentNewDownload,-1,-1).Length()==0)
	{
		tempBuf.Append(iMainEngine.GetDesById(ECoCoTextRes_MainView_Two));
	}
	else
	{
		tempBuf.Append(iMainEngine.GetSynchronContent().GetValue(EContentNewDownload,-1,-1));
	}
	
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

	//TInt firstid = EContentNewDownload;
	//TInt secondid = 0;
	//TInt thirdid = -1;
	//TInt sendid = iMainEngine.GetSynchronContent().GetItemID(firstid,secondid,thirdid);

	urlBuf.Append(_L8("http://59.36.96.182/coco/psearch?p=1&username="));
	urlBuf.Append(iMainEngine.GetIMEI());
	urlBuf.Append(_L8("&bid="));						//id
	urlBuf.AppendNum(iSearchid);								//ilistbox->CurItemIndex()
	urlBuf.Append(_L8("&keyword="));					//关键字.
	//urlBuf.Append(iMainEngine.GetDesById(ETurkeyTextRes_All));

	SetChildWindow(iMainEngine.WindowFactory().CreateSearchResultView(ESearchResultView,this,ilistbox->CurItemIndex(),urlBuf,tempBuf));

	delete iBuf;
	iBuf = NULL;
}

void NewestDownload::CheckDowndtype()
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
	//	iDialog=iMainEngine.ControlFactory().CreateTipDialog(_L("is down?"),*this,3);//delete by hesanyuan
		//iDialog = iMainEngine.ControlFactory().CreateInquireDialog(iMainEngine.GetDesById(ETurkeyTextRes_MainDown),*this,3,4);//add by hesanyuan
		DownLoadFile(0);
	}
	//iShowInfo->SendRequest(downid);

	delete iBuf;
	iBuf = NULL;
}
void NewestDownload::CacheFailed()
{
	iMainEngine.ControlFactory().CreateInquireDialog(iMainEngine.GetDesById(ECoCoTestRes_NetError_IsRetry),*this,7,8);
}//add
