/*
============================================================================
 Name        : DownloadHistoryList.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CDownloadHistoryList declaration
============================================================================
*/

#ifndef DOWNLOADHISTORYLIST_H
#define DOWNLOADHISTORYLIST_H

#include "Control.h"
#include "PopUpMenuObserver.h"
#include "DownLoadHistoryEngine.h"
#include "MControlObserver.h"
#include "DialogObserver.h"

class CMainEngine;
class CListBox;
class CDownloadManagerWindow; 

class CDownloadHistoryList : public CControl , public MPopUpMenuObserver , public MDownLoadHistoryNotice , public MInputObserver , public MDialogObserver
{
public: // Constructors and destructor
	~CDownloadHistoryList();
	static CDownloadHistoryList* NewL(CMainEngine& aMainEngine,CDownloadManagerWindow& aDownloadManagerWindow);
	static CDownloadHistoryList* NewLC(CMainEngine& aMainEngine,CDownloadManagerWindow& aDownloadManagerWindow);

private:
	CDownloadHistoryList(CMainEngine& aMainEngine,CDownloadManagerWindow& aDownloadManagerWindow);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);	//待论证

public://From MPopUpMenuObserver
	virtual void DoPopUpMenuCommand(TInt aCommand);

public://From MDownLoadHistoryNotice
	virtual void Update();

public://From MInputObserver
	virtual void InputResponseEvent(TInt aEvent,const TDesC& aText);

public://From MDialogObserver
	virtual void DialogEvent(TInt aCommand);

private:
	void	InitPopUpMenu();
	void	InitListBox(TInt aPageIndex);

	void	GetStartPage();
	void	GetPrePage();
	void	GetNextPage();
	void	GetEndPage();
	void	GotoPage();
	void	OpenCurFile();
	void	OpenCurFileWith();
	void	DelTaskFromList();
	void	DelTaskFromFile();
	void	DelAllTaskFromList();
	void	DelAllTaskFromFile();

private:
	CMainEngine&			iMainEngine;

	CDownloadManagerWindow&	iDownloadManagerWindow;
	CDownLoadHistoryEngine&	iDownLoadHistoryEngine;

	CListBox*				iListBox;

	RArray<TDownLoadHistoryShowInfo> iDownLoadHistoryInfoArray;

	TInt			iTotalPageNum;
	TInt			iPageIndex;
};

#endif // DOWNLOADHISTORYLIST_H
