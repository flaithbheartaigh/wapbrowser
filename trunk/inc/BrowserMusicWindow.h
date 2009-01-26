
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


