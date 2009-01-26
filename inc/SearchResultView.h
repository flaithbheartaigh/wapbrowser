/*
============================================================================
 Name        : SearchResultView.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSearchResultView declaration
============================================================================
*/

#ifndef SEARCHRESULTVIEW_H
#define SEARCHRESULTVIEW_H

// INCLUDES
#include <eikedwin.h>
#include "Window.h"
#include "PopUpMenuObserver.h"
#include "MControlObserver.h"
#include "DialogObserver.h"
// CLASS DECLARATION

class CMainEngine;
class CSearchListBox;
class CHandleInSearch;
class CDialog;
class CInputDialog;
class CContentInfoDialog;
class CTitleBar;
class CHandleInfo;
class CHandleGrade;
/**
*  CSearchResultView
* 
*/
class CSearchResultView : public CWindow , public MPopUpMenuObserver,public MHandleEventObserver,public MSearchListObserver,public MDialogObserver,public MContentInfoObserver,public MInputObserver,public MGradeObserver
{
public: // Constructors and destructor

	~CSearchResultView();
	static CSearchResultView* NewL(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex,const TDesC8& aURL,const TDesC& aTitle);
	static CSearchResultView* NewLC(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex,const TDesC8& aURL,const TDesC& aTitle);

private:

	CSearchResultView(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex);
	void ConstructL(const TDesC8& aURL,const TDesC& aTitle);

public://From Dialog
	virtual void DialogEvent(TInt aCommand);
public://From ContentInfoDialog
	virtual void ContentInfoEvent(TInt aLeftCommond);
public://From GradeDialog
	virtual void GradeEvent(TInt aEvent,TInt aGrade);
public://From CWindow
	//派生类实现激活视图
	virtual void	DoActivateL();
	//派生类实现冻结视图
	virtual void	DoDeactivate();
	//派生类实现绘图
	virtual void	DoDraw(CGraphic& aGraphic)const;
	//派生类实现按键事件响应
	virtual TBool	DoKeyEventL(TInt aKeyCode);
	//派生类实现命令事件响应
	virtual TBool	DoHandleCommandL(TInt aCommand);
	//派生类实现界面尺寸改变
	virtual void	DoSizeChanged();

	//派生类实现设置左右按钮,以便覆盖按钮的控件返回时回复原样
	virtual void	ChangeButton();
public:// From MInputObserver
	virtual void InputResponseEvent(TInt aEvent,const TDesC& aText);
public://From MSearchListObserver
	virtual void SearchListResponseEvent(TInt aEvent,const TDesC& aText);
public://From MPopUpMenuObserver
	virtual void DoPopUpMenuCommand(TInt aCommand);
public://From MHandleEventObserver
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);
private:
	
	void InitGradeDialog();

	void	DownLoadFile(TInt aType);

	void    GetTextSearch(const TDesC& aText);
	void	InitWaitDialog(const TDesC& aText,TInt aCommand);
	void	InitDialog(const TDesC& aText,TInt aCommand,TInt aRCommand=-99);
	void	InitInfoDialog(const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,TInt aCommand,TBool aBool);
	void	InitPopUpMenu();
	void	InitInputDialog();
	void	InitListBox();

	void    StartInSearch(const TDesC& aText);
	
	void    GetInfo();
	void    SentGrade();

	const TDesC8& GetUrlEncode(const TDesC& aText);

	TInt GetDownLoadInfo(TDes& aUrl,TDes& aFileName);

	void CheckDownLoadFile();
private:

	CTitleBar* iTitleBar;

	CHandleInfo* iShowInfo;
	CHandleGrade* iShowGrade;
	CContentInfoDialog* iContentInfo;

	CInputDialog* iInputDialog;

	CDialog* iDialog;

	CHandleInSearch* iInSearch;

	CSearchListBox* iSearchList;

	CMainEngine& iMainEngine;

	TInt iIndex;
	HBufC* iTitle;
	HBufC8* iUrl;

	HBufC8* iTempBuf;

	TBuf<20> iKeyWord;

	TInt iCurPage;
	TInt iAllPage;
	TInt iGrade;

	TInt iPopState;
	TInt iDownLoadState;
};

#endif // SEARCHRESULTVIEW_H
