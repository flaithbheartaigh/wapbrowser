/*
============================================================================
 Name        : Window.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWindow declaration

 描述	:	界面基类,所有全屏幕显示的界面类均从此类继承,此类是一个抽象类,不
			可以被实例化
============================================================================
*/

#ifndef WINDOW_H
#define WINDOW_H

#include <e32std.h>
#include <e32base.h>
#include <badesca.h>

#include "ControlObserver.h"


//#include "Control.h"

class CMainEngine;
//class CWindowFactory;
class CScreenLayout;
class CControlFactory;
class CGraphic;
class CControl;
class CControlText;
class CFbsBitmap;
class MDialogObserver;

class CWindow : public CBase , public MDrawEventObserver , public MCleanUpObserver
{
public: // Constructors and destructor
	//CWindow(const CMainEngine& aMainEngine);
	CWindow(TInt aWindowType,CWindow* aParent,CMainEngine& aMainEngine);
	virtual ~CWindow();

public:
//////////////////////////////////////////////////////////////////////////
//ActivateL()与Deactivate()
//当设置一个子界面或其它操作使本界面不再显示时使用Deactivate(),将一些暂时不
//用的资源释放掉,本界面重新获得显示时调用ActivateL()重新启用资源
//
//用于优化内存
//////////////////////////////////////////////////////////////////////////
	//激活视图
	void	ActivateL();
	//冻结视图,不能在基类的析构函数里调用该函数,因为派生类子对象的析构函数先调用,DoDeactivate()函数将失去绑定,导致访问空的虚函数指针
	void	Deactivate();

	void	Draw(CGraphic& aGraphic)const;
	TBool	KeyEventL(TInt aKeyCode);
	TBool	HandleCommandL(TInt aCommand);
	void	SizeChanged();
// 	void	NotifyL(const TDesC& aDes);

	//初始化左右按键,可考虑修改该方法,在初始化时传入要处理的命令
	void	SetLeftButtonL(const TDesC& aDes);
	void	SetRightButtonL(const TDesC& aDes);

	void	ResetLeftButton();
	void	ResetRightButton();

	//返回窗体类型
	TInt	GetWindowType()const;

	//直接调用CreateChildWindow,并传回子Window的指针
	CWindow*	MakeChildWindow(TInt aWindowType);

	//获取父窗体
	CWindow*	GetParentWindow()
	{
		return iParentWindow;
	}
//////////////////////////////////////////////////////////////////////////
//模板方法留给派生类的实现步骤
//////////////////////////////////////////////////////////////////////////
private:
	//派生类实现激活视图
	virtual void	DoActivateL() = 0;
	//派生类实现冻结视图
	virtual void	DoDeactivate() = 0;
	//派生类实现绘图
	virtual void	DoDraw(CGraphic& aGraphic)const = 0;
	//派生类实现按键事件响应
	virtual TBool	DoKeyEventL(TInt aKeyCode) = 0;
	//派生类实现命令事件响应
	virtual TBool	DoHandleCommandL(TInt aCommand) = 0;
	//派生类实现界面尺寸改变
	virtual void	DoSizeChanged() = 0;

	//派生类实现设置左右按钮,以便覆盖按钮的控件返回时回复原样
	virtual void	ChangeButton() = 0;

/*
	//派生类实现网络消息处理,待研究/有的界面不需要处理网络事件,所以该方法不定义为纯虚函数
	virtual void	DoNotifyL(const TDesC& aDes){};
	*/
//////////////////////////////////////////////////////////////////////////
//实现以下两个接口以响应控件的请求
//////////////////////////////////////////////////////////////////////////
public://From MDrawEventObserver
	virtual void ControlRequestDraw();

public://From MCleanUpObserver
	virtual void CleanupControl(CControl* aControl);
//////////////////////////////////////////////////////////////////////////
//聚合控制
//////////////////////////////////////////////////////////////////////////
public:
	//添加一个新控件
	TBool	AddControl(const CControl* aControl);	
	//移除控件
	TBool	RemoveControl(const CControl* aControl);	
	//移除并删除控件
	TBool	RemoveAndDeleteControl(const CControl* aControl);	

// 	void	DeleteControl(const CControl* aControl);	//移除并删除控件
 	void	DeleteAllControl();

