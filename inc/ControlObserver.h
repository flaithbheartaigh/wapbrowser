/*
============================================================================
 Name        : ControlObserver.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : 定义用于CControl派生类回调的公共接口,在CWindow中实现

 问题		 : 是否可以将这些接口聚合在一个接口中?
============================================================================
*/

#ifndef CONTROLOBSERVER_H
#define CONTROLOBSERVER_H

class CControl;
//////////////////////////////////////////////////////////////////////////
//MDrawEventObserver
//此接口接收控件的重绘请求,对其做处理,一般是能够定时重绘的控件
//////////////////////////////////////////////////////////////////////////
class MDrawEventObserver
{
public:
	virtual void ControlRequestDraw() = 0;
};
//////////////////////////////////////////////////////////////////////////
//MCleanUpObserver
//此接口接收控件的清除自己的请求,参数为控件的this指针
//////////////////////////////////////////////////////////////////////////
class MCleanUpObserver
{
public:
	virtual void CleanupControl(CControl* aControl) = 0;
};
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class MCommondBase
{
public:
	virtual void Execute() = 0;
};

#endif // CONTROLOBSERVER_H

// End of File
