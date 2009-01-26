/*
============================================================================
 Name        : NewDownload.cpp
 Author      : hsany
 Version     :
 Copyright   : Your copyright notice
 Description : CNewDownload implementation
============================================================================
*/

#include "NewDownload.h"
#include "GradeDialog.h" 
#include "HandleGrade.h"
#include "StaticCommonTools.h"
#include "WindowFactory.h"
#include "CacheEngine.h"
#include "HandleSynchronization.h"
#include "HandleInfo.h"
#include "SystemInfoDialog.h"
#include "ControlFactory.h"

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
	EAll,
	EPicture,
	EMusic,
	EVido,
	EGame,
	ETool,
	EBook,
	ETopic,
	EOK,
	EUpData
};

CNewDownload::CNewDownload(CWindow* aParent,CMainEngine& aMainEngine):CWindow(EnewDownloadView,aParent,aMainEngine)
{
	this->iflalgs = FALSE;
	this->ilistbox = NULL;
	this->iTitlebar = NULL;
	this->titletxt = NULL;
	iDesArray = NULL;//用于存放所有的值.
	this->iInputDialog = NULL;
	this->iDialog = NULL;
	this->iSecondID = 5;
	this->iThirdID = 0;
	this->iCacheEngine = NULL;
	this->iFirstID = EContentBestDownload;
	this->iShowInfo =NULL;
	iContentInfo = NULL;
	this->iMidpage = 1;
	iJumpPage = 1;//add
	iJumpFlage = 0;
	iCurpage = 0;
	iAllpage = 0;//add
	iGradeFlage = FALSE;//add
	itempdes = NULL;//add
//	this->iDialog = NULL;
	// No implementation required
}


CNewDownload::~CNewDownload()
{
	UtilityTools::WriteLogsL(_L("CNewDownload::~CNewDownload"));
	if (this->iDesArray)
	{
		delete iDesArray;
		iDesArray = NULL;
	}
	if (iCacheEngine)
	{
		delete iCacheEngine;
		iCacheEngine = NULL;
	}
	UtilityTools::WriteLogsL(_L("CNewDownload::~CNewDownload End"));
}