	void	ActivateAllControlL();

	void	DeactivateAllControl();

private:
	void	DrawEachControl(CGraphic& aGraphic)const;			//依序画出所有控件
	TBool	HandleEachControlKeyL(TInt aKeyCode);				//依序处理所有控件的按键事件
	TBool	HandleEachControlCommandL(TInt aCommand);			//依序处理所有控件的命令事件
	void	EachControlSizeChanged();							//处理所有控件的屏幕改变事件

//////////////////////////////////////////////////////////////////////////
//被模板方法调用
//////////////////////////////////////////////////////////////////////////
protected:
	void	DrawBackground(CGraphic& aGraphic)const;
	void	DrawButton(CGraphic& aGraphic)const;

//////////////////////////////////////////////////////////////////////////
//其它函数,留给派生类使用
//////////////////////////////////////////////////////////////////////////
protected:
	//////////////////////////////////////////////////////////////////////////
	//界面控制函数
	//////////////////////////////////////////////////////////////////////////
	//返回到父界面,调用该函数以后不应再调用其它成员,以免引发错误
	void	GoBackToParentWindow();

	//返回到主界面,调用该函数以后不应再调用其它成员,以免引发错误
	void	GoBackToMainWindow();

	//根据给定的窗口类型创建一个界面，并将其设为子界面
	void	CreateChildWindow(TInt aWindowType);

	//清空子界面
	void	ResetChildWindow();

	void	SetChildWindow(CWindow* aWindow);
	//////////////////////////////////////////////////////////////////////////
	//其它便利函数
	//////////////////////////////////////////////////////////////////////////

	//配置界面布局
	void	SetLayout();
	//获取布局数据,以配置屏幕布局,例如字体高度,行高,屏幕宽度等..
	void	GetLayout();								

	//////////////////////////////////////////////////////////////////////////
	//考虑将这一类特殊方法移出该基类,在其它地方统一处理,例如CMainEngine
	//这种背景可以做成控件
	//////////////////////////////////////////////////////////////////////////
	void	CreateWhiteBackgroundL();
	void	DrawWhiteBackground(const TRect& aRect)const;

	//初始化应该移入CControlFactory中
	void	CreateControlText();
	//void	ShowDialog();

/*
	//处理特殊界面,如果此类界面比较多时如何处理?
	TBool	HandleOpenDownloadListWindow(TInt aKeyCode);
	TBool	HandleOpenMusicWindow(TInt aKeyCode);
*/

	TInt	AddDownLoadList(MDialogObserver& aObserver,TInt aCommond,const TDesC& aUrl, const TDesC& aName,const TInt aFileLen,const TInt aDownLoadLen,const TInt aType,const TInt aSave);

protected:
	CMainEngine&		iMainEngine;

	TBool				iIsNormalWindow;		//普通窗口,即含有正常背景、标题栏、控制栏的窗口
	TBool				iCanPopUpWindow;			//说明此窗口是否可以响应#*0弹出窗口

	TBool				iShowControlText;

	//CArrayFixFlat<TBuf<32>>* iTextRes;

//	const CWindowFactory&	iWindowFactory;
// 	TBool				iDrawBackground;
// 	TBool				iDrawButton;
// 	const CScreenLayout&	iScreenLayout;
// 	const CControlFactory&	iControlFactory;

	CControlText*		iControlText;		//考虑移到CMainEngine中
private:
	TInt				iWindowType;

	CWindow*			iParentWindow;
	CWindow*			iChildWindow;

	RPointerArray<CControl>	iControlArray;

// 	CDesCArray*			iLeftButtonTextArray;
// 	CDesCArray*			iRightButtonTextArray;

	TBuf<6>				iLeftButtonText;
	TBuf<6>				iRightButtonText;

	CFbsBitmap*			iBackgroundBmp;
	CFbsBitmap*			iControlPaneBmp;

	//iActivated,避免ActivateL()和Deactivate()被重复调用
	TBool				iIsActivated;
};

#endif // WINDOW_H
