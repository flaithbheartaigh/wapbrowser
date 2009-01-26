/*
============================================================================
 Name        : PageControlObserver.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : PageControlObserver.h - header file
 说明		 : 用于页控制时的回调
============================================================================
*/

#ifndef PAGECONTROLOBSERVER_H
#define PAGECONTROLOBSERVER_H

class MPageControlObserver
{
public:
	virtual void	GetNewArray(TInt aIndex) = 0;
	virtual void	Update() = 0;
};

#endif // PAGECONTROLOBSERVER_H
