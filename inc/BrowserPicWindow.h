
#include "Window.h"

#include <FBS.H>

#include "DialogObserver.h"
#include "PopUpMenuObserver.h"
#include "MultimediaController.h"

class CMainEngine;
class CImageManipAdapter;
class CTitleBar;

class CBrowserPicWindow : public CWindow ,public MDialogObserver, public MPopUpMenuObserver,public MMultimediaController
{
public: 
	// Constructors and destructor
	~CBrowserPicWindow();
	static CBrowserPicWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CBrowserPicWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CBrowserPicWindow(CWindow* aParent,CMainEngine& aMainEngine);
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
	virtual void DialogEvent(TInt aCommand);

	virtual void DoPopUpMenuCommand(TInt aCommand);

	void	RedrawView (TInt aType);

	void	SetFileName(const TDesC& aDes,TInt aType);
private:
	void	InitPopUpMenu();

	void InitImage(const TDesC& aDes);	 

	void ReadImageFileNameList(const TDesC& aFilePath, CArrayFixFlat<TFileName>* aArray);

	TInt IsImageFile(const TDesC& aFile);

	void ShowTitleBar();

	void DrawImage(CGraphicsContext& aGc, TRect& aDesPoint,const TRect& aSrcRect,TInt aDrawType) const;

	inline void SetShowRect(TRect aShow)
	{
		iShowRect=aShow;
	}

	TInt SaveMbmL(const TFileName& aFileName, CFbsBitmap* aBitmap);

	TInt SetToWallPaper();
private:
	CFbsBitmap*		iShowBmp;

	CImageManipAdapter* iAdapter;

	CMainEngine& iMainEngine;

	CTitleBar* iTitleBar;

	TDisplayMode iDisplayModel;
	TBool  iIsAnimation;

	//是否全屏显示
	TBool iFullScreenShow;

	CArrayFixFlat<TFileName>*	iFileNameArray;
	TFileName	iFilePath;
	TFileName	iFileName;
	TInt	iSelIndex;	

	TRect iShowRect;

	TBool iOpenComplete;
};


