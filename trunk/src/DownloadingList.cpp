/*
============================================================================
 Name        : DownloadingList.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CDownloadingList implementation
============================================================================
*/

#include "DownloadingList.h"
#include "DownloadManagerWindow.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "BitmapFactory.h"
#include "PopUpMenu.h"
#include "ListBox.h"
#include "PreDefine.h"
#include "TypeDefine.h"

enum TEventCmd
{
	ECmdStopTask,
	ECmdBeginTask,
	ECmdDelTask,
	ECmdStopAllTask,
	ECmdBeginAllTask,
	ECmdDelAllTask,
	ECmdGoBackToParentWindow,
	ECmdGoBackToMainWindow
};

CDownloadingList::CDownloadingList(CMainEngine& aMainEngine,CDownloadManagerWindow& aDownloadManagerWindow)
															: CControl(EDownloadingList)
															, iMainEngine(aMainEngine)
															, iDownloadManagerWindow(aDownloadManagerWindow)
															, iDownLoadEngine(aMainEngine.GetDownLoadEngine())
															//, iBitmapFactory(aMainEngine.GetBitmapFactory())
{
	iDownLoadEngine.SetNoticedWindow(this);
}

CDownloadingList::~CDownloadingList()
{
	TBool success = iDownloadManagerWindow.RemoveControl(iListBox);
	if(success)
	{
		delete iListBox;
	}

	iDownLoadEngine.SetNoticedWindow(NULL);
	iDownloadInfoArray.Close();
}

CDownloadingList* CDownloadingList::NewLC(CMainEngine& aMainEngine,CDownloadManagerWindow& aDownloadManagerWindow)
{
	CDownloadingList* self = new (ELeave)CDownloadingList(aMainEngine,aDownloadManagerWindow);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CDownloadingList* CDownloadingList::NewL(CMainEngine& aMainEngine,CDownloadManagerWindow& aDownloadManagerWindow)
{
	CDownloadingList* self=CDownloadingList::NewLC(aMainEngine,aDownloadManagerWindow);
	CleanupStack::Pop(); // self;
	return self;
}

void CDownloadingList::ConstructL()
{
	//iDownloadInfoArray.Reset();

	InitListBox();
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CDownloadingList::Draw(CGraphic& aGraphic) const
{

}

TBool CDownloadingList::KeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case EKeyDevice0:
		InitPopUpMenu();
		break;

		//哪个是删除键?
	case EKeyDelete:
	case EKeyBackspace:
		break;

	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}

TBool CDownloadingList::HandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CDownloadingList::SizeChanged(const TRect& aScreenRect)
{

}
//////////////////////////////////////////////////////////////////////////
//From MPopUpMenuObserver
//////////////////////////////////////////////////////////////////////////
void CDownloadingList::DoPopUpMenuCommand(TInt aCommand)
{
	switch(aCommand)
	{		
	case ECmdStopTask:
		StopTask();
		break;

	case ECmdBeginTask:
		BeginTask();
		break;

	case ECmdDelTask:
		DelTask();
		break;

	case ECmdStopAllTask:
		StopAllTask();
		break;

	case ECmdBeginAllTask:
		BeginAllTask();
		break;

	case ECmdDelAllTask:
		DelAllTask();
		break;

	case ECmdGoBackToParentWindow:
		iDownloadManagerWindow.GoBackToParentWindow();
		break;

	case ECmdGoBackToMainWindow:
		iDownloadManagerWindow.GoBackToMainWindow();
		break;

	default:
		ASSERT(EFalse);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//From MDownLoadNotice
//////////////////////////////////////////////////////////////////////////
void CDownloadingList::DownLoadEvent(TInt aTaskID,TInt aEventType)	//aEventType代表什么
{
	TDownLoadShowInfo* downloadInfo = NULL;
	TInt index = GetDownloadInfoByID(aTaskID,downloadInfo);
	if(-1 != index)
	{
		UtilityTools::WriteLogsL(_L("CDownloadingList::DownLoadEvent aEventType = %d,index = %d"),aEventType,index);
		if(0 == aEventType)
		{
			iDownloadInfoArray.Remove(index);
			iListBox->Remove(index);
		}
		else
		{
			downloadInfo->nStatus = aEventType;
			ChangeListItemIcon(index);
		}
	}
}

void CDownloadingList::DownLoadReceived(TInt aTaskID,TInt aTotalLen, TInt aReceivedLen)
{
	UtilityTools::WriteLogsL(_L("CDownloadingList::DownLoadReceived aTaskID = %d,aReceivedLen = %d"),aTaskID,aReceivedLen);
	TDownLoadShowInfo* downloadInfo = NULL;
	TInt index = GetDownloadInfoByID(aTaskID,downloadInfo);
	if(-1 != index)
	{
		UtilityTools::WriteLogsL(_L("index = %d"),index);
		downloadInfo->nFileLen = aTotalLen;
		downloadInfo->nDownLoadLen = aReceivedLen;
		ChangeListItemText(index);
	}
}

void CDownloadingList::AddDownLoadTask(const TDownLoadShowInfo* aAddTask)
{
	iDownloadInfoArray.Append(*aAddTask);
	AppendListItem(*aAddTask);
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CDownloadingList::InitPopUpMenu()
{
	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);
	if(iListBox->Count() > 0)
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadMenu_StopTask),ECmdStopTask);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadMenu_BeginTask),ECmdBeginTask);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadMenu_DelTask),ECmdDelTask);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadMenu_StopAllTask),ECmdStopAllTask);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadMenu_BeginAllTask),ECmdBeginAllTask);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadMenu_DelAllTask),ECmdDelAllTask);
	}
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Back),ECmdGoBackToParentWindow);
	//popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Back),ECmdGoBackToMainWindow);
}

