/*
============================================================================
 Name        : TurkeyListBox.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CListBox declaration

 说明:该类可以根据需要做修改,但不要添加与列表的显示无关的逻辑进来

 待添加功能	:	添加选项,使得选项打开时,当前项展开,显示详细信息
				在每一项的前面显示图标
				添加对CSlide的使用
				使文字居中显示(默认),尽量使此功能与CSlide整合
				限制文字长度的选项,过长的文字用省略号代替
				使图片上下居中

 待改进		:	使数据与界面完全分离,即CListBox中不保存要显示的内容与每项的具体信息
				使每一项与列表分离,以便定制列表
				
				将此类再抽象(是否有必要)

				考虑是否有需要简化接口


				如何保证用户代码传入的CListBoxItem是同一种类型
============================================================================
*/

#ifndef __LISTBOX_H__
#define __LISTBOX_H__

#include "Control.h"
#include "ListBoxItem.h"
#include "PreDefine.h"
#include "ControlObserver.h"


class CScrollBar;

//////////////////////////////////////////////////////////////////////////
//CListBox
//////////////////////////////////////////////////////////////////////////
class CListBox : public CControl , public MDrawEventObserver
{
friend class CListBoxItem;
//private:
public: // Constructors and destructor
	CListBox(MDrawEventObserver& aObserver,const TRect& aRect,const CFont* aFont,TInt aScrollWidth);
	~CListBox();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);
	virtual	void	ActivateL();
	virtual	void	Deactivate();

public://From MDrawEventObserver
	virtual void ControlRequestDraw();

public:
	//向ListBox中添加一个CListBoxItem类型的指针,
	//同时把该指针的拥有权交给ListBox,当
	//调用Remove、Reset等方法时,ListBox会删除这些实例
	void	AppendL(CListBoxItem* aItem);		

	void	Remove(TInt aIndex);

	void	RemoveCurItem();

	void	Reset();

	TInt	Count()const;				//返回项目的个数

	TInt	CurItemIndex()const;	

	CListBoxItem&	GetItem(TInt aIndex) const;	//获取指定项
	CListBoxItem&	CurItem() const;	//获取当前列表项

	CScrollBar*	ScrollBar()const;		//返回滚动条,以便自定义配置

	void	SetTextColor(const TRgb& aTextColor,const TRgb& aTextColorWhenFocus);

	//void	SetBackgroundColor(const)

	void	Layout();

	void	SetUseHotKey(TBool aUseHotKey);

	//卞涛增加,获取当前界面可显示的行数,在外面调用判断是否需要翻屏
	TInt	GetScreenMaxLine(){ return iMaxLinePerPage; }
private:
	void	CalculateSize();
	void	ActiveCurItem();
	void	DeactivateCurItem();

	void	LayoutScrollBar();

	void	Assert()const;	

private:
	RPointerArray<CListBoxItem>		iItemArray;
	CScrollBar*			iScrollBar;
	const CFont*		iFont;

	TRect				iClientRect;
	TSize				iItemSize;

	//绘制文本的颜色,有待扩展,以便配置子项
	TRgb				iTextColor;
	TRgb				iTextColorWhenFocus;

	TInt				iCurScreenIndex;		//相对索引

	TInt				iLineHeight;			//行距
	TInt				iMaxLinePerPage;
	TInt				iScrollWidth;

	TBool				iAlreadyLayout;			//在使用前必须调用Layout(),否则不能正常执行
	TBool				iUseHotKey;
};

#endif // __LISTBOX_H__
