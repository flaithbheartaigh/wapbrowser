/*
============================================================================
 Name        : DownloadingList.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CDownloadingList declaration
============================================================================
*/

#ifndef DOWNLOADINGLIST_H
#define DOWNLOADINGLIST_H

#include "Control.h"
#include "PopUpMenuObserver.h"
#include "DownLoadEngine.h"

class CMainEngine;
class CListBox;
class CDownloadManagerWindow;
class CBitmapFactory;

class CDownloadingList : public CControl , public MPopUpMenuObserver , public MDownLoadNotice
{
public: // Constructors and destructor
	~CDownloadingList();
	static CDownloadingList* NewL(CMainEngine& aMainEngine,CDownloadManagerWindow& aDownloadManagerWindow);
	static CDownloadingList* NewLC(CMainEngine& aMainEngine,CDownloadManagerWindow& aDownloadManagerWindow);

private:
	CDownloadingList(CMainEngine& aMainEngine,CDownloadManagerWindow& aDownloadManagerWindow);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);	//待论证

public://From MPopUpMenuObserver
	virtual void DoPopUpMenuCommand(TInt aCommand);

public://From MDownLoadNotice
	virtual void DownLoadEvent(TInt aTaskID,TInt aEventType);
	virtual void DownLoadReceived(TInt aTaskID,TInt aTotalLen, TInt aReceivedLen);
	virtual void AddDownLoadTask(const TDownLoadShowInfo* aAddTask);

private:
	void	InitPopUpMenu();
	void	InitListBox();

	TInt	GetDownloadInfoByID(TInt aTaskID,TDownLoadShowInfo*& aDownLoadShowInfo);

	void	AppendListItem(const TDownLoadShowInfo& aDownloadInfo);
	void	ChangeListItemIcon(TInt aIndex);
	void	ChangeListItemText(TInt aIndex);

	void	StopTask();
	void	BeginTask();
	void	DelTask();
	void	StopAllTask();
	void	BeginAllTask();
	void	DelAllTask();

private:
	CMainEngine&				iMainEngine;
	CDownloadManagerWindow&		iDownloadManagerWindow;
	CDownLoadEngine&			iDownLoadEngine;
	RArray<TDownLoadShowInfo>	iDownloadInfoArray;
	CListBox*					iListBox;
};

#endif // DOWNLOADINGLIST_H
