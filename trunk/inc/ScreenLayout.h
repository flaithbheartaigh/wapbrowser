/*
============================================================================
 Name        : ScreenLayout.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CScreenLayout declaration

 功能		 : 定义界面布局
============================================================================
*/

#ifndef SCREENLAYOUT_H
#define SCREENLAYOUT_H

// INCLUDES
#include "PreDefine.h"

class CMainEngine;

class CScreenLayout : public CBase
{
public: // Constructors and destructor
	~CScreenLayout();
	static CScreenLayout* NewL(CMainEngine* aEngine);
	static CScreenLayout* NewLC(CMainEngine* aEngine);

private:
	CScreenLayout(CMainEngine* aEngine);
	void ConstructL();

public:

public:
	void	SetFont(const CFont* aDefaultFont);
	const CFont* Font()const;

	void	SetClientRect(const TRect& aRect);	//设置顶部状态栏与底部控制栏之间的客户区域

	void	SetTitleBarRect(const TRect& aRect);

	const TRect&	TitleBarRect() const;

	TRect	GetStatusPaneRect()const;	//顶部状态栏矩形

	TRect	GetFullClientRect()const;	//顶部状态栏与底部控制栏之间的完整客户区域

	TRect	GetClientRect()const;		//顶部状态栏与底部控制栏之间的客户区域

	TRect	GetControlPaneRect()const;	//底部控制栏矩形

	TPoint	GetNaviPanePoint()const;	//未使用
	TSize	GetNaviPaneSize()const;		//未使用
	TRect	GetNaviPaneRect()const;		//导航栏与标题栏矩形

	TPoint	GetListBoxPoint()const;		//未使用
	TSize	GetListBoxSize()const;		//未使用
	TRect	GetListBoxRect()const;		//未使用

	TRect	GetDialogRect()const;		//未使用

	TRect	GetDialogIconRect()const;	//未使用

	TRect	GetScrollbarRect()const;	//未使用

	TRect	LeftButtonRect()const;			//未使用

	TRect	RightButtonRect()const;			//未使用		

	TRect	MultiTextRect()const;			//未使用
	
	TRect	WhiteBackgroundRect()const;		//透明白底的矩形,列表与其它内容一般显示在这个矩形内

	TRect	InfoRect()const;				//消息矩形

	TSize	PopMenuItemSize()const;			

	TInt	TextLineHeight()const;			//文字行间距
				
	TInt	LineHeight()const;				//列表行距

	TInt	LineNum()const;					//列表在一屏内显示的行数

	TInt	FontHeight()const;							//默认字体的高度
	TInt	ScrollBarWidth()const;						//滚动条宽度

	TInt	IconNumPerLine()const;						
	TInt	IconLineNum()const;

	//因为不使用浮点数,用两个乘数因子表示比例
	TInt	Multiple()const;							//进行缩放时的乘数
	TInt	Divisor()const;								//进行缩放时的除数
	//TSize	ScaleFixToNewScreen(CFbsBitmap* aBitmap);

	TInt	InnerMargin() const;								//内页边距

	TInt	OuterMargin() const;
	//TInt	AttributeOffset()const;
	void	GetAttributeOffset(TInt& aOffset1,TInt& aOffset2)const;

	void	GetLoadingTextPos(TInt& aFirstTextPos,TInt& aSecondTextPost)const;

	void	GetOfferRewardEditorHeight(TInt& aHeight1,TInt& aHeight2)const;

	void	GetSaveAsPos(TPoint& aTitlePos,TPoint& aListPos)const;

	TSize	ScaleByDefaultRatio(CFbsBitmap* aBitmap)const;			//按默认比例缩放

	//截取图标后对其进行缩放
	//该方法不能在Draw函数里使用
	//否则图片将被持续放大或缩小
	CFbsBitmap*	GetIconL(TInt aId,TInt aMaxXIndex = 1,TInt aMaxYIndex = 1,TInt aXIndex = 0,TInt aYIndex = 0)const;

	CFbsBitmap*	GetIconL(const TDesC& aFileName,TInt aId,TInt aMaxXIndex = 1,TInt aMaxYIndex = 1,TInt aXIndex = 0,TInt aYIndex = 0)const;

/*
	CFbsBitmap*	GetIconL(const TDesC& aFileName,TInt aId,const TRect& aSrcRect)const;	//截取图标后对其进行缩放
																						//该方法不能在Draw函数里使用
																						//否则图片将被持续放大或缩小
*/

public:
	static	TSize	ScreenSize();									//屏幕尺寸
	static	TSize	ScaleFitToScreenWidth(CFbsBitmap* aBitmap);		//以屏幕宽度为准进行缩放
	static	TSize	ScaleFitToScreenHeight(CFbsBitmap* aBitmap);	//以屏幕高度为准进行缩放
	static	TSize	ScaleWithShortSide(CFbsBitmap* aBitmap);		//以短边为基准进行缩放
	//static	TSize	Scale(CFbsBitmap* aBitmap,TInt)

private:
	void	InitData();
	void	InitByScreenSize();			//根据屏幕尺寸设定	
	void	InitByFont();				//根据字体尺寸设定

	//void	InitDialogRect();			//初始化对话框的矩形

private:
	CMainEngine*	iEngine;
	const	CFont*	iDefaultFont;

	TRect	iStatusPaneRect;
	TRect	iFullClientRect;
	TRect	iClientRect;
	TRect	iControlPaneRect;

	TRect	iTitleBarRect;

	TRect	iNaviPaneRect;
	TRect	iListBoxRect;
	TRect	iDialogRect;
	TRect	iDialogIconRect;

	TRect	iScrollbarRect;

	TRect	iLeftButtonRect;
	TRect	iRightButtonRect;

	TRect	iMultiTextRect;

	TRect	iWhiteBackgroundRect;

	TRect	iInfoRect;

	TSize	iPopMenuItemSize;

	TInt	iFontHeight;

	TInt	iTextLineHeight;

	TInt	iListBoxLineHeight;

	TInt	iLineNum;

	//////////////////////////////////////////////////////////////////////////
	
	TInt	iScrollBarWidth;

	TInt	iIconNumPerLine;
	TInt	iIconLineNum;
	TInt	iInnerMargin;
	TInt	iOuterMargin;

	TInt	iMultiple;			//以176*208屏幕为基准的倍数
	TInt	iDivisor;			//以176*208屏幕为基准的除数

	TInt	iLoadingTextPos_1;	//程序启动界面第一行文件显示的垂直位置
	TInt	iLoadingTextPos_2;	//程序启动界面第一行文件显示的垂直位置

	TInt	iAttributeOffset1;
	TInt	iAttributeOffset2;

	//TInt	iSaveAsTitlePos;
	TPoint	iSaveAsTitlePos;	//另存为,选择程序对话框中标题的绘制位置
	TPoint	iSaveAsListPos;		//另存为,选择程序对话框中列表的绘制位置

	//TInt	iOfferRewardEditorLineHeight;
	TInt	iOfferRewardEditorHeight1;
	TInt	iOfferRewardEditorHeight2;
};

#endif // SCREENLAYOUT_H
