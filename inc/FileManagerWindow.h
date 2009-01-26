/*
============================================================================
 Name        : FileManagerWindow.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CFileManagerWindow declaration
============================================================================
*/

#ifndef FILEMANAGERWINDOW_H
#define FILEMANAGERWINDOW_H

#include "Window.h"
#include "NaviPaneObserver.h"
#include "PopUpMenuObserver.h"
#include "FileEngineObserver.h"

class CListBox;
class CFileEngine;
class RFs;

class CFileManagerWindow : public CWindow , public MNaviPaneObserver , public MPopUpMenuObserver , public MFileEngineEvent
{
public: // Constructors and destructor
	~CFileManagerWindow();
	static CFileManagerWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CFileManagerWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CFileManagerWindow(CWindow* aParent,CMainEngine& aMainEngine);
	void ConstructL();

private://From CWindow
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

public://From MNaviPaneObserver
	virtual void NaviPaneIndexChanged(TInt aNewIndex);

public://From MPopUpMenuObserver
	virtual void DoPopUpMenuCommand(TInt aCommand);

public://From MFileEngineEvent
	virtual void Update(TInt aReason);

private:
	void	InitNaviPane();
	void	InitPopUpMenu();

	void	InitListBox();

	//void	GetFolderPath(TDes& aFilePath,TInt aIndex) const;
	void	GetFolderPath(TInt aIndex);

	void	ReadFileNameList(const TDesC& strPath, 
								const TInt aSortType,
								CArrayFixFlat<TFileName>* pArray, 
								CArrayFixFlat<TUint8>* pDirArray);

	void	ExecuteCurFile();
	void	ExecuteCurFileWith();
	void	EnterFolder();
	void	ShowCurFileAttribute();
	void	DeleteCurFile();
	void	RenameCurFile();
	void	SaveAsCurFile();
	void	RefreshList();
	void	CreateFolder();
	void	CopyFile();
	void	PasteFile();
	void	Enter();
	void	GoBack();

	void	GoBackToParentFolder();

	TBool	CurIsFolder() const;

	void	ShowFileByDate();
	void	ShowFileByName();

	TFileName	GetCurFileFullName() const;

private:
	CFileEngine&	iFileEngine;
	CListBox*		iTempListBox;
	TFileName		iCurPath;
	RFs&			iFs;

	TInt			iSortType;

	const CFbsBitmap*	iIconBitmap;
	const CFbsBitmap*	iIconBitmapMask;

	TBool			iIsSubFolder;
};

#endif // FILEMANAGERWINDOW_H
