/*
============================================================================
 Name        : PopUpMenuObserver.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : PopUpMenuObserver.h - header file
============================================================================
*/

#ifndef POPUPMENUOBSERVER_H
#define POPUPMENUOBSERVER_H

class CControl;
//aCommand表示要执行的命令索引
//aControl是调用者传入自己的指针,使自己可以被删除
class MPopUpMenuObserver
{
public:
	virtual void DoPopUpMenuCommand(TInt aCommand) = 0;
	virtual void PopUpDestried(){};
};

#endif // POPUPMENUOBSERVER_H

// End of File