CNewDownload* CNewDownload::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CNewDownload* self = new (ELeave)CNewDownload(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CNewDownload* CNewDownload::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CNewDownload* self=CNewDownload::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CNewDownload::ConstructL()
{
	ChangeButton();
	iDesArray = new (ELeave) CDesCArrayFlat(10);
	iMainEngine.ControlFactory().SetParentWindow(this);

	this->ilistbox = iMainEngine.ControlFactory().CreateDefaultListBoxL();
	ilistbox->AppendL(CNormalListBoxItem::NewL(_L("")));
	ilistbox->Reset();//add by hesanyuan;
	//CBitmapFactory& bitmapFactory = iMainEngine.GetBitmapFactory();
	//iIconBitmap = bitmapFactory.GetFileTypeIcon(1);
	//iIconBitmapMask = bitmapFactory.GetFileTypeIconMask(1);
}
//////////////////////////////////////////////////////////////////////////
void CNewDownload::ConstructLater()
{
	ASSERT(NULL == iCacheEngine);
	iCacheEngine = iMainEngine.CreateCacheEngineL(*this,iFirstID,iSecondID,iThirdID);
	iCacheEngine->RequestPageFileName(iMidpage);

}

void CNewDownload::DoActivateL()
{
	TInt temp = 0;
	CreateTileAndList();
	//this->AddListITemL();
}

void CNewDownload::CreateTileAndList()
{
	if (!this->iTitlebar)
	{
		this->iTitlebar = iMainEngine.ControlFactory().CreateDefaultTitleBarL(this->titlebuf);
		this->DrawRightInfo();
	}
	/*if (!this->ilistbox)
	{
		this->ilistbox = iMainEngine.ControlFactory().CreateDefaultListBoxL();
		_LIT(name,"NewestDownload");
		ilistbox->AppendL(CNormalListBoxItem::NewL(name));
		ASSERT(NULL == iCacheEngine);
		iCacheEngine = iMainEngine.CreateCacheEngineL(*this,iFirstID,iSecondID,iThirdID);
		iCacheEngine->RequestPageFileName(iMidpage);
	}*/
}
 
void CNewDownload::AddListITemL()
{
	/*if (this->ilistbox)
	{
		_LIT(name,"readxml faile");
		this->ilistbox->AppendL(CNormalListBoxItem::NewL(name));
		this->ReadXML();
	}*/
}
void CNewDownload::DoDeactivate()
{
	//this->iTitlebar = NULL;
	//ilistbox=NULL;
	RemoveControl(iTitlebar);
	iTitlebar = NULL;
	//DeleteAllControl();
}
void CNewDownload::DoSizeChanged()
{

}

void CNewDownload::CacheUpdate(const TDesC& aFileName)
{
	iMainEngine.WriteLog16(_L("CSutraDownload::CacheUpdate"));

	if(ilistbox)
	{
		iMainEngine.WriteLog16(_L("CSutraDownload::CacheUpdate InitListBox"));

		//iDesArray->Reset();
		TInt itemp = 0;
		//ilistbox->AppendL(CNormalListBoxItem::NewL(_L("")));
		//ilistbox->Reset();//add by hesanyuan
		ReadXML(aFileName);		
		TInt allcount = iDesArray->Count();//add
		if (allcount <= 0)
		{//add
			this->InitDialog(iMainEngine.GetDesById(ETurkeyTextRes_ListNull));
			return;
		}
		//this->DrawRightInfo();
	}
}
void CNewDownload::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = this->iMainEngine.GetTextRes();
	SetLeftButtonL(textRes[ETurkeyTextRes_Option]);
	SetRightButtonL(textRes[ETurkeyTextRes_Back]);
}
void CNewDownload::Search()
{
	TBuf<10>titlekey;
	TBuf<10>listkey;
	this->GetKeyWord(titlekey,listkey);
	iMainEngine.WriteLog16(titlekey);
	iMainEngine.WriteLog16(listkey);
	iMainEngine.WriteLog16(_L("SearchKey"));

	TBuf<20>tempBuf;
	tempBuf.Append(titlekey);
	tempBuf.Append(_L("->"));
	tempBuf.Append(listkey);

	//CNormalListBoxItem& item = (CNormalListBoxItem&)ilistbox->CurItem();
	//tempBuf.Append(item.GetText());
	TInt index = this->ilistbox->CurItemIndex();
	HBufC* iBuf = NULL;
	const TPtrC& ptr = (*iDesArray)[index];//原来他是返回一个TPtrC的值。
	iBuf=HBufC::NewL(ptr.Length());
	iBuf->Des().Append(ptr);

	TBuf8<200> urlBuf;
	urlBuf.Append(_L8("http://59.36.96.182/coco/fsearch?p=1&username="));
	urlBuf.Append(iMainEngine.GetIMEI());
	urlBuf.Append(_L8("&bid="));						//id//&sid=38
	if (this->iSecondID == -1)
	{
		return;
	}
	urlBuf.AppendNum(this->iSecondID);	//this->iSecondID				//this->ilistbox->CurItemIndex()
	urlBuf.Append(_L8("&sid="));
	urlBuf.AppendNum(this->iThirdID);
	//urlBuf.Append(_L8("&pageid="));						//页
	//urlBuf.AppendNum(this->iCurpage);					//this->iCurpage
	urlBuf.Append(_L8("&keyword="));					//关键字.
	//urlBuf.Append(iMainEngine.GetDesById(ETurkeyTextRes_All));
	iMainEngine.WriteLog8(urlBuf);
	
	SetChildWindow(iMainEngine.WindowFactory().CreateSearchResultView(ESearchResultView,this,this->ilistbox->CurItemIndex(),urlBuf,tempBuf));

	delete iBuf;
	iBuf = NULL;
}
void CNewDownload::DoPopUpMenuCommand(TInt aCommand)
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
			//iDialog=iMainEngine.ControlFactory().CreateTipDialog(_L("is down?"),*this,3);
			TInt type = 0;
			this->EnterModel(type);
			break;
		}
	case EFileGrade://评分
		{
			this->InitGradeDialog();
			break;
		}
	case EcontentDetial://信息详情.
		{
			GetInfo();
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
			}//add
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
			break;
		}
	default:
		break;
	}
}
void CNewDownload::DoDraw(CGraphic& aGraphic)const
{
	
}
TBool CNewDownload::DoKeyEventL(TInt aKeyCode)
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
			//	PrepareExit();
			GoBackToParentWindow();
			//HideApplication();
			break;

		case EKeyDevice3:	//OK键,进入模块		
			{
				TInt type = 0;
				this->EnterModel(type);
			}
			break;
		case EKeyRightArrow:
			{
				if (iDesArray->Count()>0)
				{
					iflalgs = TRUE;
					Changpage(iflalgs);
				}//add
				break;
			}
		case EKeyLeftArrow:
			{
				if (iDesArray->Count()>0)
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
TBool CNewDownload::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}
void CNewDownload::InitMenu()
{
	if (iDesArray->Count()<=0)
	{
		CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);
		//popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Ok),EOK);//delete by hesanyuan
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Res_UpDate),EUpData);
		return;
	}
	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);//CreatePopUpMenu(*this);


	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Search_SDownLoad),Edownload);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_FileGrade),EFileGrade);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_ContentDetail),EcontentDetial);
	//////////////////////////////////////////////////////////////////////////
	//
	if(iViewType==EContentTypeDownload)
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MainSearch),EMainSearch);

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
void CNewDownload::ChangListItem(TInt aItem)
{
}
void CNewDownload::EnterModel(TInt aType)
{
	if (iDesArray->Count()<=0)
	{
		return;//add by hesanyuan
	}
	//iDialog=iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_MainDown),*this,3);//delete by hesanyuan
	CheckDowndtype();
}
TBool CNewDownload::HandleIconKeyL(TInt aKeyCode)
{
	return EFalse;

}
void CNewDownload::Goto(TInt apage)
{
	iCacheEngine->RequestPageFileName(apage);
	//这里跳转
//	this->DrawRightInfo();
}
void CNewDownload::Changpage(TBool aflalgs)
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

	}//add
	iCacheEngine->RequestPageFileName(temppage);
	//这里是翻页.
}
void CNewDownload::Changdrawpage(TBool aflalgs)
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
void CNewDownload::InputResponseEvent(TInt aEvent,const TDesC& aText)
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
			iJumpPage = page;
		}
	}
}
void CNewDownload::InputDialog()
{
	this->iInputDialog=iMainEngine.ControlFactory().CreateInputNumberDialog(*this,iMainEngine.GetDesById(ETurkeyTextRes_GoToPage),_L(""),1,this);
}
void CNewDownload::ShowList()//用来在列表中显示数据.
{
	TInt i = 0;
	TInt allcount = iDesArray->Count();
	if (this->ilistbox)
	{
		
		if (allcount <= 0)
		{
			this->InitDialog(iMainEngine.GetDesById(ETurkeyTextRes_ListNull));
			return;
		}
		_LIT(KNAMEFLAGE,",");	
	//	this->ilistbox->Reset();//delete by hesanyuan
		
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
		}	*/
		
		this->DrawRightInfo();//add
	}
}

