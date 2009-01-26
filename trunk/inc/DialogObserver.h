/*
============================================================================
 Name        : DialogObserver.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : DialogObserver.h - header file
============================================================================
*/

#ifndef DIALOGOBSERVER_H
#define DIALOGOBSERVER_H

#include "ControlObserver.h"

//调用者调用以下函数时传入自己的指针,使自己能够被删除
class MDialogObserver //: public MDrawEventObserver , public MCleanUpObserver
{
public:
	virtual void DialogEvent(TInt aCommand) = 0;
// 	virtual void DialogConfirm(TInt aCommand) = 0;
// 	virtual void DialogCancel(TInt aCommand) = 0;
};

// class MDialogObserverExtent : public MDialogObserver , public MDrawEventObserver, public MCleanUpObserver
// {
// };

#endif // DIALOGOBSERVER_H

// End of File
