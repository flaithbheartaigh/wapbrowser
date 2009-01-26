
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

	TInt               iCurrentLine; //��ǰ��

	TFileName          iFileName;

	CBookEngine*            iBookEngine; //load 

	//	Pointer array
	RPointerArray<HBufC>   iCurrentArrayBuf;//��ǰ��ʾ��

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