void CNewDownload::HandleResponseEvent(TInt aEvent,TInt aType)
{
	if(iDialog)
		RemoveAndDeleteControl(iDialog);

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
		case EHttpTempBuf:
			{
				if(iShowInfo->IsImage())
				{
					if(iContentInfo)
						RemoveAndDeleteControl(iContentInfo);

				 	iContentInfo=iMainEngine.ControlFactory().CreateContentInfoDialog(*this,iShowInfo->GetName(),iShowInfo->GetSize(),iShowInfo->GetType(),iShowInfo->GetRemark(),iShowInfo->GetImagePath(),12,iShowInfo->IsImage());
				 	break;
				}
				else
				{		
				 	InitInfoDialog(iShowInfo->GetName(),iShowInfo->GetSize(),iShowInfo->GetType(),iShowInfo->GetRemark(),12,iShowInfo->IsImage());
				 	break;
				}
				break;
			}
		case EHttpContentInfo:
			{				
				InitInfoDialog(iShowInfo->GetName(),iShowInfo->GetSize(),iShowInfo->GetType(),iShowInfo->GetRemark(),12,iShowInfo->IsImage());
				break;
			}
		}		
	}
}

void CNewDownload::InitDialog(const TDesC& aText)//用来初始化对话框
{
	iDialog=iMainEngine.ControlFactory().CreateTipDialog(aText,*this,2,this);//这里会有内存泄漏吧. 
}


