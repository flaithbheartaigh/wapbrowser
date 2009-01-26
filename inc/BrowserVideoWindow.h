
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
	//������ʵ�ּ�����ͼ
	virtual void	DoActivateL();
	//������ʵ�ֶ�����ͼ
	virtual void	DoDeactivate();
	//������ʵ�ֻ�ͼ
	virtual void	DoDraw(CGraphic& aGraphic)const;
	//������ʵ�ְ����¼���Ӧ
	virtual TBool	DoKeyEventL(TInt aKeyCode);

	//������ʵ�������¼���Ӧ
	virtual TBool	DoHandleCommandL(TInt aCommand);
	//������ʵ�ֽ���ߴ�ı�
	virtual void	DoSizeChanged();

	//������ʵ���������Ұ�ť,�Ա㸲�ǰ�ť�Ŀؼ�����ʱ�ظ�ԭ��
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
