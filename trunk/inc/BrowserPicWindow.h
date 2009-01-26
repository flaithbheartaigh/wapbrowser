
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

	//�Ƿ�ȫ����ʾ
	TBool iFullScreenShow;

	CArrayFixFlat<TFileName>*	iFileNameArray;
	TFileName	iFilePath;
	TFileName	iFileName;
	TInt	iSelIndex;	

	TRect iShowRect;

	TBool iOpenComplete;
};


