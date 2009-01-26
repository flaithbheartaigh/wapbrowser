
#include "Window.h"
#include "DialogObserver.h"
#include "PopUpMenuObserver.h"
#include "DBEngine.h"

class CMainEngine;
class CScrollBar;
class CBookEngine;
class CTitleBar;

class CBrowserBookWindow : public CWindow ,public MDialogObserver, public MPopUpMenuObserver
{
public:
	~CBrowserBookWindow();
	static CBrowserBookWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CBrowserBookWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CBrowserBookWindow(CWindow* aParent,CMainEngine& aMainEngine);
	void ConstructL();

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

public:
	void SetFileName(const TDesC& aDes);
	void SetFileName(const TDesC& aDes,const TInt aLine);
	void LoadBookSection();
	
	TBool FormatRawBufToLines();

public:
	virtual void DialogEvent(TInt aCommand);

	virtual void DoPopUpMenuCommand(TInt aCommand);

private:
	TInt SetFullScreenMode( TBool bFullScreen );

	void	InitPopUpMenu();

	void InitScrollHeight();

	TInt SetBookMark();
	TInt GetBookMark();

	void ShowTitleBar();

private:
	CMainEngine& iMainEngine;

	CTitleBar* iTitleBar;

	CScrollBar*	iScrollbar;

	//	Added by xueyw on May 28 2007
	TBool				iFullScreenMode;

	TInt               iCurrentLine; //当前行

	TFileName          iFileName;

	CBookEngine*            iBookEngine; //load 

	//	Pointer array
	RPointerArray<HBufC>   iCurrentArrayBuf;//当前显示的

	//	Changed name by xueyw on May 28 2007
	HBufC*               iRawUnicodeBuf;

	//	To support load book by sections
	TInt                 iLastPos;
	TInt				iLastLoadCount	;
	TInt                 iCurrentPos;
	TInt				iHasPrev;
	TInt				iHasMore;

	TInt                 iBlooder;	 
	RArray<TInt>         iLastPosArray;

	RArray<TInt>		iTextPosArray;
};
