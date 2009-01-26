/*
============================================================================
 Name        : MainEngine.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CMainEngine declaration
============================================================================
*/

#ifndef MAINENGINE_H
#define MAINENGINE_H

#include "MainControlEngine.h"
#include "ControlObserver.h"
#include "CacheObserver.h"

class CCoCoAppUi;
class CCoCoAppView;

class CWindow;
class CMainWindow;
class CWindowFactory;
class CControlFactory;
class CEditorsManager;
class CFileEngine;
class CFileExecuter;
class CGraphic;
class CCoeControl;
class CDownLoadEngine;
class CDownLoadHistoryEngine;
class CHttpManager;
class CBitmapFactory;
class CCacheEngine;
class CTestEngine;
class CMusicPlayManageEngine;

class MCacheObserver;
class CHandleSynchronization;

class CMainEngine : public CMainControlEngine , public MDrawEventObserver , public MCleanUpObserver , public MCacheObserver
{
public: // Constructors and destructor
	~CMainEngine();
	static CMainEngine* NewL(CCoCoAppView& aAppView);

private:
	CMainEngine(CCoCoAppView& aAppView);
	void ConstructL();

//////////////////////////////////////////////////////////////////////////
//实现以下两个接口以响应控件的请求
//////////////////////////////////////////////////////////////////////////
public://From MDrawEventObserver
	virtual void ControlRequestDraw();

public://From MCleanUpObserver
	virtual void CleanupControl(CControl* aControl);

public://From MCacheObserver
	virtual void	CacheUpdate(const TDesC&);
	virtual	void	CacheFailed();
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
public:
	inline const TDesC& GetV(){return iV;}
	inline const TDesC& GetR(){return iR;}
	inline const TDesC& GetA(){return iA;}
	inline const TDesC& GetP(){return iP;}

	CBitmapFactory&		GetBitmapFactory();

	inline CHttpManager*	GetHttpManager() 
	{
		return iHttpManager;
	}

	TInt ParseMobiWapPage(const TDesC& mUrl,const TDesC& mBodyData,HBufC* rUrl);

	void	SetMainWindow(CMainWindow* aMainWindow);

	const CWindowFactory&	WindowFactory() const;

	CControlFactory&	ControlFactory() const;

	const CScreenLayout&	ScreenLayout() const;

	CFileEngine&			FileEngine() const;

	CFileExecuter&			FileExecuter(CWindow* aParentWindow) const;
	
	CEditorsManager&	EditorsManager();

	CMainWindow&		MainWindow() const;

	CMusicPlayManageEngine& MusicPlayManageEngine();

	CGraphic&			Graphic() const;

	CDownLoadEngine&	GetDownLoadEngine();
	CDownLoadHistoryEngine&	GetDownLoadHistoryEngine();

	void	RequestDraw();

	void	SetCurWindow(CWindow* aWindow);

	CWindow*	CurWindow() const;

	void	GoBackToMainWindow();

	TFileName	GetMbmFileName() const;

	CFbsBitmap*	CreateBmpById(TInt aId) const;

	CFbsBitmap*	GetBitmapById(TInt aBitmapId) const;

	const TDesC&	GetDesById(TInt aDesId) const;

	CWindow*	CreateWindow(TInt aWindowType,CWindow* aParent) const;

	//创建一个CCacheEngine的指针并交出其拥有权
	CCacheEngine*	CreateCacheEngineL(MCacheObserver& aObserver,TInt aFirstTypeId,TInt aSecondTypeId = -1, TInt aThirdTypeId = -1);

	CTestEngine&	TestEngine() const;

	void		CleanupCache();

	void		SetScreenRect(const TRect& aRect);

	TRect		ScreenRect() const;

	TSize		ScreenSize() const;

	TInt		GetInputType();

	void		InitHttpManager();

	void		InitDownLoadEngine();

	void		SetClippingRect(const TRect& aRect);

	void		SetClippingRegion(const TRegion& aRegion);

	void		CancelClipping();

	void	 GetControlText(TInt aIndex,TDes& aDes);
	void	SetWeather(const TDesC& aText);
//////////////////////////////////////////////////////////////////////////
public:
	void	GetFileTypeIcon(const TDesC& aFilaName,const CFbsBitmap*& aIcon,const CFbsBitmap*& aIconMask) const;

public:
	TInt GetSmallGWState(){ return iIsSmallGW; } //0不使用小网关，1使用小网关
	void SetSmallGWState(TInt aState){ iIsSmallGW=aState;} 
	CHandleSynchronization& GetSynchronContent(){ return *iSyschr;}
private:
	CCoCoAppUi*		AppUi() const;
public:
	CCoCoAppView*	Container() const;

private:
	void	InitSynchronContent();
	void		InitScreenLayout();
	void		InitMusicPlayManageEngine();
	void		PraseVersion();
private:
	CHttpManager* iHttpManager;

	CWindowFactory*		iWindowFactory;
	CControlFactory*	iControlFactory;
	CEditorsManager*	iEditorsManager;
	CFileEngine*		iFileEngine;
	CFileExecuter*		iFileExecuter;
	
	CMusicPlayManageEngine* iMusicPlayManageEngine;
	CDownLoadEngine*	iDownLoadEngine;
	CDownLoadHistoryEngine*		iDownLoadHistoryEngine;

	CMainWindow*		iMainWindow;

	CGraphic*			iGraphic;

	CCoCoAppView&		iAppView;

	CBitmapFactory*		iBitmapFactory;
	TRect				iScreenRect;

	CHandleSynchronization* iSyschr;
	
	CTestEngine*		iTestEngine;

	TBuf<20> iAppVersion;
	TBuf<5> iV;
	TBuf<5> iR;
	TBuf<5> iA;
	TBuf<5> iP;

	TInt iIsSmallGW;

	TBuf<50> iWeatherText;

};

#endif // MAINENGINE_H

