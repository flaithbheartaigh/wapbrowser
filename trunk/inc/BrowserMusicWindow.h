
#include "Window.h"

#include "PopUpMenuObserver.h"
#include "MusicPlayManageEngine.h"

class CMainEngine;
class CListBox;

class CBrowserMusicWindow : public CWindow ,public MPopUpMenuObserver,public MAudioPlayNoticeObserver
{
public: 
	// Constructors and destructor
	~CBrowserMusicWindow();
	static CBrowserMusicWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CBrowserMusicWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CBrowserMusicWindow(CWindow* aParent,CMainEngine& aMainEngine);
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
	virtual void DoPopUpMenuCommand(TInt aCommand);

	void AudioUpdate(const TDesC& aCurrentTime,const TDesC& aTotalTime,TInt aPercent) ;

	void AudioSelected(const TInt aSelectIndex);

	void AudioFileUpdate();
private:
	void	InitPopUpMenu();

	void InitImage();	 

	void InitListBox();

	void InitMusicInterface(CGraphic& aGraphic) const;
private:
	CMainEngine& iMainEngine;

	CArrayFixFlat<TFileName>* iFileNameArray;

	CListBox* iListBox;

	CFbsBitmap* iMusicVoiceImage;
	CFbsBitmap* iMusicMainImage;

	TBuf<256> iTotalTime;
	TBuf<256> iCurrentTime;

	TInt iCurrentPlayIndex;
	TInt iPercent;
};


