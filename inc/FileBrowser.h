/*
============================================================================
 Name        : FileBrowser.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CFileBrowser declaration
 描述		 : 浏览文件或文件夹
 说明		 : 初始化时将目录设置到根目录,可以从盘符浏览文件
			   可调用SetPath设置当前目标,如果设置的目录不存在,则报错
============================================================================
*/

#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include "Control.h"


class CMainEngine;
class CListBox;
class MFileBrowserObserver;
class CFbsBitmap;

class CFileBrowser : public CControl
{
public:
	enum TBrowserType
	{
		EBrowserFolders,
		EBroswerFiles
	};
public: // Constructors and destructor
	~CFileBrowser();
	static CFileBrowser* NewL(CMainEngine& aMainEngine,MFileBrowserObserver& aFileBrowserObserver,TInt aBrowserType);
	static CFileBrowser* NewLC(CMainEngine& aMainEngine,MFileBrowserObserver& aFileBrowserObserver,TInt aBrowserType);

private:
	CFileBrowser(CMainEngine& aMainEngine,MFileBrowserObserver& aFileBrowserObserver,TInt aBrowserType);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

	virtual	const TDesC&	LeftButton() const;
	virtual	const TDesC&	RightButton() const;

public:
	void	SetPath(const TDesC& aFilePath);

	void	SetTitle(const TDesC& aTitleText);

	void	SetCommandType(TInt aLeftCommand,TInt aRightCommand);

private:
	void	DoConfirm();

	void	DoCancel();

	void	DoReadSub();

	void	InitRootDir();

	void	ReadSub();
	void	ReadParentDir();
/*
	void	ReadSubDir();
	void	ReadParentsDir();
	void	JudgeFile();
*/

	void	InitDirList(const TDesC& aFilePath);

	void	InitListBox();

	HBufC*	GetFileOrPathLC() const;

	TBool	CurIsFile() const;

	void	InitBitmaps();

	TFileName	ParseParentPath(const TDesC& aPath) const;

public:
	MFileBrowserObserver&	iObserver;
	CMainEngine&	iMainEngine;
	CListBox*		iListBox;

	RFs&			iFs;
	HBufC*			iTitleText;
	//HBufC*			iCurrentPath;
	TFileName		iCurrentPath;

	CFbsBitmap*		iSaveAsBmp;

	CDesCArray*		iDesArray;

//	RArray<TInt>	iIndexArray;		//保存每一级目录的当前索引,以便返回上一级目录时保持原来的焦点

	TInt			iBrowserType;

	TSize			iBmpSize;
	TPoint			iStartPoint;
	TPoint			iTextPoint;
	TInt			iLineHeight;

	TRect			iListBoxRect;

	TInt			iFolderNum;
	TInt			iFileNum;

	TInt			iLeftCommond;			//当按下确定键时传回什么事件
	TInt			iRightCommand;			//取消时传回什么事件
};

#endif // FILEBROWSER_H
