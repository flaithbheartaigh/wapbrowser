/*
============================================================================
 Name        : MControlObserver.h
 Author      : 卞涛
 Version     : 2.10
 Copyright   : soarsky
 Description : 控件需要被继承接口
============================================================================
*/

#ifndef MCONTROLOBSERVER_H
#define MCONTROLOBSERVER_H

//输入框被继承接口
class MInputObserver
{
public:
	virtual void InputResponseEvent(TInt aEvent,const TDesC& aText) = 0;
};
//公告信息提示界面
class MInfoObserver
{
public:
	virtual void InfoResponseEvent(TInt aEvent) = 0;
};

class MSearchListObserver
{
public:
	virtual void SearchListResponseEvent(TInt aEvent,const TDesC& aText) = 0;
};

class MHandleEventObserver
{
public:
	virtual void HandleResponseEvent(TInt aEvent,TInt aType) = 0;
};

class MCopyToObserver
{
public:
	virtual void CopyToEvent(TInt aEvent,const TDesC& aPath) = 0;
};

class MGradeObserver
{
public:
	virtual void GradeEvent(TInt aEvent,TInt aGrade) = 0;
};
class MContentInfoObserver
{
public:
	virtual void ContentInfoEvent(TInt aLeftCommond) = 0;
};
class MLinkListObserver
{
public:
	virtual void LinkListEvent(TInt aEvent,TInt aType) = 0;
};

class MMultiObserver
{
public:
	virtual void MultiEvent(TInt aEvent) = 0;
};
#endif // MCONTROLOBSERVER_H

// End of File
