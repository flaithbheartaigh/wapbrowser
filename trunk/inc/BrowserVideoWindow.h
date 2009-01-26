
#include "Window.h"
#include "DialogObserver.h"
#include "VideoPlayObserver.h"

class CMainEngine;
class CTitleBar;
class CVideoPlayEngine;

class CBrowserVideoWindow : public CWindow,public MVideoPlayObserver, public MDialogObserver
{
public:
	~CBrowserVideoWindow();
	static CBrowserVideoWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CBrowserVideoWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CBrowserVideoWindow(CWindow* aParent,CMainEngine& aMainEngine);
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
	virtual void InitControllerCompletedL(TInt aError);
	virtual void PlayCompletedL(TInt aError);
	virtual void PlaybackPositionChangedL(TInt64 aPlaybackPosInSeconds, TInt64 aTotalLengthInSeconds);

public:
	virtual void DialogEvent(TInt aCommand);

	void SetFileName(const TDesC& aFileName);

private:
	void InitVideo(const TDesC& aFileName);
	void Play();
	void Pause();
	void Stop();

private:
	CMainEngine& iMainEngine;
	CVideoPlayEngine* iVideo;

	CTitleBar* iTitleBar;

	TFileName	iFullFileName;

	HBufC* iMinSecFormatString;
	HBufC* iHourMinSecFormatString;

	TBuf<30> iFileName;
	TBuf<30> iTime;
	bool iRightStop;

	TBool iSystemPaused;

	TBool iIsPlayError;
};