void CNewDownload::ReadXML(const TDesC& aFileName)
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
		return;
	}
	if (!apnext->FirstChild())
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
			return;//add by hesanyuan
		}
		iDesArray->AppendL(* CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)Lnext->FirstChild()->Value())));
		Lnext=Lnext->NextSiblingElement();
		//	ilistbox->AppendL(* CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)Lnext->FirstChild()->Value())));

	}
	this->ShowList();
	this->iMidpage = iCurpage;
}
//
//响应MDialogObserver
//
void CNewDownload::DialogEvent(TInt aCommand)
{
	switch(aCommand)
	{
		case -98:
		break;
		case 3:
		DownLoadFile(0);
		break;
		case 1:
		case 4:
			{
				if (iDialog)
				{
					RemoveAndDeleteControl(iDialog);
					iDialog = NULL;
				}
			}
			break;
		case 13:
			{
				if ((iDwonType == 2)||(iDwonType == 5))
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
void CNewDownload::DownLoadFile(TInt aDowndType)
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
		iMainEngine.WriteLog16(downLoadUrl);
		//this->AddDownLoadList(*this,2,downLoadUrl,fileName,fileSize,0,0,1);//delete by hesanyuan
		iDwonType = AddDownLoadList(*this,13,downLoadUrl,fileName,fileSize,0,aDowndType,1);//add by hesanyuan		
	}
	else
	{
		return;
	}

	//iMainEngine.GetDownLoadEngine().AddTaskToDownLoadThread();
}
void CNewDownload::GetDownLoadInfo(TDes& aUrl,TDes& aFileName,TInt& iFileSize)
{

	//得到列表里的内容。
	//TInt index=this->ilistbox->CurItemIndex(); 
	//CNormalListBoxItem& item = (CNormalListBoxItem&)ilistbox->CurItem();
	//HBufC* buf=HBufC::NewL((item.GetText()).Length());//得到当前长度。
	//buf->Des().Copy(item.GetText() );//上面三句我改动过
	TInt temp = 0;
	TInt index = this->ilistbox->CurItemIndex();
	HBufC* buf = NULL;
	const TPtrC& ptr = (*iDesArray)[index];//原来他是返回一个TPtrC的值。
	buf=HBufC::NewL(ptr.Length());
	buf->Des().Append(ptr);
	iMainEngine.WriteLog16(*buf);
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
TInt CNewDownload::AdvertisMent()
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
		//this->AddDownLoadList(*this,2,downLoadUrl,tempbuf,ifielsize,0,0,1);//delete by hesanyuan
		iDwonType = AddDownLoadList(*this,13,downLoadUrl,tempbuf,ifielsize,0,0,1);//add by hesanyuan
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
void CNewDownload::GradeEvent(TInt aEvent,TInt aGrade)
{
	if(aEvent == 0)
	{
		this->iGrade = aGrade;
		this->SendMyGrade();
		//这里写处理确定的事件。
	}
}
void CNewDownload::SendMyGrade()
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

void CNewDownload::InitWaitDialog(const TDesC& aText)
{
	iDialog=iMainEngine.ControlFactory().CreateWaitDialog(aText,*this,1,this);
}
void CNewDownload::InitGradeDialog()//评分界面。
{
	CGradeDialog* gradeDialog=iMainEngine.ControlFactory().CreateGradeDialog(*this,this);//评分控件
}

void CNewDownload::GetKeyWord(TDes& desleft,TDes& desright)
{
	if (this->titlebuf.Length() == 0)
	{
		return;
	}
	TInt postion = this->titlebuf.Find(_L("->"));
	if (postion == KErrNotFound)
	{
		return;
	}
	desleft.Zero();
	desleft.Copy(this->titlebuf.Left(postion));
	desright.Zero();
	desright.Copy(this->titlebuf.Right(titlebuf.Length()-postion-2));
	///desleft.this->titlebuf.Left(postion);//二级栏目
	//desright = this->titlebuf.Right(titlebuf.Length()-postion-2);//三级栏目
	
}
void CNewDownload::DrawRightInfo()
{
	
	if(this->iTitlebar)
	{	
		TBuf<20>page;
		page.Zero();
		_LIT(tigle,"/");
		page.AppendNum(iCurpage);
		page.Append(tigle);
		page.AppendNum(iAllpage);
		this->iTitlebar->SetRightInfoL(page);
	}
}

void CNewDownload::GetInfo()
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

void CNewDownload::InfoResponseEvent(TInt aEvent)
{
	//这里也可以处理信息详情。
}
void CNewDownload::ContentInfoEvent(TInt aLeftCommond)
{
	//这里处理信息详情的事件。
	iContentInfo=NULL;
	iShowInfo->CancelSendRequest();
	if(aLeftCommond==12)
	{
		DownLoadFile(0);
	}
}

void CNewDownload::InitInfoDialog(const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,TInt aCommand,TBool aBool)
{
	if(iContentInfo)
		RemoveAndDeleteControl(iContentInfo);

	iContentInfo=iMainEngine.ControlFactory().CreateContentInfoDialog(*this,aName,aSize, aType,aRemark,aCommand,aBool,this);
}

void CNewDownload::CheckDowndtype()
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
void CNewDownload::CacheFailed()
{
	iMainEngine.ControlFactory().CreateInquireDialog(iMainEngine.GetDesById(ECoCoTestRes_NetError_IsRetry),*this,7,8);

}//add