void CDownloadingList::InitListBox()
{
	ASSERT(NULL == iListBox);
	iListBox = iMainEngine.ControlFactory().CreateDefaultListBoxL();
	//CBitmapFactory& bitmapFactory = iMainEngine.GetBitmapFactory();

	iDownLoadEngine.GetDownLoadingList(iDownloadInfoArray);

	for(TInt i = 0 ; i < iDownloadInfoArray.Count() ; i++)
	{
		AppendListItem(iDownloadInfoArray[i]);
	}
}

TInt CDownloadingList::GetDownloadInfoByID(TInt aTaskID,TDownLoadShowInfo*& aDownLoadShowInfo)
{
	TInt index = -1;
	for (TInt i = 0 ; i < iDownloadInfoArray.Count() ; i++)
	{
		if(iDownloadInfoArray[i].nTaskID == aTaskID)
		{
			index = i;
			aDownLoadShowInfo = &iDownloadInfoArray[i];
			break;
		}
	}
	return index;
}

void CDownloadingList::AppendListItem(const TDownLoadShowInfo& aDownloadInfo)
{
	TInt iconType = 0;
	CBitmapFactory& bitmapFactory = iMainEngine.GetBitmapFactory();
	switch(aDownloadInfo.nStatus)
	{
	case 1:
		iconType = EDownLoadWaiting;
		break;

	case 2:
	case 3:
		iconType = EDownLoadDownLoading;
		break;

	case 4:
		iconType = EDownLoadStop;
		break;

	case 5:
		iconType = EDownLoadError;
		break;

	default:
		//ASSERT(EFalse);
		break;
	}

	UtilityTools::WriteLogsL(_L("iconType = %d"),iconType);
	const CFbsBitmap* iconBitmap = bitmapFactory.GetDownLoadIcon(iconType);
	const CFbsBitmap* iconBitmapMask = bitmapFactory.GetDownLoadIconMask(iconType);

	//TBuf<40> buf;太小，已经测试出问题，卞涛修改
	TFileName buf;
	TInt nR = (TInt)(aDownloadInfo.nDownLoadLen /1024);
	TInt nT = (TInt)(aDownloadInfo.nFileLen /1024);

	buf.AppendNum(nR);
	buf.Append(_L("K"));
	buf.Append(_L("/"));
	buf.AppendNum(nT);
	buf.Append(_L("K"));

	buf.Append(aDownloadInfo.nFileName);
	//buf.Append(aDownloadInfo.nFileName.Left(10));
	//buf.Append(_L("..."));

	iListBox->AppendL(CNormalListBoxItem::NewL(buf,iconBitmap,iconBitmapMask,ETrue));
}

