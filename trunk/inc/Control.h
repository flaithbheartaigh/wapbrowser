/*
============================================================================
 Name        : Control.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CControl declaration

 描述 : 控件基类,所有可显示的组件都从该类继承
============================================================================
*/

#ifndef CONTROL_H
#define CONTROL_H

#include <e32std.h>
#include <e32base.h>


class CGraphic;
class MDrawEventObserver;
class MCleanUpObserver;

class CControl : public CBase
{
protected: // Constructors and destructor
	CControl(TInt aControlType);

public:
	virtual ~CControl();

public:
	virtual void	Draw(CGraphic& aGraphic)const = 0;
	virtual TBool	KeyEventL(TInt aKeyCode) = 0;
	virtual TBool	HandleCommandL(TInt aCommand) = 0;
	virtual void	SizeChanged(const TRect& aScreenRect) = 0;	//待论证

	virtual	void	ActivateL();
	virtual void	Deactivate();

	virtual	const TDesC&	LeftButton() const;
	virtual	const TDesC&	RightButton() const;
	//virtual	TBool	HasButton() const = 0;

public:
	TInt	GetControlType() const;

	//若要使用RequestDraw(),一定要先调用该设值函数
	void	SetDrawEventObserver(MDrawEventObserver* aDrawEventObserver);
	//要使用CleanupSelf(),一定要先调用该设值函数
	void	SetCleanUpObserver(MCleanUpObserver* aCleanUpObserver);

//	void	SetHasButton(TBool aHasButton);

	TBool	HasButton() const;

protected:
	//发起一个重绘请求,由MDrawEventObserverr处理
	void	RequestDraw();
	//请求清除自己,由MCleanUpObserver处理,调用该函数以后不应再其它处理,特别是不能使用成员,否则可能导致访问违例
	void	CleanupSelf();

protected:
	TBool		iIsActive;
	TBool		iHasButton;			

private:
	TInt		iControlType;

	MDrawEventObserver*	iDrawEventObserver;
	MCleanUpObserver*	iCleanUpObserver;

};

#endif // CONTROL_H
