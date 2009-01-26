/*
============================================================================
 Name        : MultiText.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CMultiText declaration

 描述		:	绘制多行的文件,例如帮助,电子书等等,使用CScrollbar
 使用说明	:	该类在初始化后必须调用以下三个函数方可使用
				void	SetTextFont(const CFont* aFont);
				void	SetClientRect(const TRect& aRect);
				void	AppendL(const TDesC& aDes);

待添加功能	:	定时的自动滚屏
============================================================================
*/

#ifndef MULTITEXT_H
#define MULTITEXT_H

#include <badesca.h>
#include "Control.h"

#include "PreDefine.h"

class CScrollBar;
class MMultiObserver;

class CMultiText : public CControl
{
public: // Constructors and destructor
	~CMultiText();
	static CMultiText* NewL();
	static CMultiText* NewLC();

private:
	CMultiText();
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);
	

public:
// 	TBool	KeyEventL(TInt aKeyCode);
// 
// 	void	Draw(CBitmapContext& gc)const;

	void	AppendL(const TDesC& aDes);

	void	LoadFromFileL(const TDesC& aFileName);		//装载一个以unicode形式表示的文本文件

	void	SetScrollSize(TInt aScrollWidth,TInt aScrollHeight);

	void	SetClientRect(const TRect& aRect);

	void	SetMargin(TInt aLeftRight,TInt aTopBottom);	

	void	SetTextColor(const TRgb& aColor);

	void	SetTextFont(const CFont* aFont);

	void	Layout(const TRect& aRect,const CFont* aFont,TInt aScrollBarWidth,TInt aRowSpace);						//须先调用SetMargin,SetClientRect

	void	InitScrollbarL();	

	CScrollBar*	Scrollbar()const;			//返回CScrollbar给用户进行设置

	void	AppendEndItem(const TDesC& aText); //卞涛增加最后一行处理20070918
	TBool   GetEndFlag();
	void	SetObserver(MMultiObserver* aObserver);
private:
	void	Assert()const;

private:
	CScrollBar*		iScrollbar;
	CDesCArray*		iDesArray;

	MMultiObserver* iObserver;

	const CFont*	iFont;

	TRect			iClientRect;		//用于绘制文字的区域

	TPoint			iStartPoint;
	TRgb			iTextColor;
	TInt			iMaxLinePerPage;
	TInt			iLineHeight;
	TInt			iLineWidth;			//通过屏幕尺寸与字体大小计算得到
	TInt			iRowSpacing;		//行距
	TInt			iScrollWidth;
	TInt			iScrollHeight;		

	TInt			iLeftRightMargin;	//左右页边距
	TInt			iTopBottomMargin;	//上下页边距

	TBool			iBool; //卞涛增加，是否添加了最后一行的标示20070918，默认识EFalse
	TInt			iLastIndex;
};

#endif // MULTITEXT_H