void CDownloadingList::ChangeListItemIcon(TInt aIndex)
{
	CNormalListBoxItem& listItem = (CNormalListBoxItem&)iListBox->GetItem(aIndex);


	TDownLoadShowInfo& info = iDownloadInfoArray[aIndex];

	TInt iconType = 0;
	switch(info.nStatus)
	{
	case 1:
		iconType = EDownLoadWaiting;
		break;

	case 2:
	case 3:
		iconType = EDownLoadDownLoading;
		break;

	case 4:
		iconType = EDownLoadStop;
		break;

	case 5:
		iconType = EDownLoadError;
		break;

	default:
		//ASSERT(EFalse);
		break;
	}
	UtilityTools::WriteLogsL(_L("iconType = %d"),iconType);
	CBitmapFactory& bitmapFactory = iMainEngine.GetBitmapFactory();
	const CFbsBitmap* iconBitmap = bitmapFactory.GetDownLoadIcon(iconType);
	const CFbsBitmap* iconBitmapMask = bitmapFactory.GetDownLoadIconMask(iconType);
	listItem.ChangeIcon(iconBitmap,iconBitmapMask);
}

void CDownloadingList::ChangeListItemText(TInt aIndex)
{
	CNormalListBoxItem& listItem = (CNormalListBoxItem&)iListBox->GetItem(aIndex);

	TDownLoadShowInfo& downloadInfo = iDownloadInfoArray[aIndex];

	TBuf<40> buf;
	TInt nR = (TInt)(downloadInfo.nDownLoadLen /1024);
	TInt nT = (TInt)(downloadInfo.nFileLen /1024);

	buf.AppendNum(nR);
	buf.Append(_L("K"));
	buf.Append(_L("/"));
	buf.AppendNum(nT);
	buf.Append(_L("K"));

	buf.Append(downloadInfo.nFileName.Left(10));
	buf.Append(_L("..."));

	UtilityTools::WriteLogsL(_L("CDownloadingList::ChangeListItemText"));
	UtilityTools::WriteLogsL(buf);
	listItem.ChangeText(buf);
}

void CDownloadingList::StopTask()
{
	TInt index = iListBox->CurItemIndex();
	const TDownLoadShowInfo& downLoadShowInfo = iDownloadInfoArray[index];
	iDownLoadEngine.PauseDownLoadingTask(downLoadShowInfo.nTaskID);
}

void CDownloadingList::BeginTask()
{
	TInt index = iListBox->CurItemIndex();
	const TDownLoadShowInfo& downLoadShowInfo = iDownloadInfoArray[index];
	iDownLoadEngine.BeginDownLoadingTask(downLoadShowInfo.nTaskID);
}

void CDownloadingList::DelTask()
{
	TInt index = iListBox->CurItemIndex();
	const TDownLoadShowInfo& downLoadShowInfo = iDownloadInfoArray[index];
	iDownLoadEngine.DelDownLoadingTask(downLoadShowInfo.nTaskID);

	iDownloadInfoArray.Remove(index);
	iListBox->Remove(index);
	//InitListBox();
}

void CDownloadingList::StopAllTask()
{
	iDownLoadEngine.PauseDownLoadingTask(-1);
}

void CDownloadingList::BeginAllTask()
{
	iDownLoadEngine.BeginDownLoadingTask(-1);
}

void CDownloadingList::DelAllTask()
{
	iDownLoadEngine.DelDownLoadingTask(-1);
	iDownloadInfoArray.Reset();
	iListBox->Reset();
}
